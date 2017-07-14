#ifndef DRIVERS_ESP8266_I2C_H_
#define DRIVERS_ESP8266_I2C_H_

#include <drivers/gpio.h>

struct i2c_bus {
	uint32_t id;
	struct gpio *sda;
	struct gpio *scl;
	uint32_t delay;
};

#endif /* DRIVERS_ESP8266_I2C_H_ */
