#include "pwm.h"

#include <hal/reg.h>
#include <kernel/irq.h>
#include <drivers/irqctrl.h>

#define IRQ_NR 6
#define CCOMPARE_OFFSET 16000

static uint32_t ccount = 0;
static uint8_t stage = 0; // current stage in percents
static lx106_pwm_t servos[MAX_SERVOS] = {0};

static irq_return_t lx106_pwm_handle(int irq_nr, void *data);

static inline void change_ccompare(void){
    SREG_READ("ccount", ccount);
    SREG_WRITE("ccompare0", ccount + CCOMPARE_OFFSET);
}

int lx106_pwm_enable(void){
    int res = irq_attach(IRQ_NR, lx106_pwm_handle, 0, NULL, "lx106 pwm handler");
    irqctrl_enable(IRQ_NR);
    change_ccompare();
    return res;
}

int lx106_pwm_disable(void){
    irqctrl_disable(IRQ_NR);
    return irq_detach(IRQ_NR, NULL);
}

static irq_return_t lx106_pwm_handle(int irq_nr, void *data){
    for(uint8_t i = 0; i < MAX_SERVOS; i++){
        lx106_pwm_t *servo = &servos[i];

        uint8_t state = servo->power >= stage;

        if(state != servo->last_state){
            gpio_set_level(servo->gpio, 0, state);

            servo->last_state = state;
        }
    }

    stage += 1;
    if(stage > 100)
        stage = 1;
    change_ccompare();
    return IRQ_HANDLED;
}

int lx106_pwm_add_servo(uint8_t idx, lx106_pwm_t servo){
    if(idx >= MAX_SERVOS)
        return -1;

    servo.enabled = 1;
    servo.gpio = gpio_by_num(servo.pin);
    servos[idx] = servo;

    gpio_settings(servo.gpio, 0, GPIO_MODE_OUTPUT);
    return 0;
}

int lx106_pwm_remove_servo(uint8_t idx){
    servos[idx].enabled = 0;
}
