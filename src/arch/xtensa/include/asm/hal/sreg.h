/**
 * @file
 * @brief Handle Special Registers
 *
 * @author  Egor Shitov
 * @date    12.04.2017
 */

#ifndef XTENSA_SREG_H_
#define XTENSA_SREG_H_

#define WRITE_SREG(to, value) \
	asm volatile ("wsr %0, " to: :"r"(value));

#define READ_SREG(from, value) \
	asm volatile ("rsr %0, " from : "=r"(value))
	
#define RSYNC() asm volatile ("rsync")

#endif /* XTENSA_SREG_H_ */
