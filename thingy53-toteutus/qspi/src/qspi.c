#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/flash.h>
#include <nrfx_qspi.h>

#include "qspi.h"

#define EXTFLASH_SIZE 0x800000
#define WORDSIZE 4

/* gcc optimizes this to a tst instruction 
 * so use modulo because it's a little bit clearer */
#define IS_ALIGNED(x) ( !(x % WORDSIZE) )


/* Assembly functions */
extern void _qspi_configure(void);
extern void _qspi_activate(void);
extern void _qspi_write(uint32_t *tx_buff, size_t tx_len, uint32_t dst);
extern void _qspi_read(uint32_t *rx_buff, size_t rx_len, uint32_t src);
extern void _qspi_erase(uint32_t start, enum erase_len len);
extern uint32_t _qspi_status();


/* Checks ready bit, must be done before read/write/erase operations */
static inline void qspi_wait_ready(){
    while(_qspi_status() & 0x4);
}


int qspi_read(uint32_t *rx_buff, size_t rx_len, uint32_t src){
    /* Both rx_len and src need to be word-aligned */
    if(!IS_ALIGNED(rx_len) || !IS_ALIGNED(src)){
        return -1;
    }

    /* Make sure read doesn't start at the end of flash
     * and that read wouldn't run past the end of flash */
    if(src >= EXTFLASH_SIZE || (src + rx_len) > EXTFLASH_SIZE){
        return -2;
    }

    qspi_wait_ready();
    _qspi_read(rx_buff, rx_len, src);
    return 0;
}

int qspi_write(uint32_t *tx_buff, size_t tx_len, uint32_t dst){
    /* Both tx_len and dst need to be word-aligned */
    if(!IS_ALIGNED(tx_len) || !IS_ALIGNED(dst)){
        return -1;
    }

    /* Make sure write doesn't start at the end of flash
     * and that write wouldn't run past the end of flash */
    if(dst >= EXTFLASH_SIZE || (dst + tx_len) > EXTFLASH_SIZE){
        return -2;
    }

    qspi_wait_ready();
    _qspi_write(tx_buff, tx_len, dst);
    return 0;
}


int qspi_erase(uint32_t start, enum erase_len len){
    if(!IS_ALIGNED(start)){
        return -1;
    }

    qspi_wait_ready();
    _qspi_erase(start, len);
    return 0;
}

/* Deactivate qspi peripheral */
void qspi_deactivate(){
    _qspi_deactivate();
}


/* Configure and activate QSPI driver */
void qspi_init(){
    _qspi_configure();
    _qspi_activate();
}


int main(void){	
    uint32_t read_data = 0;
    qspi_read(&read_data, sizeof(read_data), 0x0);

    /* Sleep is required for printk() to read correct data */
    k_msleep(100);
    printk("Value of read_data: %x\n", read_data);

    return 0;
}
