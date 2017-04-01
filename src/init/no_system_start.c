/**
 * @file
 * @brief Runs start script
 *
 * @date 04.10.11
 * @author Alexander Kalmuk
 */
 
 #include <drivers/gpio.h>
 
 #define FCPU 160000000

//Be careful! This function is very imprecise
static inline void delay(uint32_t milliseconds) {
	uint32_t nops = ((uint64_t) milliseconds * FCPU) / 30000;
	while (nops > 1) {
		asm volatile ("nop");
		nops--;
	}
}

void blinky_test() {
	gpio_settings(gpio_by_num(1), 0, GPIO_MODE_OUTPUT);
	
	int status = 1;
	while (1) {
		delay(1000);
		if (status) {
			gpio_set_level(gpio_by_num(1), 0, 0);
			status = 0;
		} else {
			gpio_set_level(gpio_by_num(1), 0, 1);
			status = 1;
		}
	}
}

int system_start(void) {
	blinky_test();
	return 0;
}
