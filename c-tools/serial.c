#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "serial.h"
#include "layer.h"


#define UART_ACK    0x06
#define UART_NAK    0x15

#define PROT_WSTART 0x80
#define PROT_WSTOP  0x90
#define PROT_WRITE  0xA0

#define MAX_LEN 128
#define WORDSIZE 4
#define IS_ALIGNED(x) (!(x % WORDSIZE))
#define ALIGN(x) ((x + (WORDSIZE -1)) & -WORDSIZE)


#ifdef DEBUG
#define PRINT_RET(x) (printf("%x\n", x))
#else
#define PRINT_RET(x) (x)
#endif

static int set_attrs(int fd, int speed){
    struct termios tty;

    if(tcgetattr(fd, &tty) != 0){
        perror("Error getting attributes");
        return -1;
    }

    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // set to 8 data bits
    tty.c_cflag |= CLOCAL | CREAD;              // local and enable receiver
    tty.c_cflag &= ~CSTOPB;                     // 1 stop bit
    tty.c_cflag &= ~PARENB;                     // disable parity bit
    tty.c_cflag |= CRTSCTS;                     // enable hardware flow control

    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // raw input
    
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);     // disable software flow control
    tty.c_iflag |= IGNBRK;
    tty.c_iflag |= IGNCR;
    tty.c_iflag &= ~INPCK;

    tty.c_oflag &= ~OPOST;  // raw output

    tty.c_cc[VMIN] = 0;     // no block on read
    tty.c_cc[VTIME] = 10;   // 1.0s read timeout

    if(tcsetattr(fd, TCSANOW, &tty) != 0){
        perror("Error setting attributes");
        return -1;
    }

    return 0;

}


/* Writes len bytes from tx_buff to receiver */
static int do_write(int fd, uint8_t *tx_buff, uint8_t len){
    if(len > MAX_LEN || len == 0 || len % 2 != 0){
        return -1;
    }

    uint8_t cmd = 0;
    uint8_t ret = 0;

    cmd = PROT_WRITE;
    write(fd, &cmd, 1);
    read(fd, &ret, 1);

    if(ret == UART_NAK){
        puts("Error: NAK after write cmd");
        return UART_NAK;
    }

    if(ret != UART_ACK){
        puts("Error: no ACK after write cmd");
        return -2;
    }

    /* The receiver will increment len by 1
     * so we need to send len - 1 */
    cmd = len - 1;
    write(fd, &cmd, 1);
    read(fd, &ret, 1);

    if(ret == UART_NAK){
        puts("Error: NAK after length given\nThis probably means your length was too big or 0");
        return UART_NAK;
    }

    if(ret != UART_ACK){
        puts("Error: no ACK after length given");
        return -2;
    }

    /* Receiver expects len bytes so no need to -1 here */
    write(fd, tx_buff, len);
    read(fd, &ret, 1);

    if(ret != UART_ACK){
        puts("Error: write not acknowledged");
        return -2;
    }

    return 0;
}

static int wstart(int fd){
    uint8_t cmd = PROT_WSTART;
    uint8_t ret;
    write(fd, &cmd, 1);
    read(fd, &ret, 1);

    if(ret != UART_ACK){
        puts("Error: write start not acknowledged");
        return -2;
    }

    return 0;
}

static int wstop(int fd){
    uint8_t cmd = PROT_WSTOP;
    uint8_t ret;
    write(fd, &cmd, 1);
    read(fd, &ret, 1);

    if(ret != UART_ACK){
        puts("Error: write stop not acknowledged");
        return -2;
    }

    return 0;
}

int open_port(const char *port){
    int fd = open(port, O_RDWR | O_NOCTTY | O_SYNC);

    if(fd < 0){
        perror("Error opening port");
        return -1;
    }

    set_attrs(fd, B115200);

    return fd;
}

void close_port(int fd){
    close(fd);
}


void dummy_write(const void *tx_buff, uint8_t len){
    if(!IS_ALIGNED(len)){
        uint8_t aligned = ALIGN(len);
        uint8_t pad = aligned - len;

        uint8_t *buff = calloc(sizeof(uint8_t), aligned * sizeof(uint8_t));
        memmove(buff, tx_buff, len);

        for(uint8_t i = 0; i < aligned; ++i){
            printf("%#x ", ((uint8_t*)buff)[i]);
        }

        putchar('\n');
        free(buff);

        return;
    }

    for(uint8_t i = 0; i < len; ++i){
        printf("%#x ", ((uint8_t*)tx_buff)[i]);
    }

    putchar('\n');
}

int write_weights(int fd, struct layer *l){
    size_t kernel_bytes = 0;
    size_t bias_bytes = 0;
    kernel_bytes = l->weights->klen * sizeof(_Float16);
    bias_bytes = l->weights->blen * sizeof(_Float16);

    printf("\nWriting layer %s\n", l->name);

    printf("Kernel length of %zu bytes starting at address %#x\n", kernel_bytes, l->offsets->kernel);
    printf("Will be written in %zu %zu byte blocks and one %zu byte block\n", kernel_bytes / MAX_LEN, MAX_LEN, kernel_bytes % MAX_LEN); 

    if(kernel_bytes % MAX_LEN != 0){
        size_t blocks = kernel_bytes / MAX_LEN;

        for(size_t i = 0; i < blocks; ++i){
            dummy_write(l->weights->kernel + (i * MAX_LEN / sizeof(_Float16)), MAX_LEN);
        }

        dummy_write(l->weights->kernel + ((blocks * MAX_LEN) / sizeof(_Float16)), kernel_bytes % MAX_LEN);

    }

    if(bias_bytes == 0){
        return 0;
    }

    printf("Bias length of %zu bytes starting at address %#x\n", bias_bytes, l->offsets->bias);
    printf("Will be written in %zu %zu byte blocks and one %zu byte block\n", bias_bytes / MAX_LEN, MAX_LEN, bias_bytes % MAX_LEN); 

    if(bias_bytes % MAX_LEN != 0){
        size_t blocks = bias_bytes / MAX_LEN;

        for(size_t i = 0; i < blocks; ++i){
            dummy_write(l->weights->bias + (i * MAX_LEN / sizeof(_Float16)), MAX_LEN);
        }

        dummy_write(l->weights->bias + ((blocks * MAX_LEN) / sizeof(_Float16)), bias_bytes % MAX_LEN);

    }


}


