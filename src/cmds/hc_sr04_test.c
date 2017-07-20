#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <drivers/sensors/hc_sr04.h>

int main(int argc, char **argv) {

	struct hc_sr04 *sensor = hc_sr04_setup(4, 5);
	uint32_t centimeters = hc_sr04_response(sensor);

	printf("Cm:%d\n", centimeters);

	return 0;
}
