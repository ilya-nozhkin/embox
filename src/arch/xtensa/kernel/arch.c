/**
 * @file
 * @brief
 *
 * @date 22.03.2017
 * @author Ilya Nozhkin
 */

#ifndef ARCH_C
#define ARCH_C
 
#include <stdint.h>
#include <hal/system.h>
#include <hal/reg.h>
#include <hal/arch.h>
 
int __sync_bool_compare_and_swap_4(unsigned long *addr, unsigned long old, unsigned long new) {
	if (*addr != old) {
		return 0;
	}
	*addr = new;
	return 1;
}

void arch_init(void) {

}

void arch_idle(void) {

}

void _NORETURN arch_shutdown(arch_shutdown_mode_t mode) {
	while (1);
}

void arch_delay(uint32_t useconds) {
	const uint32_t useconds_cycles = SYS_CLOCK / 1000000;
	useconds *= useconds_cycles;
	
	uint32_t last;
	SREG_READ("ccount", last);

	while (1) {
		uint32_t current = 0;
		SREG_READ("ccount", current);

		if ((current - last) >= useconds) {
			break;
		}
	}
}

#endif
