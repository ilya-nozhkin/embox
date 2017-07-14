#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <drivers/sensors/dht11.h>

int main(int argc, char **argv) {
	struct dht11 *sensor = dht11_setup(5, 1);

	dht11_request(sensor);
	struct dht11_response res = dht11_read_response(sensor);

	printf("RH: %d.%d%% T: %d.%dC Valid: %d\n", res.integral_rh, res.float_rh,
                                            	res.integral_temp, res.float_temp,
												res.ok);

	return 0;
}
