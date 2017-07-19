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
	servo_init(4, 5, 12, 13);
	set_analog_sensor(0, 1);
	
	ruc_import(argv[1]);

	fflush(stdout);
  
	return 0;
}
