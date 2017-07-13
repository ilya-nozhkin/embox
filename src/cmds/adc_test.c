#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <drivers/adc.h>

int main(int argc, char **argv) {
	struct ad_converter adc = {.id = 1};
	for (int i = 0; i < 1000; i++) {
		adc_start(&adc, 8);
		uint32_t *data = adc_get_raw(&adc);
		printf("%d             %d %d\n", adc_get_corrected(&adc), data[0], data[7]);
		usleep(10000);
	}

	adc_stop(&adc);

	return 0;
}
