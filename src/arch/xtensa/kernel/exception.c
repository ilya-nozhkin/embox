/**
 * @file exception.c
 * @brief Setup exception handler / print stack trace
 *
 * @author  Egor Shitov
 * @date    11.05.2017
 */

#include <hal/exception.h>
#include <kernel/printk.h>

typedef long unsigned int luint32_t;

struct XTensa_exception_frame_s {
	luint32_t pc;
	luint32_t ps;
	luint32_t sar;
	luint32_t vpri;
	luint32_t a0;
	luint32_t a[14]; //a2..a15
	// The following are added manually by the exception code; the HAL doesn't set these on an exception.
	luint32_t litbase;
	luint32_t sr176;
	luint32_t sr208;
	luint32_t a1;
	luint32_t reason;
	luint32_t excvaddr;
};

extern void ets_wdt_disable();
//extern void ets_wdt_enable();
extern void _xtos_set_exception_handler(int cause, void (exhandler)(struct XTensa_exception_frame_s *frame));
extern void ets_memcpy(void*, void*, unsigned int);
extern void save_extra_sfrs_for_exception(void);
struct XTensa_exception_frame_s savedRegs;

static unsigned int getaregval(int reg) {
	if (reg==0) return savedRegs.a0;
	if (reg==1) return savedRegs.a1;
	return savedRegs.a[reg-2];
}

static void print_stack(luint32_t start, luint32_t end) {
	luint32_t pos = 0;
	printk("\nStack dump:\n");
	printk("================================================================\n");
	for (pos = start; pos < end; pos += 0x10) {
		luint32_t* values = (luint32_t*)(pos);
		// rough indicator: stack frames usually have SP saved as the second word
		unsigned int looksLikeStackFrame = (values[2] == pos + 0x10);

		printk("%08lx:  %08lx %08lx %08lx %08lx %c\n",
		pos, values[0], values[1], values[2], values[3], (looksLikeStackFrame)?'<':' ');
	}
	printk("\n");
	printk("================================================================\n");
}

static void print_reason() {
	struct XTensa_exception_frame_s *reg = &savedRegs;
	printk("\n\n***** Fatal exception %ld\n", reg->reason);
	printk("pc=0x%08lx ", reg->pc);
	printk("sp=0x%08lx ", reg->a1);
	printk("excvaddr=0x%08lx\n", reg->excvaddr);

	printk("ps=0x%08lx ", reg->ps);
	printk("sar=0x%08lx ", reg->sar);
	printk("vpri=0x%08lx\n", reg->vpri);

	for (unsigned int i=0; i<16; i++) {
		unsigned int r = getaregval(i);
		printk("r%02d: 0x%08x=%10d ", i, r, r);
		if (i%3 == 2) printk("\n");
	}

	printk("\n");
	print_stack(getaregval(1), 0x3fffffb0);
}

static void exception_handler(struct XTensa_exception_frame_s *frame) {
	//Save the extra registers the Xtensa HAL doesn't save
  	save_extra_sfrs_for_exception();
  	//Copy registers the Xtensa HAL did save to gdbstub_savedRegs
	ets_memcpy(&savedRegs, frame, 19*4);

	//Credits go to Cesanta for this trick. A1 seems to be destroyed, but because it
	//has a fixed offset from the address of the passed frame, we can recover it.
	//savedRegs.a1=(uint32_t)frame+EXCEPTION_GDB_SP_OFFSET;
	savedRegs.a1=(luint32_t)frame;

	ets_wdt_disable();
	print_reason();
//	ets_wdt_enable();

	extern void die(void);
	die();
}

void exception_init(void){
	unsigned int i;
  	int exno[]={EXCCAUSE_ILLEGAL, EXCCAUSE_SYSCALL, EXCCAUSE_INSTR_ERROR, EXCCAUSE_LOAD_STORE_ERROR,
		EXCCAUSE_DIVIDE_BY_ZERO, EXCCAUSE_UNALIGNED, EXCCAUSE_INSTR_DATA_ERROR, EXCCAUSE_LOAD_STORE_DATA_ERROR,
		EXCCAUSE_INSTR_ADDR_ERROR, EXCCAUSE_LOAD_STORE_ADDR_ERROR, EXCCAUSE_INSTR_PROHIBITED,
		EXCCAUSE_LOAD_PROHIBITED, EXCCAUSE_STORE_PROHIBITED};
	for (i=0; i<13; i++) { //(sizeof(exno)/sizeof(exno[0]))
		_xtos_set_exception_handler(exno[i], exception_handler);
	}
}
