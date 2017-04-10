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

 #include <embox/unit.h>

 #define FREQUENCY 80*1024*1024

 static struct clock_source this_clock_source;

 static inline unsigned get_ccount(void)
 {
         unsigned r;
         asm volatile ("rsr %0, ccount" : "=r"(r));
         return r;
 }

 static struct clock_source this_clock_source;
 static irq_return_t clock_handler(unsigned int irq_nr, void *data) {
 	clock_tick_handler(irq_nr, data);
 	return IRQ_HANDLED;
 }

 static int esp8266_init(void) {
 	clock_source_register(&this_clock_source);
 	return ENOERR; //irq_attach(TIMER_IRQ, clock_handler, 0, &this_clock_source, "ESP8266 systick timer");
 }

 static int esp8266_config(struct time_dev_conf * conf){
    return 0;
 }

 static struct time_event_device this_event = {
 	.config = esp8266_config,
 	.event_hz = 1000,
 	.irq_nr = 0,
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

 STATIC_IRQ_ATTACH(0, clock_handler, &this_clock_source);
