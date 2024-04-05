#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/flash.h>
#include <stdint.h>
#include <nrfx_qspi.h>

#include "qspi.h"

int main(void){	
    uint32_t read_data = 0;
    qspi_read(&read_data, sizeof(read_data), 0x0);

    /* Sleep is required for printk() to read correct data */
    k_msleep(100);
    printk("Value of read_data: %x\n", read_data);

    return 0;
}

