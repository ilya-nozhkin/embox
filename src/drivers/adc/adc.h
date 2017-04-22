/**
 * @file
 * @brief
 *
 * @author  Ilya Nozhkin
 * @date    22.04.2017
 */

#ifndef DRIVERS_ADC_H_
#define DRIVERS_ADC_H_

#include <stdint.h>

void adc_start(uint32_t measures);
uint32_t adc_completed(void);
uint32_t *adc_get_raw(void);
uint32_t adc_get_corrected(void);

#endif /* DRIVERS_ADC_H_ */
