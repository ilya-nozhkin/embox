#include "sharp_2y0a21.h"
#include <hal/system.h>
#include <math.h>
#include <unistd.h>

#include <drivers/adc.h>

uint8_t sharp_2y0a21_measure(void){
	adc_start (8);
	double power = pow(adc_get_corrected(), -1.330678);
	uint8_t result =(int) (power * 93375.470664);
	adc_stop ();

	if (result > 80)
	{
		return 0;
	}
	return result;
}
