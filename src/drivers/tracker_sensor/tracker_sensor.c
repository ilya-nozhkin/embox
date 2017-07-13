#include "tracker_sensor.h"
#include <hal/system.h>
#include <drivers/adc.h>

uint8_t tracker_sensor_measure(uint32_t adc_id){
	struct ad_converter *adc = adc_by_id(adc_id);
	adc_start(adc, 1);
	uint8_t data = adc_get_corrected(adc);
	adc_stop(adc);
	if (data == 255){
		return 1;
	}
	return 0;
}
