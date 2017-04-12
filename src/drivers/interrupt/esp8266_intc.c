/**
 * @file
 * @brief
 *
 * @author  Ilya Nozhkin
 * @date    04.04.2017
 */

#include <drivers/irqctrl.h>
#include <kernel/printk.h>
#include <inttypes.h>

extern void ets_isr_mask(uint32_t);
extern void ets_isr_unmask(uint32_t);
extern _xtos_handler	_xtos_set_interrupt_handler( int n, _xtos_handler f );
extern _xtos_handler	_xtos_set_interrupt_handler_arg( int n, _xtos_handler f, void *arg );

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

int irqctrl_pending(unsigned int irq) {
	printk("irqctrl_pending #%u\n", irq);
	return 0;
}

_xtos_handler set_irq_handler(int irq_n, _xtos_handler handler, void *arg)
{
	if(arg)
		return _xtos_set_interrupt_handler_arg(irq_n, handler, arg);
	else return _xtos_set_interrupt_handler(irq_n, handler);
}
