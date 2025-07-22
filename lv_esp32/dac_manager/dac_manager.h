#ifndef DAC_MANAGER_H
#   define DAC_MANAGER_H

#   include "esp_err.h"
#   include <stdint.h>

#   if CONFIG_IDF_TARGET_ESP32
#       define DAC_CHAN0_ADC_CHAN   (DAC_CHAN_0) // GPIO25, same as DAC channel 0
#       define DAC_CHAN1_ADC_CHAN   (DAC_CHAN_1) // GPIO26, same as DAC channel 1
#   endif  /* CONFIG_IDF_TARGET_ESP32 */

esp_err_t init_dac(uint32_t freq);
esp_err_t start_dac(uint32_t freq);

#endif /* DAC_MANAGER_H */