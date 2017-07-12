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

struct ad_converter;

void adc_start(struct ad_converter *adc, uint32_t measures);
void adc_stop(struct ad_converter *adc);

uint32_t adc_waiting(struct ad_converter *adc);
uint32_t *adc_get_raw(struct ad_converter *adc);
uint32_t adc_get_corrected(struct ad_converter *adc);

struct ad_converter *adc_by_id(uint32_t adc_id);

#endif /* DRIVERS_ADC_H_ */
