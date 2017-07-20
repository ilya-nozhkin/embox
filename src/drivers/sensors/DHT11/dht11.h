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

struct dht11{
	struct gpio *dht;
};

void dht11_request(struct dht11 *sensor);
struct dht11 *dht11_setup(uint8_t pin_number, uint8_t wait);
struct dht11_response dht11_read_response(struct dht11 *sensor);

#endif /* DRIVERS_SENSORS_DHT11_H_ */
