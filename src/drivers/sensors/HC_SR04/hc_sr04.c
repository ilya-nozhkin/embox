#include "hc_sr04.h"

#include <hal/clock.h>
#include <hal/system.h>
#include <unistd.h>
#include <stdlib.h>
#include <drivers/gpio.h>

#define SENSOR_TIMEOUT 2


static struct gpio* trig;
static struct gpio* echo;

static inline uint32_t timestamp(void){
    uint32_t ccount;
    asm volatile ("rsr %0, ccount" : "=r"(ccount));
    return ccount / (SYS_CLOCK / 1000000);
}



static inline uint32_t wait_level_change(gpio_mask_t current_level){
	uint32_t time_start = timestamp();
	while(1){
		gpio_mask_t level = gpio_get_level(echo, 0);
		if(level != current_level)
			return level;
	}
	return timestamp() - time_start;
}

void sensor_setup(uint8_t trig_pin, uint8_t echo_pin){
	trig = gpio_by_num(trig_pin);
	echo = gpio_by_num(echo_pin);
	gpio_settings(trig, 0, GPIO_MODE_OUTPUT);
	gpio_set_level(trig, 0, 0);
	gpio_settings(echo, 0, GPIO_MODE_INPUT);
}

void sensor_request(void){
	gpio_set_level(trig, 0, 1);
	usleep(14*1000);
	gpio_set_level(trig, 0, 0);
}

uint32_t sensor_response(void){
	uint32_t response_time;
	uint32_t time_start = timestamp();
	uint32_t delta = 0;
	while (1){
		gpio_mask_t level = gpio_get_level(echo, 0);
		if(level == 1)
		{
			printf("level=1\n");
			response_time = wait_level_change(1);

			if (response_time == 38*1000){
				return 1; // No obstacles
			}
			return response_time / 58;
		}

		delta = timestamp() - time_start;
		printf("%d\n", delta);
		if (delta >= SENSOR_TIMEOUT){
			return 2; //Timeout
		}
	}
}
