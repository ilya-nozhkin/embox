#include "pwm.h"

#include <errno.h>
#include <mem/misc/pool.h>
#include <util/indexator.h>
#include <drivers/gpio.h>

INDEX_DEF(pwm_idx,0,NPWM);

static struct gpio *gpios[NPWM];
static pwm_t pwms[NPWM];

int pwm_add(uint8_t pin){
    pwm_t pwm;
    int idx;

    if((idx = index_alloc(&pwm_idx, INDEX_MIN)) < 0)
        return -ENOMEM;

    pwm.pin = pin;
    pwm.power = 0;

    pwms[idx] = pwm;
    gpios[idx] = gpio_by_num(pin);

    gpio_settings(gpios[idx], 0, GPIO_MODE_OUTPUT);

    return idx;
}

int pwm_remove(uint8_t idx){
    if(idx >= NPWM)
        return -1;

    index_free(&pwm_idx, idx);
    pwms[idx].power = 0;

    return 0;
}

int pwm_set_power(uint8_t idx, uint8_t power){
    if(idx >= NPWM)
        return -1;

    pwms[idx].power = power;
    return 0;
}

int pwm_get_power(uint8_t idx){
    if(idx >= NPWM)
        return -1;

    return pwms[idx].power;
}

void pwm_on(uint8_t idx){
    gpio_set_level(gpios[idx], 0, 1);
}

void pwm_off(uint8_t idx){
    gpio_set_level(gpios[idx], 0, 0);
}
