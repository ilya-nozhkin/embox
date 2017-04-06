#include <inttypes.h>

extern void kernel_start();

extern uint32_t _bss_start;
extern uint32_t _bss_end;

void zerobss(void)
{
	uint32_t *addr = &_bss_start;
	for (addr = &_bss_start; addr < &_bss_end; addr++)  *addr = 0; //Safe, _bss_start doesn't have to == _bss_end
}

void call_user_start(void) {
	zerobss();
	kernel_start();
}


void die(void) {
	while (1) {
	}
}
