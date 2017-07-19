#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <drivers/block_dev/esp8266/flashdisk.h>

typedef struct ttt {
	uint32_t 	exist;			/* helps find out if this is garbage */
	char		name[NAME_MAX]; /* name itself */
	char 		node_info[8];
	char 		ph[20];			/* placeholder */
} ttt_t;

int main(int argc, char **argv) {

	//flashdisk_create(argv[1], 64*1024);

	int offset = atoi(argv[1]) * 16;

	if(offset > 0){
		char *dest = malloc(sizeof(ttt_t));

		for(int i = 0; i < 4; i++){
			int from = offset + i*16;
			int res = spi_flash_read(from, dest + i*16, 16);
			printk("i: %d res: %d sector: %u current: %u\n", i, res, from/64, from%64);
		}

		printf("res: 0x%X %s\n", ((ttt_t *)dest)->exist, ((ttt_t *)dest)->name);

		free(dest);
	} else {
		ttt_t t;
		t.exist = 0x01061998;
		memcpy(t.name, "somename\0", 9);

		char *data = &t;

		for(int i = 0; i < 4; i++){
			int to = -offset + i*16;
			int res = spi_flash_write(to, data + i*16, 16);

			printk("i: %d res: %d sector: %u current: %u\n", i, res, to/64, to%64);
		}
	}

	return 0;
}
