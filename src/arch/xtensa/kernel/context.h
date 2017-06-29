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

//calling convention:
//a0 and a2 is stored on stack (a0 on a1+0, a1 on a1+4, a2 on a1+8) (original a1 is current a1 + 12)
//a1 should be restored manually after this function
//this function can't be called from C, use macro 'context_store' instead
void _context_store(struct context *ctx);

#define context_store(ctx) asm volatile ("addi a1, a1, -12\n" \
                                         "s32i a0, a1, 0\n" \
                                         "s32i a2, a1, 8\n" \
                                         "addi a2, a1, 12\n" \
                                         "s32i a2, a1, 4"); \
                           _context_store(ctx);

//calling convention:
//a2 will be replaced by stored value
//a1 will be replaced by stored value
//a0 will be located on the top of stack (original a1 is current a1 + 4)
//a1 should be restored manually after this function
//this function can't be called from C, use macro 'context_load' instead
void _context_load(struct context *ctx);

#define context_load(ctx) _context_load(ctx); \
                          asm volatile ("l32i a0, a1, 0\n" \
                                        "addi a1, a1, 4");

void context_switch(struct context *prev, struct context *next);
void context_init(struct context *ctx, unsigned int flags,
                  void (*routine_fn)(void), void *sp);

#else

#endif

#endif /* ARCH_MXTENSA_CONTEXT_H_ */
