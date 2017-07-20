#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <drivers/block_dev/esp8266/flashdisk.h>
#include <drivers/pwm/lx106/pwm.h>

static int ok = 0;

int main(int argc, char **argv) {

	//flashdisk_create(argv[1], 64*1024);

	lx106_pwm_t first;
	lx106_pwm_t second;

	first.pin = 5;
	second.pin = 12;

	first.power = atoi(argv[1]);
	second.power = atoi(argv[2]);

	int f = lx106_pwm_add_servo(0, first);
	int s = lx106_pwm_add_servo(1, second);

	printf("First: %d second: %d\n", f, s);

	if(ok)
		return 0;

	int res = lx106_pwm_enable();
	printf("Enabled: %d\n", res);
	ok++;

	return 0;
}
