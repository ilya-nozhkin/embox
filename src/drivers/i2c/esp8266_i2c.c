#include <drivers/i2c.h>
#include <drivers/gpio.h>
#include <stdint.h>

#include <embox/unit.h>
#include <time.h>

#include <hal/reg.h>
#include <hal/system.h>
#include <hal/arch.h>

#define MAX_I2C_BUS_N OPTION_GET(NUMBER, max_n)

struct i2c_bus buses[MAX_I2C_BUS_N];
uint32_t i2c_bus_number = 0;

static void delay(struct i2c_bus *bus) {
	arch_delay(bus->delay);
}

static inline void set_levels(struct i2c_bus *bus, uint8_t sda_level, uint8_t scl_level) {
	if (sda_level) {
		gpio_settings(bus->sda, 0, GPIO_MODE_INPUT | GPIO_MODE_IN_PULL_UP);
	} else {
		gpio_settings(bus->sda, 0, GPIO_MODE_OUTPUT | GPIO_MODE_OUT_OPEN_DRAIN);
	}
	
	if (scl_level) {
		gpio_settings(bus->scl, 0, GPIO_MODE_INPUT | GPIO_MODE_IN_PULL_UP);
	} else {
		gpio_settings(bus->scl, 0, GPIO_MODE_OUTPUT | GPIO_MODE_OUT_OPEN_DRAIN);
	}
} 

static inline uint8_t get_level(struct gpio *line) {
	return gpio_get_level(line, 0);
}

struct i2c_bus *i2c_init(uint8_t sda_pin, uint8_t scl_pin, uint32_t delay_time) {
	buses[i2c_bus_number].sda = gpio_by_num(sda_pin);
	buses[i2c_bus_number].scl = gpio_by_num(scl_pin);
	
	buses[i2c_bus_number].delay = delay_time;
	
	buses[i2c_bus_number].id = i2c_bus_number;
	
	gpio_set_level(buses[i2c_bus_number].scl, 0, 0);
	gpio_set_level(buses[i2c_bus_number].sda, 0, 0);
	
	set_levels(bus, 1, 1);
	delay(bus);
	
	return &buses[i2c_bus_number++];
}

void i2c_terminate(struct i2c_bus *bus) {
	for (int i = 0; i < i2c_bus_number; i++) {
		if (buses[i].id == bus->id) {
			for (int j = i; j < i2c_bus_number - 1; j++) {
				buses[j] = buses[j + 1];
			}
			
			i2c_bus_number--;
			return;
		}
	}
}

void i2c_start(struct i2c_bus *bus) {
	set_levels(bus, 1, 1);
	delay(bus);
	set_levels(bus, 0, 1);
	delay(bus);
}

void i2c_stop(struct i2c_bus *bus) {
	delay(bus);
	set_levels(bus, 0, 1);
	delay(bus);
	set_levels(bus, 1, 1);
	delay(bus);
}

uint8_t i2c_send(struct i2c_bus *bus, uint8_t data) {
	delay(bus);
	set_levels(bus, 1, 0);
	delay(bus);
	
	for (int8_t i = 7; i >= 0; i--) {
		uint8_t bit = (data >> i) & 1;
		
		set_levels(bus, bit, 0);
		delay(bus);
		set_levels(bus, bit, 1);
		delay(bus);
		
		while (get_level(scl) == 0);
		
		set_levels(bus, bit, 0);
		delay(bus);
	}
	
	set_levels(bus, 1, 0);
	delay(bus);
	set_levels(bus, 1, 1);
	delay(bus);
	
	uint8_t ack = get_level(bus->sda);
	
	set_levels(bus, 1, 0);
	delay(bus);
	
	return ack;
}

uint8_t i2c_receive(struct i2c_bus *bus, uint8_t last) {
	uint8_t data = 0;
	
	delay(bus);
	set_levels(bus, 1, 0);
	delay(bus);
	
	for (int8_t i = 7; i >= 0; i--) {
		set_levels(bus, 1, 0);
		delay(bus);
		set_levels(bus, 1, 1);
		delay(bus);
		
		while (get_level(bus->scl) == 0);
		
		data <<= 1;
		data |= get_level(bus->sda) & 1;
		
		set_levels(bus, 1, 0);
		delay(bus);
	}
	
	set_levels(bus, last, 0);
	delay(bus);
	set_levels(bus, last, 1);
	delay(bus);
	set_levels(bus, last, 0);
	delay(bus);
	
	return data;
}
