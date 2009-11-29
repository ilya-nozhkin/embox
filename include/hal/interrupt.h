/**
 * @file interrupt.h
 *
 * @brief Interrupt controller interface.
 *
 * @date 25.11.2009
 * @author Eldar Abusalimov
 */

#ifndef HAL_INTERRUPT_H_
#define HAL_INTERRUPT_H_

#include <asm/haldefs.h>

/**
 * Total amount of interrupt lines available on the controller.
 *
 * @note Implementation should provide @link __INTERRUPT_NRS_TOTAL @endlink
 * definition indicating positive constant.
 */
#define INTERRUPT_NRS_TOTAL __INTERRUPT_NRS_TOTAL

/**
 * Checks if the specified interrupt_nr is less then
 * @link INTERRUPT_NRS_TOTAL @endlink value.
 */
#define interrupt_nr_valid(interrupt_nr) \
	((interrupt_nr_t) interrupt_nr < (interrupt_nr_t) INTERRUPT_NRS_TOTAL)

/**
 * Type representing interrupt line number.
 *
 * @note Implementation should provide @link __interrupt_nr_t @endlink type
 * indicating unsigned (it is essential!) integer suitable to hold up to
 * @link INTERRUPT_NRS_TOTAL @endlink values.
 */
typedef __interrupt_nr_t interrupt_nr_t;

/**
 * Initializes interrupt controller
 *
 * @note Implementation have to perform basic controller initialization,
 * disabling all interrupts and clearing any pending IRQs (if needed).
 */
void interrupt_init();

/**
 * Enables the specified IRQ.
 *
 * @param interrupt_nr the IRQ number to enable
 */
void interrupt_enable(interrupt_nr_t interrupt_nr);

/**
 * Disables the specified IRQ.
 *
 * @param interrupt_nr the IRQ number to disable
 */
void interrupt_disable(interrupt_nr_t interrupt_nr);

/**
 * Clears pending status for the specified IRQ.
 *
 * @param interrupt_nr the IRQ number to clear
 */
void interrupt_clear(interrupt_nr_t interrupt_nr);

/**
 * Forces interrupt controller to generate the specified IRQ.
 *
 * @param interrupt_nr the IRQ number to force
 */
void interrupt_force(interrupt_nr_t interrupt_nr);

#endif /* HAL_INTERRUPT_H_ */
