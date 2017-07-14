#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <drivers/servos/pca9685.h>

static struct pca9685 *driver = 0;

int main(int argc, char **argv) {
	//if (driver == 0) {
	//	driver = pca9685_start(i2c_initialize(4, 5, 20), 0b1000000);
	//}
	
	struct i2c_bus *bus = i2c_initialize(4, 5, 100);
	
	for (int i = 0; i < 255; i++) {
		i2c_start(bus);
		printk("%d", i2c_send(bus, i));
		i2c_stop(bus);
		i2c_start(bus);
		printk("%d", i2c_send(bus, i));
		i2c_stop(bus);
		
		if (i % 24 == 0) {
			printk("\n");
		}
	}
	
	/*if (argc >= 3) {
		int id = argv[2][0] - '0';
		if (argv[1][0] == 'e') {
			printf("%p\n", driver);
			pca9685_servo_set(driver, id, 4096);
		}
		if (argv[1][0] == 'd') {
			pca9685_servo_set(driver, id, 0);
		}
		if (argv[1][0] == 'f') {
			pca9685_servo_set(driver, id, 4096);
		}
		if (argv[1][0] == 'b') {
			pca9685_servo_set(driver, id, 4096);
		}
	}*/

	return 0;
}
