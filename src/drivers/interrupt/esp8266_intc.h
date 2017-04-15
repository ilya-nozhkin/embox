/**
 * @file
 * @brief
 *
 * @author  Ilya Nozhkin
 * @author 	Egor Shitov
 *			- Interrupt handler
 *			- RSIL
 * @date    04.04.2017
 */

#ifndef DRIVERS_INTERRUPT_ESP8266_INTC_H_
#define DRIVERS_INTERRUPT_ESP8266_INTC_H_

#define __IRQCTRL_IRQS_TOTAL 32

typedef void (_xtos_handler_func)();
typedef _xtos_handler_func *_xtos_handler;

// Read and set interrupt level
#define INTLEVEL_SET(prev, level) \
	asm volatile ("rsil %0," #level : "=r"(prev))
#define INTLEVEL_RESTORE(prev) \
	asm volatile ("wsr.ps %0" : : "r"(prev))

#endif /* DRIVERS_INTERRUPT_ESP8266_INTC_H_ */
