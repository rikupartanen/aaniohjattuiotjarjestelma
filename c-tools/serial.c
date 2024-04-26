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
#include "floattype.h"


/* These are just the ASCII control codes for ACK and NAK */
#define UART_ACK    0x06
#define UART_NAK    0x15

/* Custom defined control codes for file transfer */
#define PROT_WSTART 0x80
#define PROT_WSTOP  0x90
#define PROT_WRITE  0xA0
#define PROT_ERASE  0xB0

#define MAX_LEN 128
#define WORDSIZE 4
#define IS_ALIGNED(x) (!(x % WORDSIZE))
#define ALIGN(x) ((x + (WORDSIZE -1)) & -WORDSIZE)


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

    tty.c_cc[VMIN] = 1;     // block on read
    tty.c_cc[VTIME] = 10;   // 1.0s read timeout

    if(tcsetattr(fd, TCSANOW, &tty) != 0){
        perror("Error setting attributes");
        return -1;
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

    puts("Wstart acknowledged");

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

    puts("Wstop acknowledged");

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


int do_write(int fd, const void *tx_buff, uint8_t len){
    int retval = 0;
    uint8_t cmd = 0;
    uint8_t ret = 0;
    uint8_t *buff = NULL;

    uint8_t aligned = len;

    /* in case our len isn't aligned, align it
     * and allocate a new buffer with padding */
    if(!IS_ALIGNED(len)){
        aligned = ALIGN(len);
        buff = calloc(sizeof(uint8_t), aligned * sizeof(uint8_t));
        memmove(buff, tx_buff, len);
    }else{
        buff = (uint8_t*)tx_buff;
    }


    cmd = PROT_WRITE;
    write(fd, &cmd, 1);
    read(fd, &ret, 1);

    if(ret == UART_NAK){
        puts("Error: NAK after write cmd");
        retval = UART_NAK;
        goto end;
    }

    if(ret != UART_ACK){
        puts("Error: no ACK after write cmd");
        printf("Got %#x instead\n", ret);
        retval = -2;
        goto end;
    }

    /* The receiver will increment len by 1
     * so we need to send len - 1 */

    cmd = aligned - 1;
    write(fd, &cmd, 1);
    read(fd, &ret, 1);

    if(ret == UART_NAK){
        puts("Error: NAK after length given\nThis probably means your length was too big or 0");
        retval = UART_NAK;
        goto end;
    }

    if(ret != UART_ACK){
        puts("Error: no ACK after length given");
        retval = -2;
        goto end;
    }

    /* Receiver expects len bytes so no need to -1 here */
    write(fd, buff, aligned);
    read(fd, &ret, 1);

    if(ret != UART_ACK){
        puts("Error: write not acknowledged");
        retval = -2;
        goto end;
    }

    retval = 0;
end:
    /* We only had to allocate in case our len wasn't aligned */
    if(len != aligned){
        free(buff);
    }

    return retval;
}

void erase_chip(int fd){
    char cmd = PROT_ERASE;
    char ret = 0;
    write(fd, &cmd, 1);
    read(fd, &ret, 1);
    if(ret != UART_ACK){
        puts("Erase chip: did not get ACK");
    }
}


int write_weights(int fd, struct layer *l){
    size_t kernel_bytes = 0;
    size_t bias_bytes = 0;
    kernel_bytes = l->weights->klen * sizeof(FLOAT_T);
    bias_bytes = l->weights->blen * sizeof(FLOAT_T);

    printf("\nWriting layer %s\n", l->name);

    printf("Kernel length of %zu bytes starting at address %#x\n", kernel_bytes, l->offsets->kernel);
    printf("Will be written in %zu %zu byte blocks and leftover %zu byte block\n", kernel_bytes / MAX_LEN, MAX_LEN, kernel_bytes % MAX_LEN); 

    wstart(fd);

    /* if data doesn't fit neatly into blocks */
    if(kernel_bytes % MAX_LEN != 0){
        size_t blocks = kernel_bytes / MAX_LEN;

        printf("Starting write of blocks\n");

        /* First write out all our 128 byte blocks */
        for(size_t i = 0; i < blocks; ++i){
            /* weights->kernel is a float array so we must go by elements here, not bytes */
            do_write(fd, l->weights->kernel + (i * MAX_LEN / sizeof(FLOAT_T)), MAX_LEN);
        }

        /* and then whatever is left over */
        printf("Starting write of leftover\n");
        do_write(fd, l->weights->kernel + ((blocks * MAX_LEN) / sizeof(FLOAT_T)), kernel_bytes % MAX_LEN);
    }else{
        /* if all our data fits neatly in 128 byte blocks just write them */
        size_t blocks = kernel_bytes / MAX_LEN;
        for(size_t i = 0; i < blocks; ++i){
            do_write(fd, l->weights->kernel + (i * MAX_LEN / sizeof(FLOAT_T)), MAX_LEN);
        }
    }

    /* Bias can in some cases be 0 */
    if(bias_bytes == 0){
        goto end;
    }

    printf("Bias length of %zu bytes starting at address %#x\n", bias_bytes, l->offsets->bias);
    printf("Will be written in %zu %zu byte blocks and leftover %zu byte block\n", bias_bytes / MAX_LEN, MAX_LEN, bias_bytes % MAX_LEN); 

    /* same as with kernel */
    if(bias_bytes % MAX_LEN != 0){
        size_t blocks = bias_bytes / MAX_LEN;

        for(size_t i = 0; i < blocks; ++i){
            do_write(fd, l->weights->bias + (i * MAX_LEN / sizeof(FLOAT_T)), MAX_LEN);
        }

        do_write(fd, l->weights->bias + ((blocks * MAX_LEN) / sizeof(FLOAT_T)), bias_bytes % MAX_LEN);

    }else{
        size_t blocks = bias_bytes / MAX_LEN;
        for(size_t i = 0; i < blocks; ++i){
            do_write(fd, l->weights->bias + (i * MAX_LEN / sizeof(FLOAT_T)), MAX_LEN);
        }
    }

end:
    printf("Done writing %s\n", l->name);
    wstop(fd);
    return 0;

}


