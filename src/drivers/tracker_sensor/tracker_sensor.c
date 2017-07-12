#include "tracker_sensor.h"
#include <hal/system.h>
#include <drivers/adc.h>

uint8_t tracker_sensor_measure(void){
	adc_start(1);
	uint8_t data = adc_get_corrected();
	if (data == 255){
		return 1;
	}
	return 0;
}
