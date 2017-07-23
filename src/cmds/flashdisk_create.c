#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <drivers/block_dev/esp8266/flashdisk.h>
#include <drivers/pwm/pwm.h>

static int ok = 0;
static int fid = 0;
static int sid = 0;

int main(int argc, char **argv) {
	flashdisk_create(argv[1], 64*1024);

	return 0;
}
