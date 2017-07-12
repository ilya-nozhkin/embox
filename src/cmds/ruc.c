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

extern void set_digital_sensor(uint8_t id, uint32_t type, uint8_t pin0, uint8_t pin1);
extern void set_analog_sensor(uint8_t id, uint32_t type);

extern void servo_settings(uint8_t id, uint32_t speed);
extern uint32_t read_digital_sensor(uint8_t id);
extern uint32_t read_analog_sensor(uint8_t id);

int main(int argc, char *argv[]) {
	set_digital_sensor(0, 1, 0, 14);
	
	ruc_import("/export.txt");
	fflush(stdout);
	return 0;
}
