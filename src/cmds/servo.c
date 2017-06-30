#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <drivers/servo.h>

int main(int argc, char **argv) {
	servo_enable(servo_by_id(0));

	return 0;
}
