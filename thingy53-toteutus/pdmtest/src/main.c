#include <stdlib.h>
#include <stdint.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/audio/dmic.h>
#include <nrfx_pdm.h>

LOG_MODULE_REGISTER(pdmtest, LOG_LEVEL_DBG);

#define PDM_CLK     41 // P1.09 = 32 + 9
#define PDM_DIN     27 // P0.27

static int16_t g_buff[1024];
K_SEM_DEFINE(data_ready, 0, 1);

void dump_buffer(uint16_t *buff, size_t len){
    printf("\n*** START OF BUFFER DUMP ***");
    for(size_t i = 0; i < len / sizeof(uint16_t); ++i){
        if(i % 8 == 0){ putchar('\n'); }
        printf("%04x ", buff[i]);
    }
    
    putchar('\n');
    printf("*** END OF BUFFER DUMP ***\n\n");

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


    /* TODO: handle allocation for buffers */
    if(p_evt->buffer_requested){
        //LOG_INF("Buffer requested");
        nrfx_pdm_buffer_set(g_buff, 1024);
    }

    /* TODO: handle buffer read */
    if(p_evt->buffer_released != NULL){
        k_sem_give(&data_ready);
    }

}

int main(){
    nrfx_pdm_config_t pdm_cfg = NRFX_PDM_DEFAULT_CONFIG(PDM_CLK, PDM_DIN);
    nrfx_pdm_init(&pdm_cfg, pdm_evt_handler);

    for(;;){
        k_msleep(5000);

        nrfx_pdm_start();

        k_sem_take(&data_ready, K_FOREVER);
        dump_buffer(g_buff, 1024);
    }


    return 0;
}
