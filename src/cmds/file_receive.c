#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

static char buffer[16];

int main(int argc, char **argv) {
	int size = atoi(argv[1]);

	FILE *f = fopen("/flash/file1.txt", "w");
	char data;
	int counter = 0;
	while((data = getchar()) != EOF){
		buffer[counter++] = data;
		size--;

		if(counter >= 15 || !size){
			fwrite(buffer, counter, 1, f);
			counter = 0;

			if(!size)
				break;
		}
	}
	fclose(f);
}
