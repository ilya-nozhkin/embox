#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <drivers/block_dev/esp8266/flashdisk.h>

typedef struct ttt {
	uint32_t 	exist;			/* helps find out if this is garbage */
	char		name[NAME_MAX]; /* name itself */
	char 		ph[28];			/* placeholder */
} ttt_t;

int main(int argc, char **argv) {

	//flashdisk_create(argv[1], 64*1024);

	int offset = atoi(argv[1]) * 16;

	ttt_t dest;
	int res = spi_flash_read(offset, &dest, sizeof(ttt_t));

	printf("res: %d 0x%X %s\n", res, dest.exist, dest.name);

	return 0;
}
