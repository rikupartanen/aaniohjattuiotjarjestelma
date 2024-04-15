#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>

#include "test.h"

#define UART_ACK    0x06
#define UART_NAK    0x15

#define PROT_WSTART 0x80
#define PROT_WSTOP  0x90
#define PROT_WRITE  0xA0

#define MAX_LEN 128


#ifdef DEBUG
#define PRINT_RET(x) (printf("%x\n", x))
#else
#define PRINT_RET(x) (x)
#endif

int set_attrs(int fd, int speed){
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
int do_write(int fd, const _Float16 *tx_buff, uint8_t len){
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

int wstart(int fd){
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

int wstop(int fd){
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


int main(int argc, char *argv[]){
    char *port = "/dev/ttyACM0";
    int fd = open(port, O_RDWR | O_NOCTTY | O_SYNC);

    if(fd < 0){
        perror("Error opening port");
        return -1;
    }

    set_attrs(fd, B115200);

    int ret;

    puts("Sending WSTART");
    ret =  wstart(fd);
    
    if(ret){
        return -1;
    }

    printf("Writing %zu bytes\n", dense.size);
    ret = do_write(fd, dense.values, dense.size);
    if(ret){
        return -1;
    }

    puts("Sending WSTOP");
    ret = wstop(fd);
    if(ret){
        return -1;
    }


    close(fd);
    puts("Done");
    return 0;
}
