/*
 * Copyright (c) 2024 OAMK Oulun ammattikorkeakoulu 
 * Petteri Karjalainen
 * Yritysprojekti 3 2024 Iot ääniohjaus Threadilla.
 */

/*
PDM address for microphone Thingy:53 are 0x50026000 to 0x40026000
Easydma register is SAMPLE.PTR.
VM3011 is microphones type in Thingy:53.
Dataline for microphone is P0.27 DIN
Data out line is P0.25 OUT

*/


#include <stdio.h>
#include <math.h> //Needed for mathematical functions
#include <zephyr/kernel.h>
#include <nrfx_pdm.h>
#include <nrfx.h>



//PDM setuping dma
int din_pin_out = 27, clk_pin_out = 25; //PDM microphone pinout

double audio_data = 0.0; //Output microphone data for print

#define NRFX_PDM_MAX_BUFFER_SIZE 32767 //PDM buffer size

#define NRFX_PDM_DEFAULT_CONFIG(test, test2) //Port configure for PDM


//Easy dma memory buffer
#define READERBUFFER_SIZE 255 //Easydma buffer




int main(void)
{	


	setup_pdm_audio(); //Setuping pdm audio

	while(-1) //Infinite loop for printing audio value
	{

		//Get data and print it		
		printf("Audio value is %f\n", audio_data);
		k_msleep(20); //Delay for print

	}

	return 0;
}



void test_data_plc(nrfx_pdm_evt_t const *p_evt){


}



void setup_pdm_audio()
{
	
	uint8_t gain_setup_left_right = 200; //Gain setting for pdm
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

	nrfx_pdm_evt_t PDM_event_handl = {

		.buffer_released = NULL,
		.buffer_requested = 6554,
		.error = NRFX_PDM_NO_ERROR,

	};
	
	nrfx_pdm_init(&pdm_config_init, test_data_plc); //Initalize setup to PDM


	nrfx_pdm_start(); //Starting pdm sending
	
}



