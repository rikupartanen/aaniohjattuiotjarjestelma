#include <stdlib.h>
#include <stdint.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/audio/dmic.h>
#include <nrfx_pdm.h>

LOG_MODULE_REGISTER(pdmtest, LOG_LEVEL_DBG);

#define PDM_CLK     41 // P1.09 = 32 + 9
#define PDM_DIN     27 // P0.27

#define AUDIO_SAMPLE_RATE       16000
#define AUDIO_SAMPLE_SIZE       (sizeof(int16_t))
#define AUDIO_BLOCK_SIZE        ((AUDIO_SAMPLE_RATE * AUDIO_SAMPLE_SIZE) * 2)


enum PDM_RATIO{
    PDM_RATIO64 = 0,
    PDM_RATIO80 = 1,
};

/* Set ratio for PDMCLK / ratio 
 * 0 for ratio 64
 * 1 for ratio 80
 * */
extern void set_pdm_ratio(enum PDM_RATIO);


K_SEM_DEFINE(data_ready, 0, 1);

static int16_t *g_buff;

void dump_buffer(uint16_t *buff, size_t len){
    printf("\n*** START OF BUFFER DUMP ***\n");
    for(size_t i = 0; i < len / sizeof(int16_t); ++i){
        if(i % 8 == 0){ putchar('\n'); }
        printf("%04x ", buff[i]);
    }
    
    putchar('\n');
    printf("\n*** END OF BUFFER DUMP ***\n\n");

}


/*void read_samples(int16_t *buff, size_t len){
    nrfx_pdm_stop();
    dump_buffer(buff, len);
    k_sem_give(&data_ready);
}*/

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
        /* We should have a valid buffer already but might as well check here too */
        if(g_buff == NULL){
            LOG_ERR("Malloc failed for audio buffer %p", g_buff);
            return;
        }
        nrfx_pdm_buffer_set(g_buff, (AUDIO_BLOCK_SIZE / AUDIO_SAMPLE_SIZE));
    }

    if(p_evt->buffer_released != NULL){
        k_sem_give(&data_ready);
    }

}

int main(){
    g_buff = calloc(AUDIO_BLOCK_SIZE, 1);
    nrfx_pdm_config_t pdm_cfg = NRFX_PDM_DEFAULT_CONFIG(PDM_CLK, PDM_DIN);
    pdm_cfg.mode        = NRF_PDM_MODE_MONO;
    pdm_cfg.edge        = NRF_PDM_EDGE_LEFTFALLING;
    pdm_cfg.gain_l      = NRF_PDM_GAIN_MAXIMUM;

    // 1280K / 80 = 16K sample frequency
    pdm_cfg.clock_freq  = NRF_PDM_FREQ_1280K;

    nrfx_pdm_init(&pdm_cfg, pdm_evt_handler);
    set_pdm_ratio(PDM_RATIO80);

    for(;;){
        k_msleep(5000);

        nrfx_pdm_start();

        k_sem_take(&data_ready, K_FOREVER);
        dump_buffer(g_buff, AUDIO_BLOCK_SIZE);
    }


    return 0;
}
