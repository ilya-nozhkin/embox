#ifndef HAL_ARCH_H_
# error "Do not include this file directly!"
#endif /* HAL_ARCH_H_ */

#ifndef __BYTE_ORDER__
#define __BYTE_ORDER __LITTLE_ENDIAN
#define BYTE_ORDER    __BYTE_ORDER
#endif

#include <stdint.h>

#define PLL_FREQ 80000000UL

#define __PLATFORM_ARCH "xtensa"

void arch_delay(uint32_t useconds);
