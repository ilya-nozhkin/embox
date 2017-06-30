#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <drivers/i2c.h>
#include <time.h>

int main(int argc, char **argv) {
	i2c_start();
	
	struct timespec ts, ts2;
	ts.tv_sec = 0;
	ts.tv_nsec = 500000000;
	printf("start\n");
	
	nanosleep(&ts, &ts2);
	
	printf("stop\n");

	return 0;
}
