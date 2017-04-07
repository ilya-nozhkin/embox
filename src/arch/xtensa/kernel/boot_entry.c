#include <inttypes.h>

extern void kernel_start();

extern uint32_t _bss_start;
extern uint32_t _bss_end;

void zerobss(void)
{
	uint32_t *addr = &_bss_start;
	for (addr = &_bss_start; addr < &_bss_end; addr++)  *addr = 0; //Safe, _bss_start doesn't have to == _bss_end
}

extern void SPIUnlock(void);
extern void Cache_Read_Enable(uint32_t, uint32_t, uint32_t);

#define GPIO0_MUX_alt (*((volatile uint32_t*) 0x60000834))
#define GPIO_MUX_CFG_alt (*((volatile uint32_t*) 0x60000800))
#define SPI0_USER (*((volatile uint32_t*) 0x6000021C))
#define SPI0_CTRL (*((volatile uint32_t*) 0x60000208))

void __attribute__((section(".spec.call_user_start"))) call_user_start(void) {
	GPIO0_MUX_alt = 1 << 7;
	SPI0_USER |= 1 << 5;
	GPIO_MUX_CFG_alt &= ~(1 << 8);
	SPI0_CTRL = (SPI0_CTRL & (~0x1FFF)) | 0x0101;
	Cache_Read_Enable(0, 0, 1);
	
	zerobss();
	kernel_start();
}


void die(void) {
	while (1) {
	}
}
