#ifndef _SENSOR_MAP_H
#define _SENSOR_MAP_H

#include <stdint.h>

#define DIGITAL_SENSOR_HC_SR04    1
#define DIGITAL_SENSOR_DHT11      2

#define ANALOG_SENSOR_SHARP_2Y0A21 1

void clear_sensor_map(void);

void set_digital_sensor(uint8_t id, uint32_t type, uint8_t pin0, uint8_t pin1);
void set_analog_sensor(uint8_t id, uint32_t type);

void servo_init(uint8_t pwma, uint8_t pwmb, uint8_t dira, uint8_t dirb);

void servo_settings(uint8_t id, int32_t speed);
uint32_t read_digital_sensor(uint8_t id);
uint32_t read_analog_sensor(uint8_t id);

#endif //_SENSOR_MAP_H
