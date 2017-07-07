#include <stdio.h>
#include <stdlib.h>
#include <util/err.h>

#include <drivers/esp8266/spi_api_impl.h>
#include <drivers/block_dev/esp8266/flashdisk.h>

int main(int argc, char **argv) {
	char *fs = argv[1];

	int e = err(flashdisk_create(fs, 124 * FLASH_BLOCK_SIZE));

	printf("Error: %d\n", e);

	if(!e)
		flashdisk_delete(fs);

    return 0;
}
