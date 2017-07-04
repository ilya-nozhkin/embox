#include <drivers/i2c.h>
#include <drivers/gpio.h>
#include <stdint.h>

#include <embox/unit.h>
#include <time.h>

#include <hal/reg.h>
#include <hal/system.h>

#define SDA_PIN_NUMBER 4
#define SCL_PIN_NUMBER 5

struct gpio *sda;
struct gpio *scl;

#define DELAY (SYS_CLOCK/1000000*200)

static void delay(void) {
	usleep(1000);
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

static inline void line_down(struct gpio *line) {
	gpio_settings(scl, 0, GPIO_MODE_INPUT | GPIO_MODE_IN_PULL_DOWN);
	gpio_settings(line, 0, GPIO_MODE_OUTPUT);
	gpio_set_level(line, 0, 0);
}

static inline void line_up(struct gpio *line) {
	gpio_settings(line, 0, GPIO_MODE_INPUT | GPIO_MODE_IN_PULL_UP);
}

static inline uint8_t line_get(struct gpio *line) {
	return gpio_get_level(line, 0);
}

void i2c_start(void) {
	sda = gpio_by_num(SDA_PIN_NUMBER);
	scl = gpio_by_num(SCL_PIN_NUMBER);

	gpio_settings(scl, 0, GPIO_MODE_INPUT | GPIO_MODE_IN_PULL_UP);
	gpio_settings(sda, 0, GPIO_MODE_INPUT | GPIO_MODE_IN_PULL_UP);

	gpio_set_level(scl, 0, 0);
	gpio_set_level(sda, 0, 0);

	line_up(scl);
	delay();
	
	sleep(1);

	line_down(sda);
	delay();
	line_up(sda);
}

void i2c_stop(void) {
	line_down(sda);
	line_up(scl);
	delay();
	line_up(sda);
}

uint8_t i2c_send(uint8_t data) {
	for (uint8_t i = 0; i < 8; i++) {
		uint8_t bit = (data >> (7-i)) & 1;
		line_down(scl);
		if(!bit) {
			line_down(sda);
		}
		delay();
		line_up(scl);
		delay();
		line_up(sda);
	}
	
	line_down(scl);
	delay();
	line_up(scl);
	delay();
	uint8_t check_bit = line_get(sda);
	return check_bit;
}

uint8_t i2c_receive(uint8_t last) {
	uint8_t byte = 0;
	uint8_t sda_level = 0;

	for (uint8_t i = 0; i < 8; i++) {
		byte = byte << 1;
		line_down(scl);
		delay();
		line_up(scl);
		delay();
		sda_level = line_get(sda);

		if (sda_level) {
			byte |= 1;
		}
	}

	line_down(scl);

	if (last == 0)
	{
		line_down(sda);
		delay();
		line_up(scl);
		delay();
		line_up(sda);
	}
	else
	{
		delay();
		line_up(scl);
		delay();
	}

	return byte;
}
