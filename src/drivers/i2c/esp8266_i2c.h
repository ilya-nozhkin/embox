#ifndef DRIVERS_ESP8266_I2C_H_
#define DRIVERS_ESP8266_I2C_H_

struct i2c_bus {
	uint32_t id;
	uint8_t sda;
	uint8_t scl;
	uint32_t delay;
}

#endif /* DRIVERS_ESP8266_I2C_H_ */
