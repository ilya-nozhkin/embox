#include <stdint.h>
#include <embox/unit.h>
#include <hal/reg.h>
#include <kernel/irq.h>
#include <kernel/panic.h>

#include "context.h"

#define LEVEL_1_INTERRUPT_CAUSE 4

EMBOX_UNIT_INIT(xtensa_exceptions_init);

extern uint32_t exceptions_vector_base;

#define MODULO 37
static uint8_t powers_by_mod[MODULO] = {0,  0,  1,  26, 2,  23,
                                        27, 0,  3,  16, 24, 30,
                                        28, 11, 0,  13, 4,  7,
                                        17, 0,  25, 22, 31, 15,
                                        29, 10, 12, 6,  0,  21,
                                        14, 9,  5,  20, 8,  19,
                                        18};

static inline void return_to_normal_mode() {
	uint32_t ps = 0;
	SREG_READ("ps", ps);
	ps &= ~(1 << 4);
	SREG_WRITE("ps", ps);
}

static inline int fast_handle(struct context *ctx, uint32_t interrupt) {
	uint32_t irq_nr = powers_by_mod[interrupt % MODULO];
	uint32_t mask = 1 << irq_nr;

	if (interrupt & mask) {
		SREG_WRITE("intclear", mask);
		irq_dispatch(irq_nr);
		return IRQ_HANDLED;
	}

	return IRQ_NONE;
}

static inline void full_handle(struct context *ctx, uint32_t interrupt) {
	for (uint8_t i = 0; i < 32; i++) {
		uint32_t mask = 1 << i;
		if (interrupt & mask) {
			SREG_WRITE("intclear", mask);
			irq_dispatch(i);
		}
	}
}

static void level_1_interrupt_handler(struct context *ctx) {
	critical_enter(CRITICAL_IRQ_HANDLER);

	uint32_t interrupt = 0;
	uint32_t intenable = 0;
	SREG_READ("interrupt", interrupt);
	SREG_READ("intenable", intenable);

	interrupt &= intenable;

	//return_to_normal_mode();

	if (fast_handle(ctx, interrupt) != IRQ_HANDLED) {
		full_handle(ctx, interrupt);
	}

	critical_leave(CRITICAL_IRQ_HANDLER);
	critical_dispatch_pending();
}

static void print_context(struct context *ctx) {
	uint32_t epc2 = 0;
	uint32_t epc3 = 0;
	uint32_t depc = 0;
	uint32_t exccause = 0;

	SREG_READ("epc2", epc2);
	SREG_READ("epc3", epc3);
	SREG_READ("depc", depc);
	SREG_READ("exccause", exccause);

	for (int i = 0; i < 16; i++) {
		printk("a%-2d = 0x%08X\t", i, ctx->ar[i]);
		if ((i + 1) % 4 == 0) {
			printk("\n");
		}
	}

	printk("\nepc1 = 0x%08X\tepc2 = 0x%08X\nepc3 = 0x%08X\tdepc = 0x%08X\n", ctx->epc1, epc2, epc3, depc);
	printk("\nps = 0x%08X\tsar = 0x%08X\n", ctx->ps, ctx->sar);
	printk("\nexccause = 0x%X = %d\n", exccause, exccause);
}

static void unknown_exception_handler(struct context *ctx) {
	printk("\nKernel panic (unknown xtensa exception)\nCurrent context:\n\n");
	print_context(ctx);
	panic("");
}

typedef void (*exception_handler_ptr)(struct context*);

static exception_handler_ptr handlers[64];

static int xtensa_exceptions_init(void) {
	for (int i = 0; i < 64; i++) {
		handlers[i] = &unknown_exception_handler;
	}

	handlers[LEVEL_1_INTERRUPT_CAUSE] = &level_1_interrupt_handler;

	SREG_WRITE("vecbase", &exceptions_vector_base);

	return 0;
}

void exception_c_handler(struct context *ctx) {
	ipl_t level = ipl_save();

	uint32_t exccause = 0;
	SREG_READ("exccause", exccause);
	handlers[exccause](ctx);

	ipl_restore(level);
}
