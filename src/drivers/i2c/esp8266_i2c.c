#include <drivers/i2c.h>
#include <drivers/gpio.h>
#include <stdint.h>

#include <embox/unit.h>

#include <hal/reg.h>
#include <hal/system.h>

#define SDA_PIN_NUMBER 2
#define SCL_PIN_NUMBER 14

struct gpio *sda;
struct gpio *scl;

#define DELAY (SYS_CLOCK/1000000*20)

static void delay() {
	uint32_t last = 0;
	SREG_READ("ccount", last);
	
	while (1) {
		uint32_t current = 0;
		SREG_READ("ccount", current);
		RSYNC();
		
		if (current - last >= DELAY) {
			break;
		}
		
		last = current;
	}
}

static inline void line_down(struct gpio *line) {
	gpio_settings(line, 0, GPIO_MODE_OUTPUT);
	gpio_set_level(line, 0, 0);
}

static inline void line_up(struct gpio *line) {
	gpio_settings(line, 0, GPIO_MODE_INPUT | GPIO_MODE_IN_PULL_UP);
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
}

uint8_t i2c_receive(uint8_t last) {

	lineDown (SCL);

	uint8_t bite=0;
	struct gpio_mask_t SDAlevel;

	for (unsigned int i = 0; i < 8; i++ )
	{
		bite = bite<<1;
		delay();
		lineUp(SCL);
		SDAlevel = gpio_get_level(SDA, 0);

		if (SDAlevel == 1)
		{
			bite = bite | 1;
		}

		lineDown (SCL);
	}

	delay();

	if (last == 0)
	{
		lineDown(SDA);
		lineUp(SCL);
		delay();
		lineDown(SCL);
		lineUp(SDA);
	}
	else
	{
		lineUp(SCL);
		delay();
		lineDown(SCL);
	}


	return bite;
}
