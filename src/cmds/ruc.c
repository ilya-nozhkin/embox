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

int main(int argc, char *argv[]) {
	ruc_import();
	fflush(stdout);
	return 0;
}
