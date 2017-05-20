/**
 * @file
 * @brief
 *
 * @author  Ilya Nozhkin
 * @date    22.03.2017
 */

#ifndef XTENSA_REG_H_
#define XTENSA_REG_H_

#define __REG_STORE(addr, value) \
	do { *((volatile char *)(addr)) = (value); } while (0)

#define __REG_LOAD(addr) \
	*((volatile char *)(addr))

#define __REG_ORIN(addr, mask) \
		__REG_STORE(addr, REG_LOAD(addr) | (char)(mask))

#define __REG_ANDIN(addr, mask) \
		__REG_STORE(addr, REG_LOAD(addr) & (char)(mask))
		
#define SREG_WRITE(to, value) \
	asm volatile ("wsr %0, " to : : "r"(value));

#define SREG_READ(from, value) \
	asm volatile ("rsr %0, " from : "=r"(value))
	
#define RSYNC() asm volatile ("rsync")

#endif /* XTENSA_REG_H_ */

