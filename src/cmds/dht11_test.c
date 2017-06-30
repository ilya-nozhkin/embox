#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <drivers/sensors/dht11.h>

int main(int argc, char **argv) {
	printf("Sending request\n");
	dht11_request();
	usleep(18);

    struct dht11_response res = dht11_read_response(atoi(argv[0]));

    printf("RH: %d . %d T: %d . %d Checksum: %d\n", res.integral_rh, res.float_rh,
                                                    res.integral_temp, res.float_temp,
                                                    res.checksum);

	return 0;
}
