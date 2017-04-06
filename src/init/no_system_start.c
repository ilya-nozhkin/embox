/**
 * @file
 * @brief Runs start script
 *
 * @date 04.10.11
 * @author Alexander Kalmuk
 */
 
#include <drivers/gpio.h>
#include <kernel/printk.h>
#include <drivers/diag.h>
 
#define FCPU 160000000
 
//#include <cmd/shell.h>

//Be careful! This function is very imprecise
static inline void delay(uint32_t milliseconds) {
	uint32_t nops = ((uint64_t) milliseconds * FCPU) / 30000;
	while (nops > 1) {
		asm volatile ("nop");
		nops--;
	}
}

extern int setup_tty(const char *dev_name);
extern int diag_fd(void);

void blinky_test() {
	//setup_tty("");
	//printk("\nHello! It is simple echo shell. Just type some symbols and you will receive them back:\n");
	
	while (1) {
		char c = diag_getc();
		diag_putc(c);
	}
}

int system_start(void) {
	blinky_test();
	return 0;
}
