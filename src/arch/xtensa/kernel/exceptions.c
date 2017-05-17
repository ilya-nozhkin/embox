#include <stdint.h>
#include <embox/unit.h>
#include <hal/reg.h>
#include <kernel/irq.h>

#include "context.h"

EMBOX_UNIT_INIT(xtensa_exceptions_init);

extern uint32_t exceptions_vector_base;

static int xtensa_exceptions_init(void) {
	SREG_WRITE("vecbase", &exceptions_vector_base);
	return 0;
}

#define MODULE 37
static uint8_t powers_by_mod[MODULE] = {255, 0,   1,   26, 2,   23, 
                                        27,  255, 3,   16, 24,  30,
                                        28,  11,  255, 13, 4,   7,
                                        17,  255, 25,  22, 31,  15,
                                        29,  10,  12,  6,  255, 21,
                                        14,  9,   5,   20, 8,   19,
                                        18};

void user_exception_c_handler(struct context *ctx) {
	uint32_t exccause = 0;
	SREG_READ("exccause", exccause);
	
	if (exccause == 4) {
		uint32_t interrupt = 0;
		uint32_t intenable = 0;
		SREG_READ("interrupt", interrupt);
		SREG_READ("intenable", intenable);
		
		interrupt &= intenable;
		
		uint32_t irq_nr = powers_by_mod[interrupt % MODULE];
		
		SREG_WRITE("intclear", interrupt);
		
		uint32_t ps = 0;
		SREG_READ("ps", ps);
		ps &= ~(1 << 4);
		SREG_WRITE("ps", ps);
		
		irq_dispatch(irq_nr);
	}
	
	/*for (int i = 0; i < 16; i++) {
		printk("a%d = %x\n", i, ctx->ar[i]);
	}
	printk("sar = %x\n", ctx->sar);
	printk("ps = %x\n", ctx->ps);
	printk("epc1 = %x\n", ctx->epc1);
	
	uint32_t value;
	asm volatile ("rsr.exccause %0" : "=r"(value));
	printk("exccause = %x\n", value);
	
	asm volatile ("rsr.epc2 %0" : "=r"(value));
	printk("epc2 = %x\n", value);
	
	asm volatile ("rsr.epc3 %0" : "=r"(value));
	printk("epc3 = %x\n", value);
	
	asm volatile ("rsr.depc %0" : "=r"(value));
	printk("depc = %x\n", value);*/
}
