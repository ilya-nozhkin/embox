#include <drivers/gpio.h>

#include <embox/unit.h>

#include <hal/clock.h>
#include <hal/system.h>

#include <unistd.h>

#include "dht11.h"

#define PIN_NUMBER OPTION_GET(NUMBER,pin_numer)

#define DHT11_TIMEOUT 250

static struct gpio* dht;
static inline uint32_t timestamp(void){
    uint32_t ccount;
    asm volatile ("rsr %0, ccount" : "=r"(ccount));
    return ccount / (SYS_CLOCK / 1000000);
}

static gpio_mask_t wait_another_level(gpio_mask_t current_level){
    uint32_t timeout = DHT11_TIMEOUT;
    while(1){
        gpio_mask_t level = gpio_get_level(dht, 0);
        if(level != current_level)
            return level;

        if(--timeout == 0)
            return 2; // error code
    }
}

EMBOX_UNIT_INIT(dht11_start);
static int dht11_start(void){
    dht = gpio_by_num(PIN_NUMBER);
    gpio_settings(dht, 0, GPIO_MODE_OUTPUT);
    gpio_set_level(dht, 0, 1);

    // Well, it is needed to avoid garbage from sensor (according to documentation)
    // sleep(1);
    return 0;
}

// updates current_level and current_ts
// returns delta
static inline uint32_t handle_change(gpio_mask_t* current_level, uint32_t* current_ts){
    *current_level = wait_another_level(*current_level);
    uint32_t ts = timestamp();
    uint32_t delta = ts - *current_ts;
    *current_ts = ts;

    return delta;
}

struct dht11_response dht11_read_response(void){
    gpio_settings(dht, 0, GPIO_MODE_INPUT);

    struct dht11_response res;

    gpio_mask_t current_level = 1;
    uint32_t current_ts = timestamp();

    for(int i = 0; i < 3; i++){
        current_level = wait_another_level(current_level);

        if(current_level == 2){ // timeout :c
            return res;
        }
    }

    uint32_t result = 0;
    uint8_t checksum = 0;
    current_ts = timestamp();

    for(uint8_t i = 0; i < 64; i++){
        uint32_t delta = handle_change(&current_level, &current_ts);

        if(current_level == 2){ // timeout :c
            return res;
        }

        if(i%2 && delta > 30)
            result |= (1 << (32 - i/2));
    }

    for(uint8_t i = 0; i < 16; i++){
        uint32_t delta = handle_change(&current_level, &current_ts);

        if(current_level == 2){ // timeout :c
            return res;
        }

        if(i%2 && delta > 30)
            checksum |= (1 << (8 - i/2));
    }

    uint32_t ft = result & 0xFF;
    uint32_t it = (result & 0xFF00) >> 8;
    uint32_t frh = (result & 0xFF0000) >> 16;
    uint32_t irh = (result & 0xFF000000) >> 24;

    int ok = (((irh + frh + it + ft) & 0xFF) == checksum);

    res.integral_temp = it;
    res.float_temp = ft;
    res.integral_rh = irh;
    res.float_rh = frh;
    res.ok = ok;

    return res;
}

void dht11_request(void){
    gpio_settings(dht, 0, GPIO_MODE_OUTPUT);
    gpio_set_level(dht, 0, 0);
    usleep(18 * 1000);
}
