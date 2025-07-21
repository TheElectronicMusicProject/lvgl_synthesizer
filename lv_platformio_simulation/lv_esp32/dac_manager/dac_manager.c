#include "dac_manager.h"
#include "sdkconfig.h"
#include "driver/dac_cosine.h"

static dac_cosine_handle_t gh_chan0 = NULL;

esp_err_t
init_dac (uint32_t freq)
{
    esp_err_t ret = ESP_FAIL;

    dac_cosine_config_t cfg_wave = {
        .chan_id = DAC_CHAN_1,
        .freq_hz = freq,
        .clk_src = DAC_COSINE_CLK_SRC_DEFAULT,
        .atten = DAC_COSINE_ATTEN_DEFAULT,
        .phase = DAC_COSINE_PHASE_0,
        .offset = 0,
        .flags.force_set_freq = true,
    };
    
    ret = dac_cosine_new_channel(&cfg_wave, &gh_chan0);

    return (ret);
}   /* init_dac() */

esp_err_t
start_dac (uint32_t freq)
{
    esp_err_t ret = ESP_FAIL;

    ret = dac_cosine_start(gh_chan0);

    return (ret);
}   /* start_dac() */