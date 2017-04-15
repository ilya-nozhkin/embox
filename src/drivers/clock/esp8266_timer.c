/**
 * @file
 *
 * @brief ESP8266 built-in timer
 *
 * @date 04.06.2017
 * @author Ilya Nozhkin
 */

#include <errno.h>

#include <hal/clock.h>
#include <hal/reg.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>
#include <drivers/irqctrl.h>
#include <kernel/printk.h>
#include <asm/hal/sreg.h>

#include <embox/unit.h>

struct frc1_load {
	uint32_t frc1_load_value : 23;
	uint32_t reserved : 9;
};

struct frc1_count {
	uint32_t frc1_count : 23;
	uint32_t reserved : 9;
};

struct frc1_ctrl {
	uint32_t frc1_ctrl_int_type : 1;
	uint32_t frc1_ctrl_reserved0 : 1;
	uint32_t frc1_ctrl_divisor : 2;
	uint32_t frc1_ctrl_reserved1 : 2;
	uint32_t frc1_ctrl_autoload : 1;
	uint32_t frc1_ctrl_enable : 1;
	uint32_t frc1_int : 1;
	uint32_t reserved : 23;
};

struct frc1_int {
	uint32_t frc1_int_clr_mask : 1;
	uint32_t reserved : 31;
};

#define IRQ_NR 9

#define TM1_EDGE_INT (*((volatile uint32_t*) 0x3ff00004))

#define FRC1_LOAD ((volatile struct frc1_load*) 0x60000600)
#define FRC1_COUNT ((volatile struct frc1_count*) 0x60000604)
#define FRC1_CTRL ((volatile struct frc1_ctrl*) 0x60000608)
#define FRC1_INT ((volatile struct frc1_int*) 0x6000060C)

#define CLOCK_FREQUENCY 52*1000000
#define EDGE_FREQUENCY OPTION_GET(NUMBER, frequency)

static struct clock_source this_clock_source;

extern void ets_intr_unlock(void);
extern void ets_isr_attach(uint32_t irq_nr, void (*handler)(void*), void *data);

/*static irq_return_t clock_handler(unsigned int irq_nr, void *data) {
	clock_tick_handler(irq_nr, data);
	return IRQ_HANDLED;
}*/

static int int_flag = 0;

static void event_handler(void* arg) {
	if (!int_flag) {
		int_flag = 1;
		irqctrl_disable(IRQ_NR);
		clock_tick_handler(IRQ_NR, &this_clock_source);
		irqctrl_enable(IRQ_NR);
		int_flag = 0;
	}
}

static int this_init(void) {
	ets_intr_unlock();
	
	FRC1_CTRL->frc1_ctrl_enable = 1;
	FRC1_CTRL->frc1_ctrl_int_type = 0;
	FRC1_CTRL->frc1_ctrl_divisor = 0;
	FRC1_CTRL->frc1_ctrl_autoload = 1;
	
	FRC1_LOAD->frc1_load_value = CLOCK_FREQUENCY / EDGE_FREQUENCY;
	clock_source_register(&this_clock_source);
	
	ets_isr_attach(IRQ_NR, &event_handler, NULL);
	irqctrl_enable(IRQ_NR);
	
	TM1_EDGE_INT |= 2;
	
 	return 0;
}

static int this_config(struct time_dev_conf * conf){
	return 0;
}

static cycle_t this_read(void)
{
	return 0;
	//return FRC1_COUNT->frc1_count;
}

static struct time_event_device this_event = {
	.config = this_config,
	.event_hz = EDGE_FREQUENCY,
	.irq_nr = IRQ_NR
};

static struct time_counter_device this_counter = {
	.read = this_read,
	.cycle_hz = EDGE_FREQUENCY
};

static struct clock_source this_clock_source = {
	.name = "system_tick",
	.event_device = &this_event,
	.counter_device = &this_counter,
	.read = clock_source_read,
};

EMBOX_UNIT_INIT(this_init);

//STATIC_IRQ_ATTACH(IRQ_NR, clock_handler, &this_clock_source);
