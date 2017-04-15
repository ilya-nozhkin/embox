/**
 * @file
 *
 * @brief
 *
 * @date 15.04.2017
 * @author Ilya Nozhkin
 */

#ifndef XTENSA_PTRACE_H_
#define XTENSA_PTRACE_H_


#ifndef __ASSEMBLER__

typedef struct pt_regs {
	unsigned int ar[16];
	unsigned int sar;
	unsigned int epc1;
} pt_regs_t;

static inline void ptregs_retcode(struct pt_regs *ptregs, int retcode) {
	ptregs->ar[2] = retcode;
}

#else /* assembler */

#endif /* __ASSEMBLER__ */


#endif /* XTENSA_PTRACE_H_ */
