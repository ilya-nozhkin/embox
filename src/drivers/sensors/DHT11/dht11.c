#include <drivers/gpio.h>

#include <embox/unit.h>

#include <hal/clock.h>
#include <hal/system.h>

#include <kernel/printk.h>
#include "dht11.h"

static struct gpio* dht;
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

static gpio_mask_t wait_another_level(gpio_mask_t current_level){
    while(1){
        gpio_mask_t level = gpio_get_level(dht, 0);
        if(level != current_level)
            return level;
    }
}

EMBOX_UNIT_INIT(dht11_start);
static int dht11_start(void){
    dht = gpio_by_num(5);
    gpio_settings(dht, 0, GPIO_MODE_OUTPUT);
    gpio_set_level(dht, 0, 1);

    custom_delay(1 * 1000000);
    dht11_request();

    gpio_settings(dht, 0, GPIO_MODE_INPUT);

    gpio_mask_t current_level = 1;
    uint32_t current_ts = timestamp();

    for(int i = 0; i < 2; i++){
        current_level = wait_another_level(current_level);
        //TODO: Check if we receive currect response
    }

    uint32_t result = 0;
    uint8_t checksum = 0;

    for(uint8_t i = 0; i < 64; i++){
        current_level = wait_another_level(current_level);
        uint32_t ts = timestamp();
        uint32_t delta = ts - current_ts;
        current_ts = ts;

        if(i%2 == 0 && delta > 30)
            result |= (1 << (32 - i/2));
    }

    for(uint8_t i = 0; i < 16; i++){
        current_level = wait_another_level(current_level);
        uint32_t ts = timestamp();
        uint32_t delta = ts - current_ts;
        current_ts = ts;

        if(i%2 == 0 && delta > 30)
            checksum |= (1 << (8 - i/2));
    }

    uint32_t ft = result & 0xFF;
    uint32_t it = (result & 0xFF00) >> 8;
    uint32_t frh = (result & 0xFF0000) >> 16;
    uint32_t irh = (result & 0xFF000000) >> 24;

    bool ok = (irh + frh + it + ft) & 0xFF & checksum;

    printk("\nOK: %d Rh: %d . %d T: %d . %d\n", ok, irh, frh, it, ft);

    return 0;
}

void dht11_request(void){
    gpio_settings(dht, 0, GPIO_MODE_OUTPUT);
    gpio_set_level(dht, 0, 0);
    custom_delay(18 * 1000);
}
