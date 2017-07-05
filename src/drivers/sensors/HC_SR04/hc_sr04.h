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

void sensor_setup(uint8_t trig_pin, uint8_t echo_pin);
void sensor_request(void);
uint32_t sensor_response(void);

#endif /*DRIVERS_SENSORS_HC_SR04_H_ */
