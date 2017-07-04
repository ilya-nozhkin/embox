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

#define PWMA 5
#define PWMB 12
#define DIRA 4
#define DIRB 13

struct servo *servo_by_id(uint32_t motor_id) {
	return motor_id == 0 ? &motorA : &motorB;
}

void servo_enable(struct servo *motor) {
	struct gpio *pwm = gpio_by_num(motor->id == 0 ? PWMA : PWMB);

	gpio_settings(pwm, 0, GPIO_MODE_OUTPUT);
	gpio_set_level(pwm, 0, 1);
}

void servo_disable(struct servo *motor) {
	struct gpio *pwm = gpio_by_num(motor->id == 0 ? PWMA : PWMB);

	gpio_settings(pwm, 0, GPIO_MODE_OUTPUT);
	gpio_set_level(pwm, 0, 0);
}

void servo_set_direction(struct servo *motor, uint32_t direction) {
	struct gpio *dir = gpio_by_num(motor->id == 0 ? DIRA : DIRB);

	gpio_settings(dir, 0, GPIO_MODE_OUTPUT);
	gpio_set_level(dir, 0, direction);
}
