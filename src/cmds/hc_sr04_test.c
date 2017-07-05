#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <drivers/sensors/hc_sr04.h>

int main(int argc, char **argv) {

	sensor_setup(4,5);
	sensor_request();
	uint32_t centimeters = sensor_response();

	printf("Cm:%d\n", centimeters);

	return 0;
}
