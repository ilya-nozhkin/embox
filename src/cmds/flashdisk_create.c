#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <drivers/block_dev/esp8266/flashdisk.h>

int main(int argc, char **argv) {

	//flashdisk_create(argv[1], 64*1024);

	int mode = atoi(argv[1]);

	uint32_t src[4] = {1, 1, 1, 1};
	uint32_t dest[12] = {0};

	if(mode == 1){
		int res = spi_flash_write(0, src, 16);
		printf("Result: %d\n", res);
	} else if(mode == 2){
		spi_flash_erase_sector(0);
	} else {
		int res = spi_flash_read(0, dest, 48);

		printf("Result: %d\n", res);

		for(int i = 0;i < 12; i++)
			printk("%d ", dest[i]);
	}

	return 0;
}
