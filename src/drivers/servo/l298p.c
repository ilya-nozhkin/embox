/**
 * @file
 * @brief
 *
 * @author  Ilya Nozhkin
 * @date    29.06.2017
 */

#include <drivers/servos/l298p.h>
#include <drivers/gpio.h>
#include <stdint.h>

#include <embox/unit.h>

#define L298P_MAX_N OPTION_GET(NUMBER, max_n)

struct l298p {
	struct gpio *pwma;
	struct gpio *pwmb;
	struct gpio *dira;
	struct gpio *dirb;
};

struct l298p l298p_drivers[L298P_MAX_N];
uint32_t l298p_number = 0;

struct l298p *l298p_start(uint8_t pwma, uint8_t pwmb, uint8_t dira, uint8_t dirb) {
	struct l298p *driver = &l298p_drivers[l298p_number++];
	
	driver->pwma = gpio_by_num(pwma);
	driver->pwmb = gpio_by_num(pwmb);
	driver->dira = gpio_by_num(dira);
	driver->dirb = gpio_by_num(dirb);
	
	gpio_settings(driver->pwma, 0, GPIO_MODE_OUTPUT | GPIO_MODE_OUT_PUSH_PULL);
	gpio_settings(driver->pwmb, 0, GPIO_MODE_OUTPUT | GPIO_MODE_OUT_PUSH_PULL);
	gpio_settings(driver->dira, 0, GPIO_MODE_OUTPUT | GPIO_MODE_OUT_PUSH_PULL);
	gpio_settings(driver->dirb, 0, GPIO_MODE_OUTPUT | GPIO_MODE_OUT_PUSH_PULL);
	
	return driver;
}

void l298p_servo_set(struct l298p *driver, uint8_t servo, int8_t power) {
	if (servo == 0) {
		gpio_set_level(driver->dira, 0, power >= 0 ? 1 : 0);
		gpio_set_level(driver->pwma, 0, power != 0 ? 1 : 0);
	} else if (servo == 1) {
		gpio_set_level(driver->dirb, 0, power >= 0 ? 1 : 0);
		gpio_set_level(driver->pwmb, 0, power != 0 ? 1 : 0);
	}
}
