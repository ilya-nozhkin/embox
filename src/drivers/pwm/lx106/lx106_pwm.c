#include <drivers/pwm.h>

#include <hal/reg.h>
#include <kernel/irq.h>
#include <drivers/irqctrl.h>

#define IRQ_NR 6

#define CYCLE_LENGTH 1600000
#define MAX_POWER 100

struct pwm_node {
	uint32_t offset;
	uint8_t pwm_id;
};

static struct pwm_node pwm_queue[NPWM] = {0};
static uint32_t full_offset = 0;
static uint32_t stage = 0;

static irq_return_t lx106_pwm_handle(int irq_nr, void *data);

static inline void sort_queue(void) {
	for (uint8_t i = NPWM; i > 0; i--) {
		for (int j = 0; j < i - 1; j++) {
			if (pwm_queue[j].offset > pwm_queue[j + 1].offset) {
				struct pwm_node temp = pwm_queue[j + 1];
				pwm_queue[j + 1] = pwm_queue[j];
				pwm_queue[j] = temp;
			}
		}
	}
}

static inline uint32_t get_ccount(void) {
	uint32_t ccount = 0;
    SREG_READ("ccount", ccount);
    return ccount;
}

static inline void set_ccompare(uint32_t ccompare) {
    SREG_WRITE("ccompare0", ccompare);
}

int pwm_enable(void){
	for (uint8_t i = 0; i < NPWM; i++) {
		pwm_off(i);
		pwm_set_power(i, 0);
	}
	
    int res = irq_attach(IRQ_NR, lx106_pwm_handle, 0, NULL, "lx106 pwm handler");
    irqctrl_enable(IRQ_NR);
    
    stage = 0;
    set_ccompare(get_ccount() + CYCLE_LENGTH);
    
    return res;
}

int pwm_disable(void){
    irqctrl_disable(IRQ_NR);
    return irq_detach(IRQ_NR, NULL);
}

static irq_return_t lx106_pwm_handle(int irq_nr, void *data){
    if (stage == 0) {
		for (int i = 0; i < NPWM; i++) {
			pwm_queue[i].pwm_id = i;
			pwm_queue[i].offset = pwm_get_power(i) * CYCLE_LENGTH / MAX_POWER;
		}
		
		sort_queue();
		
		while (stage < NPWM && pwm_queue[stage].offset == 0) {
			pwm_off(stage);
			stage++;
		}
		
		for (uint8_t i = stage; i < NPWM; i++) {
			pwm_on(pwm_queue[i].pwm_id);
		}
		
		for (int8_t i = NPWM - 1; i >= (int8_t) stage; i--) {
			uint32_t previous = i == 0 ? 0 : pwm_queue[i - 1].offset;
			pwm_queue[i].offset -= previous;
		}
		
		if (stage < NPWM && pwm_queue[stage].offset < CYCLE_LENGTH) {
			set_ccompare(get_ccount() + pwm_queue[stage].offset);
			stage++;
		} else {
			set_ccompare(get_ccount() + CYCLE_LENGTH);
			stage = 0;
		}
		
		full_offset =  0;
	} else {
		pwm_off(pwm_queue[stage - 1].pwm_id);
		full_offset += pwm_queue[stage - 1].offset;
		
		while (stage < NPWM && pwm_queue[stage].offset == 0) {
			pwm_off(pwm_queue[stage].pwm_id);
			stage++;
		}
		
		if (stage < NPWM && pwm_queue[stage].offset < CYCLE_LENGTH) {
			set_ccompare(get_ccount() + pwm_queue[stage].offset);
			stage++;
		} else {
			int32_t next_cycle = CYCLE_LENGTH - full_offset;
			if (next_cycle <= 0) {
				next_cycle = CYCLE_LENGTH / MAX_POWER;
			}
			set_ccompare(get_ccount() + next_cycle);
			stage = 0;
		}
	}
    
    return IRQ_HANDLED;
}
