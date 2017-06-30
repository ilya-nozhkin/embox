/**
 * @file
 * @brief
 *
 * @author  Ilya Nozhkin
 * @date    29.06.2017
 */

#include <drivers/servo.h>
#include <drivers/gpio.h>
#include <stdint.h>

struct servo motorA = {.id = 0};
struct servo motorB = {.id = 1};

struct servo *servo_by_id(uint32_t motor_id) {
	return motor_id == 0 ? &motorA : &motorB;
}

void servo_enable(struct servo *motor) {
	if (motor->id == 0) {
		struct gpio *pwmA = gpio_by_num(5);
		gpio_settings(pwmA, 0, GPIO_MODE_OUTPUT);
		gpio_set_level(pwmA, 0, 1);
		
		struct gpio *pwmB = gpio_by_num(12);
		gpio_settings(pwmB, 0, GPIO_MODE_OUTPUT);
		gpio_set_level(pwmB, 0, 1);
		
		struct gpio *dirA = gpio_by_num(4);
		gpio_settings(dirA, 0, GPIO_MODE_OUTPUT);
		gpio_set_level(dirA, 0, 1);
		
		struct gpio *dirB = gpio_by_num(13);
		gpio_settings(dirB, 0, GPIO_MODE_OUTPUT);
		gpio_set_level(dirB, 0, 1);
	}
}

void servo_disable(struct servo *motor) {
}

void servo_set_direction(struct servo *motor, uint32_t direction) {
}
