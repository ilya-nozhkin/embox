/**
 * @file
 * @brief
 *
 * @author  Ilya Nozhkin
 * @date    14.04.2017
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern int ruc_import();

int main(int argc, char **argv) {

	char dir[] = "/flash/";
	char *path = malloc(strlen(argv[1]) + 1 + 7);

	memcpy(path, dir, 7);
	memcpy(path+7, argv[1], strlen(argv[1]) + 1);

	ruc_import(path);

	fflush(stdout);
	free(path);
  
	return 0;
}
