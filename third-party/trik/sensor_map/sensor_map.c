#include "sensor_map.h"

#include <drivers/sensors/hc_sr04.h>
#include <drivers/sensors/dht11.h>
#include <drivers/servos/l298p.h>
#include <drivers/sensors/sharp_2y0a21.h>

struct digital_sensor {
	uint32_t type;
	uint8_t pin0;
	uint8_t pin1;
};

struct digital_sensor digital_sensors[8] = {0};
uint32_t analog_sensors[8] = {0};

struct l298p *servo_driver = 0;

void set_digital_sensor(uint8_t id, uint32_t type, uint8_t pin0, uint8_t pin1) {
	digital_sensors[id].type = type;
	digital_sensors[id].pin0 = pin0;
	digital_sensors[id].pin1 = pin1;
	
	switch (digital_sensors[id].type) {
		case DIGITAL_SENSOR_HC_SR04:
			hc_sr04_setup(pin0, pin1);
			break;
		case DIGITAL_SENSOR_DHT11:
			dht11_setup(pin0, 1);
			break;
	}
}

void set_analog_sensor(uint8_t id, uint32_t type) {
	analog_sensors[id] = type;
}

void servo_init(uint8_t pwma, uint8_t pwmb, uint8_t dira, uint8_t dirb) {
	servo_driver = l298p_start(pwma, pwmb, dira, dirb);
}

void servo_settings(uint8_t id, int32_t speed) {
	l298p_servo_set(servo_driver, id, speed);
}

uint32_t read_digital_sensor(uint8_t id) {
	switch (digital_sensors[id].type) {
		case DIGITAL_SENSOR_HC_SR04: {
			hc_sr04_request();
			return hc_sr04_response();}
		case DIGITAL_SENSOR_DHT11:
			dht11_request();
			return dht11_read_response().integral_temp;
	}
}

uint32_t read_analog_sensor(uint8_t id) {
	switch (analog_sensors[id]) {
		case ANALOG_SENSOR_SHARP_2Y0A21:
			return sharp_2y0a21_measure(id);
	}
}
