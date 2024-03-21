#include <stdlib.h>
#include <stdint.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/audio/dmic.h>
#include <dk_buttons_and_leds.h>
#include <nrfx_pdm.h>

LOG_MODULE_REGISTER(pdmtest, LOG_LEVEL_DBG);

/* Pins for microphone */
#define PDM_CLK     41  // P1.09 = 32 + 9
#define PDM_DIN     27  // P0.27


/* Audio sampling settings 
 * Note: as of now, there's no automated way for making sure AUDIO_SAMPLE_RATE 
 * will produce valid output due to how the actual sampling rate is calculated (PDM_CLK / ratio) 
 * so you must make sure the sampling frequency aligns with a valid clock frequency and ratio */
#define AUDIO_SAMPLE_RATE       16000
#define AUDIO_SAMPLE_SIZE       (sizeof(int16_t))
#define AUDIO_BLOCK_SIZE        ((AUDIO_SAMPLE_RATE * AUDIO_SAMPLE_SIZE) * 2)

/* Set clock frequency and ratio. 
 * These need to produce the desired sample rate = (PDM_CLK_FREQ / PDM_RATIO)
 *
 * Note: this is not very thoroughly tested,
 * but it appears PDM_CLK >= 1.1MHz is required by the VM3011 so possible valid options are:
 * 
 * *------------*----------*---------------*
 * |    FREQ    |   RATIO  |   SAMPLE FREQ |
 * |------------|----------|---------------|
 * | 1.231 MHz  |    64    |   19.234 kHz  |
 * | 1.280 MHz  |    64    |       20 kHz  |
 * | 1.333 MHz  |    64    |   20.828 kHz  |
 * | 1.231 MHz  |    80    |   15.388 kHz  |
 * | 1.280 MHz  |    80    |       16 kHz  |
 * | 1.333 MHz  |    80    |   16.662 kHz  |
 * *------------*----------*---------------*
 *
 * As of 2024-03-21, these must be given in the form 
 * NRF_PDM_FREQ_1231K, NRF_PDM_1280K and so on.
 *
 * Settings for 16kHz */
#define PDM_CLK_FREQ    NRF_PDM_FREQ_1280K;
#define PDM_RATIO       PDM_RATIO80

/* How many buffers we want 
 * nrfx_pdm_buffer_set() only allows buffer buffer sizes <= 32767 words 
 * at a 16k sampling frequency, the most we can do per buffer
 * is about 2 seconds (64K) so >1 are required for longer periods */
#define N_BUFF  2

K_SEM_DEFINE(data_ready, 0, 1);


enum PDM_RATIO{
    PDM_RATIO64 = 0,
    PDM_RATIO80 = 1,
};

static uint8_t g_buffsel = 0;
static int16_t *g_buff[N_BUFF];
static int16_t *g_buffready = NULL;

static bool g_pdm_stopped = 0;


/* Quick and (very) dirty solution to changing the PDM sampling ratio */
void set_pdm_ratio(enum PDM_RATIO ratio){
    /* 0x50026520 PDM_RATIO register address for SECURE application */
    __asm__ volatile("ldr r1, =0x50026520\n\t"
                     "str %0, [r1]\n\t"
                      :
                      : "r" (ratio));
}


void dump_buffer(uint16_t *buff, size_t len){
    printf("\n*** START OF BUFFER DUMP ***\n");
    for(size_t i = 0; i < len / sizeof(int16_t); ++i){
        if(i % 8 == 0){ putchar('\n'); }
        printf("%04x ", buff[i]);
    }
    
    putchar('\n');
    printf("\n*** END OF BUFFER DUMP ***\n\n");

}

void pdm_stop(){
    nrfx_pdm_stop();
    g_pdm_stopped = 1;
}

void pdm_start(){
    nrfx_pdm_start();
    g_pdm_stopped = 0;
}

/* Event handler for pdm events
 * will be called on:
 * - error
 * - buffer required
 * - buffer full*/
void pdm_evt_handler(nrfx_pdm_evt_t const *p_evt){

    /* Just log and exit on error */
    if(p_evt->error != NRFX_PDM_NO_ERROR){
        LOG_ERR("PDM Overflow error %d", p_evt->error);
        return;
    }

    if(p_evt->buffer_requested){
        g_buffsel ^= 1;
        nrfx_pdm_buffer_set(g_buff[g_buffsel], (AUDIO_BLOCK_SIZE / AUDIO_SAMPLE_SIZE));
    }

    if(p_evt->buffer_released != NULL){
        /* The event handler might be called once more
         * after nrfx_pdm_stop() has been called and buffer_released will be set
         * thus we end up giving an unnecessary data_ready.
         * So we simply check if we've already stopped */
        if(!g_pdm_stopped){ 
            pdm_stop(); 
            g_buffready = p_evt->buffer_released;
            k_sem_give(&data_ready);
        }
    }

}

void butt_handler(uint32_t state, uint32_t has_changed){
    if(state == 1){
        pdm_start();
    }
}

int main(){
    /* TODO: see if this can be preallocated as a slab, this is a bit dirty */
    for(int i = 0; i < N_BUFF; ++i){
        g_buff[i] = calloc(AUDIO_BLOCK_SIZE, 1);
    }

    nrfx_err_t ret = 0;
    ret = dk_buttons_init(butt_handler);

        
    if(ret){
        LOG_ERR("Got err %d\n", ret);
    }
    

    nrfx_pdm_config_t pdm_cfg = NRFX_PDM_DEFAULT_CONFIG(PDM_CLK, PDM_DIN);
    pdm_cfg.mode        = NRF_PDM_MODE_MONO;
    pdm_cfg.edge        = NRF_PDM_EDGE_LEFTFALLING;
    pdm_cfg.gain_l      = NRF_PDM_GAIN_MAXIMUM;
    pdm_cfg.gain_r      = NRF_PDM_GAIN_MAXIMUM;

    pdm_cfg.clock_freq  = PDM_CLK_FREQ;

    nrfx_pdm_init(&pdm_cfg, pdm_evt_handler);
    /* This *MUST* be called after nrfx_pdm_init or configuration will be overwritten */
    set_pdm_ratio(PDM_RATIO);

    for(;;){

        k_sem_take(&data_ready, K_FOREVER);
        dump_buffer(g_buffready, AUDIO_BLOCK_SIZE);
    }

    for(int i = 0; i < N_BUFF; ++i){
        free(g_buff[i]);
    }

    return 0;
}
