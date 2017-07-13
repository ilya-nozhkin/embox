#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <drivers/block_dev/esp8266/flashdisk.h>

int main(int argc, char **argv) {

	flashdisk_create(argv[1], 64*1024);

	return 0;
}
