#include <drivers/gpio.h>

#include <embox/unit.h>

#include <hal/clock.h>
#include <hal/system.h>

#include <kernel/printk.h>

static uint32_t timestamp(void){
    uint32_t ccount;
    asm volatile ("rsr %0, ccount" : "=r"(ccount));
    return ccount / (SYS_CLOCK / 1000000);
}

// us
static void custom_delay(uint32_t usec){
    uint32_t mark = timestamp();

    while(1){
        uint32_t current = timestamp();
        if(current - mark >= usec)
            return;
    }
}

EMBOX_UNIT_INIT(dht11_start);
static int dht11_start(void){
    struct gpio* dht = gpio_by_num(3);
    gpio_settings(dht, 0, GPIO_MODE_OUTPUT);
    gpio_set_level(dht, 0, 1);

    custom_delay(1 * 1000000);
    gpio_set_level(dht, 0, 0);
    custom_delay(18 * 1000);

    gpio_settings(dht, 0, GPIO_MODE_INPUT);

    gpio_mask_t current_level = 1;
    uint32_t counter = 0;

    while(1){
        gpio_mask_t level = gpio_get_level(dht, 0);
        if(level != current_level){
            current_level = level;
            counter++;
        }

        if(counter >= 10)
            break;
    }

    printk("Counter: %d\n", counter);

    return 0;
}

void dht11_request(void){
}
