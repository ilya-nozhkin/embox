#include "sensor_map.h"

#include <drivers/sensors/hc_sr04.h>
#include <drivers/sensors/dht11.h>

struct digital_sensor {
	uint32_t type;
	uint8_t pin0;
	uint8_t pin1;
};

struct digital_sensor digital_sensors[8] = {0};
uint32_t analog_sensors[8] = {0};

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

void servo_settings(uint8_t id, uint32_t speed) {
	struct servo *motor = servo_by_id(id);
	
	if (speed != 0) {
		servo_enable(motor);
		servo_set_direction(motor, speed > 0 ? 1 : 0);
	} else {
		servo_disable(motor);
	}
}

uint32_t read_digital_sensor(uint8_t id) {
	switch (digital_sensors[id].type) {
		case DIGITAL_SENSOR_HC_SR04: {
			usleep(100000);
			hc_sr04_request();
			uint32_t resp = hc_sr04_response();
			return resp;}
		case DIGITAL_SENSOR_DHT11:
			dht11_request();
			return dht11_read_response().integral_temp;
	}
}

uint32_t read_analog_sensor(uint8_t id) {
	
}
