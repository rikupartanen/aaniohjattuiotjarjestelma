#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/flash.h>
#include <stdint.h>
#include <zephyr/devicetree.h>
#include <zephyr/device.h>
#include <nrfx_qspi.h>
#include <zephyr/drivers/uart.h>
#include <stdlib.h>

#include "qspi.h"

#define UART_ACK        0x6
#define UART_NAK        0x15

#define UART_BAUD       115200

/* File transfer commands */
#define PROT_WSTART     0x80
#define PROT_WSTOP      0x90
#define PROT_WRITE      0xA0
#define PROT_SEEK       0


#define EXTFLASH_SIZE 0x800000
#define WORDSIZE 4
#define IS_ALIGNED(x) (!(x % WORDSIZE))

enum state{
    STATE_IDLE,
    STATE_WRITE,
    STATE_ERASE,
    STATE_READ
};


/* Pointer will keep track of where we are writing in memory */
static uint32_t g_writeptr = 0x0;

const struct device *uart_dev = DEVICE_DT_GET(DT_NODELABEL(cdc_acm_uart));

static inline void uart_send_ack(){
    uart_poll_out(uart_dev, UART_ACK);
}

static inline void uart_send_nak(){
    uart_poll_out(uart_dev, UART_NAK);
}

static inline void uart_print(char *s){
    for(; *s; s++){
        uart_poll_out(uart_dev, *s);
    }
}

void dummy_write(uint32_t *tx_buff, size_t tx_len, uint32_t dst){
/*    for(int i = 0; i < tx_len / sizeof(uint32_t); ++i){
        test[i] = tx_buff[i];
    }
    return;*/

    qspi_write(tx_buff, tx_len, dst);
}

int write_mode(){
    uart_send_ack();
    char tmp = 0;
    uint8_t len = 0;
    uint8_t rx_buff[128] = {0};

    while((uart_poll_in(uart_dev, &tmp)) != 0);

    /* See if write length is in an acceptable range (0-127)
     * NOTE: this may be changed to check for a crc enable given with data len */
    if(tmp > 0x7F){
        uart_send_nak();
        return -1;
    }

    uart_send_ack();

    /* lengths of 0 are not a thing, 
     * assume write always wants to write at least 1 byte */
    len = tmp + 1;

    for(uint8_t i = 0; i < len; ++i){
        while((uart_poll_in(uart_dev, &rx_buff[i]) != 0));
    }

    /* Make sure our len is actually word-aligned 
     * and round up if required 
     * transmitter side should have already sorted this */
    len = (len + (WORDSIZE - 1)) & -WORDSIZE;

    /* We can use malloc() here since we're always copying a multiple of 4 bytes
     * from rx_buff which has been initialized to 0 */
    uint32_t *tx_buff = malloc(len);
    memcpy(tx_buff, rx_buff, len);
    dummy_write(tx_buff, len, g_writeptr);

    free(tx_buff);

    g_writeptr += len;

    uart_send_ack();
    return 0;
}

/* These 2 are a bit unnecessary, but in case
 * any functionality past an ACK is needed, they're here */
static inline void wstart(){
    uart_send_ack();
}

static inline void wstop(){
    uart_send_ack();
}


int main(void){	
    qspi_init();
//    qspi_erase(0x0, ERASE_ALL);
    static enum state state = STATE_IDLE;
    char input = 0;

    const struct uart_config uart_cfg = {
        .baudrate = UART_BAUD,
        .parity = UART_CFG_PARITY_NONE,
        .stop_bits = UART_CFG_STOP_BITS_1,
        .data_bits = UART_CFG_DATA_BITS_8,
        .flow_ctrl = UART_CFG_FLOW_CTRL_RTS_CTS,
    };

    uart_configure(uart_dev, &uart_cfg);

    while(1){
        input = 0;
        while((uart_poll_in(uart_dev, &input)) != 0);

        switch(input){
            case PROT_WSTART:
                state = STATE_WRITE;
                wstart();
                break;
            case PROT_WSTOP:
                state = STATE_IDLE;
                wstop();
                break;
            case PROT_WRITE:
                if(state == STATE_WRITE){
                    int ret = write_mode(); 
                    /* go back to idle if write fails */
                    if(ret != 0)
                        state = STATE_IDLE;
                }
                break;
        }

    }

    return 0;
}

