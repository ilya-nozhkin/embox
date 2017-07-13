/**
 * @file
 * @brief   HC-SR04 Ultrasonic
sensor implementation
 *
 * @author  Denis Kuznetsov
 * @date    05.07.2017
 */

#ifndef DRIVERS_SENSORS_HC_SR04_H_
#define DRIVERS_SENSORS_HC_SR04_H_

#include <unistd.h>
#include <stdint.h>

void hc_sr04_setup(uint8_t trig_pin, uint8_t echo_pin);
void hc_sr04_request(void);
uint32_t hc_sr04_response(void);

#endif /*DRIVERS_SENSORS_HC_SR04_H_ */
