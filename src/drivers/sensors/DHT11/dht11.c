/**
 * @file
 * @brief   DHT11 Temp and RH sensor implementation
 *
 * @author  Egor Shitov
 * @date    04.07.2017
 */


#include "dht11.h"
#include <stdlib.h>
#include <hal/clock.h>
#include <hal/system.h>

#include <drivers/gpio.h>

#define DHT11_TIMEOUT 250

//static struct gpio* dht;
static inline uint32_t timestamp(void){
    uint32_t ccount;
    asm volatile ("rsr %0, ccount" : "=r"(ccount));
    return ccount / (SYS_CLOCK / 1000000);
}

static gpio_mask_t wait_another_level(struct gpio *dht, gpio_mask_t current_level){
    uint32_t timeout = DHT11_TIMEOUT;
    while(1){
        gpio_mask_t level = gpio_get_level(dht, 0);
        if(level != current_level)
            return level;

        if(--timeout == 0)
            return 2; // error code
    }
}

struct dht11 *dht11_setup(uint8_t pin_number, uint8_t wait){
    struct gpio *dht = gpio_by_num(pin_number);
	struct dht11 *sensor = malloc(sizeof(struct dht11));
	sensor->dht = dht;
    gpio_settings(dht, 0, GPIO_MODE_OUTPUT);
    gpio_set_level(dht, 0, 1);

    // Well, it is needed to avoid garbage from sensor (according to documentation)
    if(wait)
        sleep(1);
	return sensor;
}

// updates current_level and current_ts
// returns delta
static inline uint32_t handle_change(struct gpio *dht, gpio_mask_t* current_level, uint32_t* current_ts){
    *current_level = wait_another_level(dht, *current_level);
    uint32_t ts = timestamp();
    uint32_t delta = ts - *current_ts;
    *current_ts = ts;

    return delta;
}

struct dht11_response dht11_read_response(struct dht11 *sensor){
	struct gpio *dht = sensor->dht;
	gpio_settings(dht, 0, GPIO_MODE_INPUT);

    struct dht11_response res;

    gpio_mask_t current_level = 1;
    uint32_t current_ts = timestamp();

    for(int i = 0; i < 3; i++){
        current_level = wait_another_level(dht, current_level);

        if(current_level == 2){ // timeout :c
            return res;
        }
    }

    uint32_t result = 0;
    uint8_t checksum = 0;
    current_ts = timestamp();

    for(uint8_t i = 0; i < 64; i++){
        uint32_t delta = handle_change(dht, &current_level, &current_ts);

        if(current_level == 2){ // timeout :c
            return res;
        }

        if(i%2 && delta > 30)
            result |= (1 << (32 - i/2));
    }

    for(uint8_t i = 0; i < 16; i++){
        uint32_t delta = handle_change(dht, &current_level, &current_ts);

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

void dht11_request(struct dht11 *sensor){
	struct gpio *dht = sensor->dht;
    gpio_settings(dht, 0, GPIO_MODE_OUTPUT);
    gpio_set_level(dht, 0, 0);
    usleep(18 * 1000);
}
