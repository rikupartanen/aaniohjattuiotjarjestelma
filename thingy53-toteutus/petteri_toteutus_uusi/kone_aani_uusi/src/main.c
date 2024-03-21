/*
 * Copyright (c) 2024 OAMK Oulun ammattikorkeakoulu 
 * Petteri Karjalainen
 * Yritysprojekti 3 2024 Iot ääniohjaus Threadilla.
 */

/*
SAMPLE.PTR 0x560 RAM address pointer to write samples to with EasyDMA
SAMPLE.MAXCNT 0x564 Number of samples to allocate memory for in EasyDMA mode
VM3011 is microphones type in Thingy:53.
Dataline for microphone is P0.27 DIN
Data out line is P0.25 OUT

*/


#include <stdio.h>
#include <math.h> //Needed for mathematical functions
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <nrfx_pdm.h>
#include <nrfx.h>


#include <stdlib.h>
#include <stdint.h>

#include <zephyr/logging/log.h>
#include <zephyr/audio/dmic.h>


//PDM setuping ports
int din_pin_out = 27, clk_pin_out = 25; //PDM microphone pinout

int16_t audio_data_buffer[255];  //Output microphone data for print


#define NRFX_PDM_MAX_BUFFER_SIZE 32767 //PDM buffer size

#define AUDIO_SAMPLE_RATE       16000
#define AUDIO_SAMPLE_SIZE       (sizeof(int16_t))
#define AUDIO_BLOCK_SIZE        (AUDIO_SAMPLE_RATE * AUDIO_SAMPLE_SIZE)


static int16_t *g_buff;


K_SEM_DEFINE(data_ready, 0, 1);

int ms_delay = 5000;

void setup_pdm_audio(); //Introducing functions that are used later
void dump_buffer();

LOG_MODULE_REGISTER(foo, OCNFIG_FOO_LOG_LEVEL);

enum PDM_RATIO{
    PDM_RATIO64 = 0,
    PDM_RATIO80 = 1,
};


int main(void)
{	
	g_buff = calloc(AUDIO_BLOCK_SIZE, 1);

	setup_pdm_audio(); //Setuping pdm audio


	while(1) //Infinite loop for printing audio data
	{
		k_sleep(K_MSEC(ms_delay)); //Delay for data print
		
		nrfx_pdm_start(); //Starting pdm sending

		k_sem_take(&data_ready, K_FOREVER);
		dump_buffer(g_buff, AUDIO_BLOCK_SIZE);

	}

	return 0;
}



void pdm_event_handler(nrfx_pdm_evt_t const *pla_event)
{

	if(pla_event->error != NRFX_PDM_NO_ERROR)
	{
        LOG_ERR("PDM Overflow error %d", pla_event->error);
        return;
    }

	if(pla_event->buffer_requested){
        // We should have a valid buffer already but might as well check here too 
        if(g_buff == NULL){
            LOG_ERR("Malloc failed for audio buffer %p", g_buff);
            return;
        }
        nrfx_pdm_buffer_set(g_buff, (AUDIO_BLOCK_SIZE / AUDIO_SAMPLE_SIZE));
    }

    if(pla_event->buffer_released != NULL){
        k_sem_give(&data_ready);
    }
	

}



void dump_buffer(uint16_t *buff, size_t len){
    printf("\n*** START OF BUFFER DUMP ***\n");
    for(size_t i = 0; i < len / sizeof(uint16_t); ++i){
        if(i % 8 == 0)
		{ 
			putchar('\n'); 
		}
        printf("%04x ", buff[i]);
    }
    putchar('\n');
    printf("\n*** END OF BUFFER DUMP ***\n\n");

}



void set_pdm_ratio(enum PDM_RATIO ratio){
    /* 0x50026520 PDM_RATIO register address */
    __asm__ volatile("ldr r1, =0x50026520\n\t"
                     "str %0, [r1]\n\t"
                      :
                      : "r" (ratio));
}

void setup_pdm_audio()
{
	//Default audio gain 
	uint8_t defaultgain = 10;
	
	uint8_t gain_setup_left_right = (defaultgain - (2 * 3)); //Gain calculation for program


	nrfx_pdm_config_t pdm_config_init = {		

		.mode = NRF_PDM_MODE_MONO, //Mono or stereo mode

		.edge = NRF_PDM_EDGE_LEFTFALLING, //Rising edge or falling edge sample

		.clk_pin = clk_pin_out, //Clk pin

		.din_pin = din_pin_out, //Din pin

		.clock_freq = NRF_PDM_FREQ_1333K, //Clock frequency

		.gain_l = gain_setup_left_right, //Gain left channel

		.gain_r = gain_setup_left_right, //Gain right channel

		.interrupt_priority = 0,
		
		.skip_gpio_cfg = false,

		.skip_psel_cfg = false,

	};


	

	nrfx_pdm_init(&pdm_config_init, pdm_event_handler); //Initalize setup to PDM


		
	set_pdm_ratio(PDM_RATIO80);
	
	
}



