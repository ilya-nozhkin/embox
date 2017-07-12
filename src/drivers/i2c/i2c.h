#ifndef DRIVERS_I2C_H_
#define DRIVERS_I2C_H_

#include <stdint.h>

void i2c_init(uint8_t sda_pin, uint8_t scl_pin, uint32_t _delay_time);

void i2c_start(void);
void i2c_stop(void);

uint8_t i2c_send(uint8_t data);
uint8_t i2c_receive(uint8_t last);

#endif
