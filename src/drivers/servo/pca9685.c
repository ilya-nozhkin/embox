/**
 * @file
 * @brief
 *
 * @author  Ilya Nozhkin
 * @date    29.06.2017
 */

#include <drivers/servos/pca9685.h>
#include <drivers/i2c.h>
#include <stdint.h>

#define PCA9685_MAX_N OPTION_GET(NUMBER, max_n)

#define I2C_DELAY 10
#define ATTEMPTS 10
#define FREQUENCY 50

struct pca9685 {
	struct i2c_bus *bus;
	uint8_t shifted_address;
}

struct pca9685 pca9685_drivers[PCA9685_MAX_N];
uint32_t pca9685_number = 0;

uint8_t send_data(struct pca9685 *driver, uint8_t reg_address, const uint8_t *data, uint8_t size) {
	i2c_start(bus);
	for (int i = 0; i < ATTEMPTS; i++) {
		if (i2c_send(bus, driver->shifted_address) == 0) {
			break;
		}
		if (i == ATTEMPTS - 1) {
			return 0;
		}
	}
	
	i2c_send(bus, reg_address);
	
	for (uint8_t i = 0; i < size; i++) {
		i2c_send(bus, data[i]);
	}
	
	i2c_stop(bus);
	
	return 1;
}

struct pca9685 *pca9685_start(struct i2c_bus *bus, uint8_t address) {
	pca9685_drivers[pca9685_number].bus = bus;
	pca9685_drivers[pca9685_number].shifted_address = address << 1;
	struct pca9685 *driver = &pca9685_drivers[pca9685_number];
	
	struct i2c_bus *bus = i2c_init(i2c_sda, i2c_scl, I2C_DELAY);
	
	uint8_t config = 0b00100001;
	if (!send_data(driver, 0x00, &config, 1)) {
		return 0;
	}
	
	uint8_t pre_scale = (uint8_t) (25000000 / (4096 * FREQUENCY)) - 1;
	if (!send_data(driver, 0xFE, &pre_scale, 1)) {
		return 0;
	}
	
	pca9685_number++;
	return driver;
}

void pca9682_servo_set(struct pca9685 *driver, uint8_t servo, uint32_t power) {
	uint8_t reg_address = 0x06 + servo * 4;
	
	uint16_t data[2] = {0, 0, power, power};
	
	send_data(driver, reg_address, data, 2);
}
