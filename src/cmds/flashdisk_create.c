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
	flashdisk_create(argv[1], 64*1024);

	return 0;
}
