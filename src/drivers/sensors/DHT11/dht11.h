/**
 * @file
 * @brief   DHT11 Temp and RH sensor implementation
 *
 * @author  Egor Shitov
 * @date    04.07.2017
 */

#ifndef DRIVERS_SENSORS_DHT11_H_
#define DRIVERS_SENSORS_DHT11_H_

#include <unistd.h>
#include <stdint.h>

struct dht11_response {
    int integral_temp;
    int float_temp;
    int integral_rh;
    int float_rh;
    int ok;
};

void dht11_setup(uint8_t pin_number, uint8_t wait);
void dht11_request(void);
struct dht11_response dht11_read_response(void);

#endif /* DRIVERS_SENSORS_DHT11_H_ */
