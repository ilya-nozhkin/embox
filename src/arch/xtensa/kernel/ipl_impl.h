/**
 * @file
 *
 * @brief
 *
 * @date 18.05.2017
 * @author Ilya Nozhkin
 */

#ifndef XTENSA_ASM_IPL_H_
#define XTENSA_ASM_IPL_H_

#include <hal/reg.h>

typedef uint32_t __ipl_t;

static inline void ipl_init(void) {
	asm volatile ("rsil a2, 0");
}

static inline __ipl_t ipl_save(void) {
	uint32_t ps = 0;
	asm volatile ("rsil %0, 3" : "=r"(ps));
	return ps & 0xF;
}

static inline void ipl_restore(__ipl_t ipl) {
	uint32_t ps = 0;
	SREG_READ("ps", ps);
	ps = (ps & 0xFFFFFFF0) | (ipl & 0xF);
	SREG_WRITE("ps", ps);
}

#endif /* XTENSA_ASM_IPL_H_ */
