#include "sharp_2y0a21.h"
#include <hal/system.h>
#include <math.h>
#include <drivers/adc.h>

uint32_t sharp_2y0a21_measure(uint32_t adc_id){
	struct ad_converter *adc = adc_by_id(adc_id);
	adc_start (adc, 8);
	double power = pow(adc_get_corrected(adc), -1.330678);
	uint32_t result = (uint32_t) (power * 93375.470664);
	adc_stop (adc);

	if (result > 80)
	{
		return 80;
	}
	
	return result;
}
