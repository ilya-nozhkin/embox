#include <drivers/pwm/pwm.h>

#include <hal/reg.h>
#include <kernel/irq.h>
#include <drivers/irqctrl.h>

#define IRQ_NR 6
#define CCOMPARE_OFFSET 16000

static uint32_t ccount = 0;
static uint8_t stage = 0; // current stage in percents

static irq_return_t lx106_pwm_handle(int irq_nr, void *data);

static inline void change_ccompare(void){
    SREG_READ("ccount", ccount);
    SREG_WRITE("ccompare0", ccount + CCOMPARE_OFFSET);
}

int pwm_enable(void){
    int res = irq_attach(IRQ_NR, lx106_pwm_handle, 0, NULL, "lx106 pwm handler");
    irqctrl_enable(IRQ_NR);
    change_ccompare();
    return res;
}

int pwm_disable(void){
    irqctrl_disable(IRQ_NR);
    return irq_detach(IRQ_NR, NULL);
}

static irq_return_t lx106_pwm_handle(int irq_nr, void *data){
    for(uint8_t i = 0; i < NPWM; i++){
        int power = pwm_get_power(i);

        power >= stage ? pwm_on(i) : pwm_off(i);
    }

    stage += 1;
    if(stage > 100)
        stage = 1;
    change_ccompare();
    return IRQ_HANDLED;
}
