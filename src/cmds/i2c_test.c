#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <drivers/i2c.h>
#include <time.h>

int main(int argc, char **argv) {
	
	i2c_start();
	printk("%d\n", i2c_send(0x5C));
	printk("%d\n", i2c_send(0x03));

	return 0;
}
