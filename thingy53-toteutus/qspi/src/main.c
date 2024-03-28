#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>
#include <nrfx_qspi.h>
LOG_MODULE_REGISTER(qspi, LOG_LEVEL_DBG);


#define EXTFLASH_SIZE 0x800000
#define WORDSIZE 4

enum erase_len{
    ERASE_4KB   = 0,
    ERASE_64KB  = 1,
    ERASE_ALL   = 2
};

/* Assembly functions */
extern void _qspi_configure(void);
extern void _qspi_activate(void);
extern void _qspi_write(uint32_t *tx_buff, size_t tx_len, uint32_t dst);
extern void _qspi_read(uint32_t *rx_buff, size_t rx_len, uint32_t src);
extern void _qspi_erase(uint32_t start, enum erase_len len);
extern uint32_t _qspi_status();

/* C functions */
int qspi_write(uint32_t *tx_buff, size_t tx_len, uint32_t dst);
int qspi_read(uint32_t *rx_buff, size_t rx_len, uint32_t src);
int qspi_erase(uint32_t start, enum erase_len len);


int qspi_erase(uint32_t start, enum erase_len len){
    if(start % WORDSIZE != 0){
        return -1;
    }

    //TODO: implement in C
    _qspi_erase(start, len);
    return 0;
}


int qspi_read(uint32_t *rx_buff, size_t rx_len, uint32_t src){
    if(src > EXTFLASH_SIZE){
        return -1;
    }

    if(rx_len % WORDSIZE != 0){
        return -2;
    }

    // TODO: implement in C
    _qspi_read(rx_buff, rx_len, src);
    return 0;
}

int qspi_write(uint32_t *tx_buff, size_t tx_len, uint32_t dst){
    if(dst > EXTFLASH_SIZE){
        return -1;
    }

    if(tx_len % WORDSIZE != 0){
        return -2;
    }

    // TODO: implement in C
    _qspi_write(tx_buff, tx_len, dst);
    return 0;
}


int main(void){	
    uint32_t read_data = 0;

    _qspi_configure();
    _qspi_activate();


    while(_qspi_status() & 0x4);
    qspi_read(&read_data, sizeof(read_data), 0x0);
    k_msleep(100);

    printk("Value of read_data: %x\n", read_data);

    return 0;

}
