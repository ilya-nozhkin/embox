/**
 * @file
 * @brief
 *
 * @date 21.07.2017
 * @author Egor Shitov
 */

#ifndef DRIVERS_PWM_PWM_H
#define DRIVERS_PWM_PWM_H

#include <stdint.h>

#define NPWM 2

/* HOW TO IMPLEMENT THIS:
 * 1. implement pwm_enable and pwm_disable (e.g. interrupts by timer on/off)
 * 2. every iteration call pwm_get_power
 * 3. if power is enough for this iteration call pwm_on over otherwise call pwm_off
 * */

/* HOW TO USE THIS:
 * 1. add pwm using pwm_add
 * 2. set pwm power via pwm_set_power
 * 3. enable pwm via pwm_enable
 * */

/* depends on platform */
int pwm_enable(void);	// returns 0 on success ?
int pwm_disable(void);	// returns 0 on success ?

/* platform-independent functions (used to communicate with pwms) */
int pwm_add(uint8_t pin);		// returns idx on success otherwise -ENOMEM
int pwm_remove(uint8_t idx);	// returns 0 on success
int pwm_set_power(uint8_t idx, int power);	// returns 0 on success

/* these functions are called by platform-dependent code */
int pwm_get_power(uint8_t idx);
void pwm_on(uint8_t idx);
void pwm_off(uint8_t idx);

#endif /* DRIVERS_PWM_PWM_H */
