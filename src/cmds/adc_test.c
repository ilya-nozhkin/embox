#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <drivers/adc.h>

int main(int argc, char **argv) {
	adc_start(1);
	
	sleep(1);
	
	printf("%d\n", adc_get_raw()[0]);

	return 0;
}
