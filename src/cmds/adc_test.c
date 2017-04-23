#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <drivers/adc.h>

int main(int argc, char **argv) {
	for (int i = 0; i < 1000; i++) {
		adc_start(8);
		uint32_t *data = adc_get_raw();
		printf("%d             %d %d\n", adc_get_corrected(), data[0], data[7]);
		usleep(10000);
	}
	                                    
	adc_stop();

	return 0;
}
