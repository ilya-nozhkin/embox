/**
 * @file
 * @brief ESP8266 Timer.
 * @author Ilya Nozhkin
 * @version 0.1
 * @date 2017-04-06
 */

#include <hal/clock.h>
#include <hal/reg.h>
#include <hal/system.h>
#include <kernel/printk.h>
#include <kernel/irq.h>
#include <kernel/time/clock_source.h>
#include <embox/unit.h>

static struct clock_source this_clock_source;
static irq_return_t clock_handler(unsigned int irq_nr, void *data) {
	return 0;
}

static int this_init(void) {
	clock_source_register(&this_clock_source);
	return 0;
}

static int this_config(struct time_dev_conf * conf) {
	return 0;
}

static cycle_t this_read(void) {
	return 0;
}

static struct time_event_device this_event = {
	.config = this_config,
	.event_hz = 1000,
	.irq_nr = 0,
};

static struct time_counter_device this_counter = {
	.read = this_read,
	.cycle_hz = 1000,
};

static struct clock_source this_clock_source = {
	.name = "system_tick",
	.event_device = &this_event,
	.counter_device = &this_counter,
	.read = clock_source_read,
};

EMBOX_UNIT_INIT(this_init);
