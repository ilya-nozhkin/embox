/**
 * @file
 * @brief
 *
 * @author  Ilya Nozhkin
 * @date    29.06.2017
 */

#include <drivers/servos/l298p.h>
#include <drivers/gpio.h>
#include <drivers/pwm.h>
#include <stdint.h>

#include <embox/unit.h>

#define L298P_MAX_N OPTION_GET(NUMBER, max_n)

struct l298p {
	int pwma;
	int pwmb;
	struct gpio *dira;
	struct gpio *dirb;
};

struct l298p *l298p_start(uint8_t pwma, uint8_t pwmb, uint8_t dira, uint8_t dirb) {
	struct l298p *driver = (struct l298p*) malloc(sizeof(struct l298p));
	
	pwm_enable();
	
	driver->pwma = pwm_add(pwma);
	driver->pwmb = pwm_add(pwmb);
	driver->dira = gpio_by_num(dira);
	driver->dirb = gpio_by_num(dirb);
	
	pwm_set_power(driver->pwma, 0);
	pwm_set_power(driver->pwmb, 0);
	
	gpio_settings(driver->dira, 0, GPIO_MODE_OUTPUT | GPIO_MODE_OUT_PUSH_PULL);
	gpio_settings(driver->dirb, 0, GPIO_MODE_OUTPUT | GPIO_MODE_OUT_PUSH_PULL);
	
	return driver;
}

void l298p_finish(struct l298p *driver) {
	pwm_remove(driver->pwma);
	pwm_remove(driver->pwmb);
	gpio_settings(driver->dira, 0, GPIO_MODE_INPUT);
	gpio_settings(driver->dirb, 0, GPIO_MODE_INPUT);
	free(driver);
}

void l298p_servo_set(struct l298p *driver, uint8_t servo, int8_t power) {
	if (servo == 0) {
		gpio_set_level(driver->dira, 0, power >= 0 ? 1 : 0);
		pwm_set_power(driver->pwma, power > 0 ? power : -power);
	} else if (servo == 1) {
		gpio_set_level(driver->dirb, 0, power >= 0 ? 1 : 0);
		pwm_set_power(driver->pwmb, power > 0 ? power : -power);
	}
}
