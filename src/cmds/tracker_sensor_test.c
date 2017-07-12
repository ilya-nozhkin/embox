#include <stdlib.h>
#include <unistd.h>
#include <drivers/tracker_sensor.h>

int main(int argc, char **argv) {
	for (int i = 0; i < 1000; i++) {
		uint8_t data = tracker_sensor_measure();
		printf("%d\n", data);
		usleep(1000);
	}

	return 0;
}
