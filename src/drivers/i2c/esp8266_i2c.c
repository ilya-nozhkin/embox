#include <drivers/i2c.h>
#include <drivers/gpio.h>
#include <stdint.h>

#include <embox/unit.h>
#include <time.h>

#include <hal/reg.h>
#include <hal/system.h>

#define SDA_PIN_NUMBER 5
#define SCL_PIN_NUMBER 4

struct gpio *sda;
struct gpio *scl;

#define DELAY (SYS_CLOCK/1000000*200)

static void delay(void) {
	usleep(100000);
	/*uint32_t last = 0;
	SREG_READ("ccount", last);
	RSYNC();

	while (1) {
		uint32_t current = 0;
		SREG_READ("ccount", current);
		RSYNC();

		if ((current - last)/160 >= 50) {
			break;
		}

		last = current;
	}*/
}

static inline void set_levels(uint8_t sda_level, uint8_t scl_level) {
	if (sda_level) {
		gpio_settings(sda, 0, GPIO_MODE_INPUT | GPIO_MODE_IN_PULL_UP);
	} else {
		gpio_settings(sda, 0, GPIO_MODE_OUTPUT | GPIO_MODE_OUT_OPEN_DRAIN);
		gpio_set_level(sda, 0, 0);
	}
	
	if (scl_level) {
		gpio_settings(scl, 0, GPIO_MODE_INPUT | GPIO_MODE_IN_PULL_UP);
	} else {
		gpio_settings(scl, 0, GPIO_MODE_OUTPUT | GPIO_MODE_OUT_OPEN_DRAIN);
		gpio_set_level(scl, 0, 0);
	}
} 

static inline uint8_t get_level(struct gpio *line) {
	return gpio_get_level(line, 0);
}

void i2c_start(void) {
	sda = gpio_by_num(SDA_PIN_NUMBER);
	scl = gpio_by_num(SCL_PIN_NUMBER);

	gpio_settings(scl, 0, GPIO_MODE_INPUT | GPIO_MODE_IN_PULL_UP);
	gpio_settings(sda, 0, GPIO_MODE_INPUT | GPIO_MODE_IN_PULL_UP);

	gpio_set_level(scl, 0, 0);
	gpio_set_level(sda, 0, 0);

	set_levels(1, 1);
	
	sleep(1);

	set_levels(1, 1);
	delay();
	set_levels(0, 1);
	delay();
}

void i2c_stop(void) {
	/*line_down(sda);
	line_up(scl);
	delay();
	line_up(sda);*/
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
		
		if (i == 0) {
            delay();
        }
		
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
}
