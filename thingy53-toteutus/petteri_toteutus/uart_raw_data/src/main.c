

/* 
 * This program prints digital microphone data to terminal 
 * in raw data. Device used are Thingy53.
 * Compile function is thingy53_nrf5340_cpuapp
*/


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/audio/dmic.h>
#include <dk_buttons_and_leds.h>
#include <nrfx_pdm.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/device.h>

//Configure for UART
const struct device *uart_data_print = DEVICE_DT_GET(DT_NODELABEL(cdc_acm_uart));

//Configure for UART ends


//PDM data setup start
#define PDM_CLK     41  // P1.09 = 32 + 9
#define PDM_DIN     27  // P0.27

#define PDM_CLK_FREQ    NRF_PDM_FREQ_1280K;
#define PDM_CLK_FREQ    NRF_PDM_FREQ_1280K;
#define PDM_RATIO       PDM_RATIO80

/* How many buffers (seconds) we want to record. 
 * N_BUFF * 2 = seconds
 *
 * nrfx_pdm_buffer_set() only allows buffer buffer sizes <= 32767 words 
 * at a 16k sampling frequency, the most we can do per buffer
 * is about 2 seconds so >1 are required for longer periods 
 * Out of memory occurs after around 12 seconds */
#define N_BUFF  6 

K_SEM_DEFINE(data_ready, 0, 1);


enum PDM_RATIO{
    PDM_RATIO64 = 0,
    PDM_RATIO80 = 1,
};

//PDM data setup stops


LOG_MODULE_REGISTER(uart_raw_data, LOG_LEVEL_DBG);

void serial_uart_setup(){ //Setuping UART to work

    if (!device_is_ready(uart_data_print)) {
        LOG_ERR("Error %d\n",errno);
    }

    const struct uart_config uart_conf = {
        .baudrate = 115200,
        .parity = UART_CFG_PARITY_NONE,
        .stop_bits = UART_CFG_STOP_BITS_1,
        .data_bits = UART_CFG_DATA_BITS_8,
        .flow_ctrl = UART_CFG_FLOW_CTRL_NONE,
    };

    uart_configure(uart_data_print, &uart_conf); //Configure UART


	int error_uart = uart_err_check(uart_data_print); //Errors in code
    LOG_ERR("UART Error check %d\n", error_uart); 

}



void serial_uart_print(uint8_t *show_data_buff, size_t buff_len){ //Printing serial data to UART

    for(size_t i = 0; i < buff_len; i++){
        uart_poll_out(uart_data_print,show_data_buff[i]);
    }
} 





void pdm_config_data_configure(){ //Pdm data configure setup

    nrfx_pdm_config_t pdm_cfg = NRFX_PDM_DEFAULT_CONFIG(PDM_CLK, PDM_DIN);
    pdm_cfg.mode        = NRF_PDM_MODE_MONO;
    pdm_cfg.edge        = NRF_PDM_EDGE_LEFTFALLING;
    pdm_cfg.gain_l      = NRF_PDM_GAIN_MAXIMUM;
    pdm_cfg.gain_r      = NRF_PDM_GAIN_MAXIMUM;
    pdm_cfg.clock_freq  = PDM_CLK_FREQ;
    pdm_cfg.skip_gpio_cfg = false;
    pdm_cfg.skip_psel_cfg = false;

    //nrfx_pdm_init(&pdm_cfg, pdm_evt_handler);
    /* This *MUST* be called after nrfx_pdm_init or configuration will be overwritten */
    set_pdm_ratio(PDM_RATIO);



}



int main(void)
{
	serial_uart_setup(); //Uart setup running

	uint8_t test_buffer[] = "testdff\r\n";

	while(1){ //Infinite loop
		serial_uart_print(test_buffer,sizeof(test_buffer)); //Print serial data in UART
	}
	
	return 0;
}
