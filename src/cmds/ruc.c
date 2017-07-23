
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
	sensor_map_from_file(argv[2]);
	ruc_import(argv[1]);
	fflush(stdout);
	return 0;
}
