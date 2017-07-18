#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

static char buffer[16];

int main(int argc, char **argv) {
	int size = atoi(argv[1]);

	printf("Filename: %s\n", argv[2]);

	FILE *f = fopen(argv[2], "w");

	char data;
	int counter = 0;
	while(size--){
		data = diag_getc();
		buffer[counter++] = data;

		if(counter == 16){
			fwrite(buffer, 16, 1, f);
			counter = 0;
		}
	}

	fwrite(buffer, counter, 1, f);

	fclose(f);
}
