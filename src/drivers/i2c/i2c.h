#ifndef DRIVERS_I2C_H_
#define DRIVERS_I2C_H_

#include <stdint.h>

struct i2c_bus;

struct i2c_bus *i2c_initialize(uint8_t sda_pin, uint8_t scl_pin, uint32_t delay_time);
void i2c_terminate(struct i2c_bus *bus);

void i2c_start(struct i2c_bus *bus);
void i2c_stop(struct i2c_bus *bus);

uint8_t i2c_send(struct i2c_bus *bus, uint8_t data);
uint8_t i2c_receive(struct i2c_bus *bus, uint8_t last);

#endif
