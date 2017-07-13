#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <drivers/sensors/hc_sr04.h>

int main(int argc, char **argv) {

	hc_sr04_setup(4, 5);
	hc_sr04_request();
	uint32_t centimeters = hc_sr04_response();

	printf("Cm:%d\n", centimeters);

	return 0;
}
