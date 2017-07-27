#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <drivers/block_dev/esp8266/flashdisk.h>
#include <drivers/pwm.h>

static int first = 1;
static int pwma = 0;
static int pwmb = 0;

int main(int argc, char **argv) {
	if (first) {
		pwm_enable();
		pwma = pwm_add(5);
		pwmb = pwm_add(12);
		first = 0;
	}
	
	pwm_set_power(pwma, atoi(argv[1]));
	pwm_set_power(pwmb, atoi(argv[2]));

	return 0;
}
