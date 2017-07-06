#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <drivers/i2c.h>
#include <time.h>
#include <hal/reg.h>
#include <drivers/gpio.h>

int main(int argc, char **argv) {
	i2c_init(4, 5, 10);
	
	int success = 0;
	while (!success) {
		i2c_start();
		success = !(i2c_send(0xB8));
		printk("%d", !success);
		if (!success) {
			i2c_stop();
		}
	}
	
	printk("%d\n", i2c_send(0x03));
	printk("%d\n", i2c_send(0x00));
	printk("%d\n", i2c_send(0x04));

	
	i2c_start();
	printk("%d\n", i2c_send(0xB8 | 1));
	printk("%d\n", i2c_receive(0));
	printk("%d\n", i2c_receive(0));
	printk("%d\n", i2c_receive(0));
	printk("%d\n", i2c_receive(0));
	printk("%d\n", i2c_receive(0));
	printk("%d\n", i2c_receive(0));
	printk("%d\n", i2c_receive(1));
	i2c_stop();

	return 0;
}
