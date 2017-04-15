/**
 * @file
 * @brief
 *
 * @date 22.03.2017
 * @author Ilya Nozhkin
 */
 
int __attribute__((section(".iram0.text"))) __sync_bool_compare_and_swap_4(unsigned long *addr, unsigned long old, unsigned long new) {
	if (*addr != old) {
		return 0;
	}
	*addr = new;
	return 1;
}

void arch_init(void) {

}

void arch_idle(void) {

}

void arch_shutdown(void) {
	while (1);
}



