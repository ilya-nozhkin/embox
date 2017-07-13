#include <stdlib.h>
#include <unistd.h>
#include <drivers/sharp_2y0a21.h>

int main(int argc, char **argv) {
	for (int i = 0; i < 1000; i++) {
		uint8_t data = sharp_2y0a21_measure(1);
		printf("data: %d\n", data);
		usleep(10000);
	}

	adc_stop();

	return 0;
}
