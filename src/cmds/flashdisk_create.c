#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <drivers/block_dev/esp8266/flashdisk.h>

int main(int argc, char **argv) {

	//flashdisk_create(argv[1], 64*1024);

	int offset = atoi(argv[1]) * 16;

	uint32_t dest = 0xDEADBEEF;
	int res = spi_flash_read(offset, &dest, 4);

	printf("res: %d 0x%X\n", res, dest);

	return 0;
}
