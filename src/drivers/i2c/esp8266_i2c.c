#include <drivers/i2c.h>
#include <drivers/gpio.h>
#include <stdint.h>

#include <embox/unit.h>

#define DELAY

strcut gpio* SDA;
strcut gpio* SCL;

void i2c_start(void) {
}

void i2c_stop(void) {
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

static inline void lineDown (strcut gpio* gpio)
{
	gpio_settings(gpio, 0, GPIO_MODE_OUTPUT);
}

static inline void lineUp (strcut gpio* gpio)
{
	gpio_settings(gpio, 0, GPIO_MODE_INPUT);
}
