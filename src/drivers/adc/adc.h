/**
 * @file
 * @brief
 *
 * @author  Ilya Nozhkin
 * @date    22.04.2017
 */

#ifndef DRIVERS_ADC_H_
#define DRIVERS_ADC_H_

#include <module/embox/driver/adc/api.h>
#include <stdint.h>

#define MAX_ADC_VALUE _MAX_ADC_VALUE

void adc_start(uint32_t measures);
void adc_stop(void);

uint32_t adc_waiting(void);
uint32_t *adc_get_raw(void);
uint32_t adc_get_corrected(void);

#endif /* DRIVERS_ADC_H_ */
