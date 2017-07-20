#ifndef DRIVERS_PWM_LX106_PWM_H_
#define DRIVERS_PWM_LX106_PWM_H_

#include <stdint.h>
#include <drivers/gpio.h>

#define MAX_SERVOS 2

typedef struct lx106_pwm {
    uint8_t pin;
    uint8_t power;      /* percents */
    uint8_t enabled;    /* private */
    struct gpio *gpio;  /* private */
    uint8_t last_state;
} lx106_pwm_t;

int lx106_pwm_enable(void);
int lx106_pwm_disable(void);

int lx106_pwm_add_servo(uint8_t idx, lx106_pwm_t servo);
int lx106_pwm_remove_servo(uint8_t idx);

#endif /* DRIVERS_PWM_LX106_PWM_H_ */
