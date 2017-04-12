/**
 * @file
 *
 * @brief ESP8266 built-in timer
 *
 * @date 10.04.2017
 * @author Egor Shitov
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

#define FREQUENCY 80*1000*1000
#define IRQ_NR 8//OPTION_GET(NUMBER, irq_num)

typedef void ETSTimerFunc(void *timer_arg);

typedef struct _ETSTIMER_ {
	struct _ETSTIMER_    *timer_next;
    uint32_t              timer_expire;
    uint32_t              timer_period;
    ETSTimerFunc         *timer_func;
    void                 *timer_arg;
} ETSTimer;

extern void ets_timer_arm(ETSTimer*, uint32_t, bool);
extern void ets_timer_setfn(ETSTimer*, ETSTimerFunc*, void*);
extern void ets_timer_disarm(ETSTimer*);

static ETSTimer timer;

static inline unsigned get_ccount(void)
{
	// Just check special registers
	unsigned interrupt;
	READ_SREG("interrupt", interrupt);
	printk("interrupt: 0x%X\n", interrupt);

	unsigned intenable;
	READ_SREG("intenable", intenable);
	printk("intenable: 0x%x\n", intenable);
	// ============================

	unsigned r;
	READ_SREG("ccount", r);
	return r;
}

static struct clock_source this_clock_source;
static irq_return_t clock_handler(unsigned int irq_nr, void *data) {
	clock_tick_handler(irq_nr, data);
	return IRQ_HANDLED;
}

// Put it in iram
static INTERRUPT_FUNC void event_handler(void* arg){
 	printk("event_handler\n"); // Never triggered 
}

static int esp8266_init(void) {
 	// printk("irq_nr: %d\n", IRQ_NR);
	// set_irq_handler(IRQ_NR, event_handler, NULL);
	// WRITE_SREG("ccompare0", 10);
	// unsigned prev;
	// INTLEVEL_SET(prev, 0);
	// printk("prev: 0x%X\n", prev);
	// WRITE_SREG("intenable", 1 << IRQ_NR);
	// INTLEVEL_RESTORE(prev);

	ets_timer_disarm(&timer);
	ets_timer_setfn(&timer, event_handler, NULL);
	ets_timer_arm(&timer, 1000, 1);

	clock_source_register(&this_clock_source);
	return irq_attach(IRQ_NR, clock_handler, 0, &this_clock_source, "ESP8266 systick timer");
}

static int esp8266_config(struct time_dev_conf * conf){
	return 0;
}

static struct time_event_device this_event = {
	.config = esp8266_config,
	.event_hz = 1000,
	.irq_nr = IRQ_NR
};

static struct time_counter_device this_counter = {
	.read = get_ccount,
	.cycle_hz = FREQUENCY
};

static struct clock_source this_clock_source = {
	.name = "system_tick",
	.event_device = &this_event,
	.counter_device = &this_counter,
	.read = clock_source_read,
};

EMBOX_UNIT_INIT(esp8266_init);

STATIC_IRQ_ATTACH(IRQ_NR, clock_handler, &this_clock_source);
