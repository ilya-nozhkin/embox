/**
 * @file
 * @brief
 *
 * @author  Ilya Nozhkin
 * @date    04.04.2017
 */

#include <drivers/irqctrl.h>
#include <inttypes.h>

extern void ets_isr_mask(uint32_t);
extern void ets_isr_unmask(uint32_t);

void irqctrl_enable(unsigned int interrupt_nr) {
	ets_isr_unmask(1 << interrupt_nr);
}

void irqctrl_disable(unsigned int interrupt_nr) {
	ets_isr_mask(1 << interrupt_nr);
}

void irqctrl_clear(unsigned int interrupt_nr) {
	
}

void irqctrl_force(unsigned int interrupt_nr) {
}
