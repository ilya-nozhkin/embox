#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <drivers/servo.h>

int main(int argc, char **argv) {
	if (argc >= 3) {
		int id = argv[2][0] - '0';
		if (argv[1][0] == 'e') {
			servo_enable(servo_by_id(id));
		}
		if (argv[1][0] == 'd') {
			servo_disable(servo_by_id(id));
		}
		if (argv[1][0] == 'f') {
			servo_set_direction(servo_by_id(id), 1);
		}
		if (argv[1][0] == 'b') {
			servo_set_direction(servo_by_id(id), 0);
		}
	}

	return 0;
}
