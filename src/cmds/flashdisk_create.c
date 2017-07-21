#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <drivers/block_dev/esp8266/flashdisk.h>
#include <drivers/pwm/pwm.h>

static int ok = 0;
static int fid = 0;
static int sid = 0;

int main(int argc, char **argv) {

	//flashdisk_create(argv[1], 64*1024);

	if(!ok) {
		fid = pwm_add(5);
		sid = pwm_add(12);
	}

	ok++;

	pwm_set_power(fid, atoi(argv[1]));
	pwm_set_power(sid, atoi(argv[2]));

	if(ok++ == 1)
		pwm_enable();

	return 0;
}
