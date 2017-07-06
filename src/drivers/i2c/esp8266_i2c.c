#include <drivers/i2c.h>
#include <drivers/gpio.h>
#include <stdint.h>

#include <embox/unit.h>
#include <time.h>

#include <hal/reg.h>
#include <hal/system.h>
#include <hal/arch.h>

struct gpio *sda;
struct gpio *scl;

uint32_t delay_time;

static void delay(void) {
	arch_delay(delay_time);
}

static inline void set_levels(uint8_t sda_level, uint8_t scl_level) {
	if (sda_level) {
		gpio_settings(sda, 0, GPIO_MODE_INPUT | GPIO_MODE_IN_PULL_UP);
	} else {
		gpio_settings(sda, 0, GPIO_MODE_OUTPUT | GPIO_MODE_OUT_OPEN_DRAIN);
	}
	
	if (scl_level) {
		gpio_settings(scl, 0, GPIO_MODE_INPUT | GPIO_MODE_IN_PULL_UP);
	} else {
		gpio_settings(scl, 0, GPIO_MODE_OUTPUT | GPIO_MODE_OUT_OPEN_DRAIN);
	}
} 

static inline uint8_t get_level(struct gpio *line) {
	return gpio_get_level(line, 0);
}

void i2c_init(uint8_t sda_pin, uint8_t scl_pin, uint32_t _delay_time) {
	sda = gpio_by_num(sda_pin);
	scl = gpio_by_num(scl_pin);
	
	delay_time = _delay_time;
	
	gpio_set_level(scl, 0, 0);
	gpio_set_level(sda, 0, 0);
	
	set_levels(1, 1);
	delay();
}

void i2c_start(void) {
	set_levels(1, 1);
	delay();
	set_levels(0, 1);
	delay();
}

void i2c_stop(void) {
	delay();
	set_levels(0, 1);
	delay();
	set_levels(1, 1);
	delay();
}

uint8_t i2c_send(uint8_t data) {
	delay();
	set_levels(1, 0);
	delay();
	
	for (int8_t i = 7; i >= 0; i--) {
		uint8_t bit = (data >> i) & 1;
		
		set_levels(bit, 0);
		delay();
		set_levels(bit, 1);
		delay();
		
		while (get_level(scl) == 0);
		
		set_levels(bit, 0);
		delay();
	}
	
	set_levels(1, 0);
	delay();
	set_levels(1, 1);
	delay();
	
	uint8_t ack = get_level(sda);
	
	set_levels(1, 0);
	delay();
	
	return ack;
}

uint8_t i2c_receive(uint8_t last) {
	uint8_t data = 0;
	
	delay();
	set_levels(1, 0);
	delay();
	
	for (int8_t i = 7; i >= 0; i--) {
		set_levels(1, 0);
		delay();
		set_levels(1, 1);
		delay();
		
		while (get_level(scl) == 0);
		
		data <<= 1;
		data |= get_level(sda) & 1;
		
		set_levels(1, 0);
		delay();
	}
	
	set_levels(last, 0);
	delay();
	set_levels(last, 1);
	delay();
	set_levels(last, 0);
	delay();
	
	return data;
}
