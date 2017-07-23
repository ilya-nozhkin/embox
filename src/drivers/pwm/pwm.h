#ifndef DRIVERS_PWM_PWM_H
#define DRIVERS_PWM_PWM_H

#include <stdint.h>

#define NPWM 2

typedef struct pwm{
    uint8_t pin;
    uint8_t power;
} pwm_t;

/* depends on platform */
int pwm_enable(void);
int pwm_disable(void);

/* platform-independent functions */
int pwm_add(uint8_t pin);
int pwm_remove(uint8_t idx);
int pwm_set_power(uint8_t idx, uint8_t power);
int pwm_get_power(uint8_t idx);
void pwm_on(uint8_t idx);
void pwm_off(uint8_t idx);

#endif /* DRIVERS_PWM_PWM_H */
