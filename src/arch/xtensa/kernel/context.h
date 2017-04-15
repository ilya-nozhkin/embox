/**
 * @file
 *
 * @brief
 *
 * @date 15.04.2017
 * @author Ilya Nozhkin
 */

#ifndef ARCH_MXTENSA_CONTEXT_H_
#define ARCH_MXTENSA_CONTEXT_H_

#ifndef __ASSEMBLER__

struct context {
	unsigned int ar[16];
	unsigned int sar;
	unsigned int ps;
	unsigned int epc1;
};

#else

#endif

#endif /* ARCH_MXTENSA_CONTEXT_H_ */
