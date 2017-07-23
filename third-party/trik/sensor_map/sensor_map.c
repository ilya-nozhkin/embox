#include "sensor_map.h"

#include <drivers/sensors/hc_sr04.h>
#include <drivers/sensors/dht11.h>
#include <drivers/servos/l298p.h>
#include <drivers/sensors/sharp_2y0a21.h>

#include <stdio.h>

struct digital_sensor {
	uint32_t type;
	//uint8_t pin0;
	//uint8_t pin1;
	void* sensor;
};

#define NUM_SENSORS 8

struct digital_sensor digital_sensors[NUM_SENSORS] = {0};
uint32_t analog_sensors[NUM_SENSORS] = {0};

struct l298p *servo_driver = 0;

void clear_sensor_map(void) {
	if (servo_driver) {
		l298p_finish(servo_driver);
		servo_driver = 0;
	}
	
	for (int i = 0; i < NUM_SENSORS; i++) {
		analog_sensors[i] = 0;
	}
	
	for (int i = 0; i < NUM_SENSORS; i++) {
		if (digital_sensors[i].sensor) {
			switch (digital_sensors[i].type) {
				case DIGITAL_SENSOR_HC_SR04:
					hc_sr04_finish(digital_sensors[i].sensor);
					break;
				case DIGITAL_SENSOR_DHT11:
					dht11_finish(digital_sensors[i].sensor);
					break;
			}
		}
		
		digital_sensors[i].sensor = 0;
		digital_sensors[i].type = 0;
	}
}

void set_digital_sensor(uint8_t id, uint32_t type, uint8_t pin0, uint8_t pin1) {
	digital_sensors[id].type = type;
	//digital_sensors[id].pin0 = pin0;
	//digital_sensors[id].pin1 = pin1;

	switch (digital_sensors[id].type) {
		case DIGITAL_SENSOR_HC_SR04:
			digital_sensors[id].sensor = hc_sr04_setup(pin0, pin1);
			break;
		case DIGITAL_SENSOR_DHT11:
			digital_sensors[id].sensor = dht11_setup(pin0, 1);
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
			return hc_sr04_response(digital_sensors[id].sensor);}
		case DIGITAL_SENSOR_DHT11:
			dht11_request(digital_sensors[id].sensor);
			return dht11_read_response(digital_sensors[id].sensor).integral_temp;
	}
}

uint32_t read_analog_sensor(uint8_t id) {
	switch (analog_sensors[id]) {
		case ANALOG_SENSOR_SHARP_2Y0A21:
			return sharp_2y0a21_measure(id);
	}
}

void sensor_map_from_file(const char *filename) {
	char key[16] = {0};
	
	clear_sensor_map();
	
	FILE *file = fopen(filename, "r");
	
	uint8_t processing = 0b111;
	
	while (processing) {
		fscanf(file, "%s", key);
		
		if (strcmp(key, "servos") == 0) {
			uint32_t pwma = 0;
			uint32_t pwmb = 0;
			uint32_t dira = 0;
			uint32_t dirb = 0;
			
			fscanf(file, "%d%d%d%d", &pwma, &pwmb, &dira, &dirb);
			servo_init(pwma, pwmb, dira, dirb);
			
			processing &= ~0b001;
		} else if (strcmp(key, "digital") == 0) {
			uint32_t sensors = 0;
			fscanf(file, "%d", &sensors);
			
			for (uint32_t i = 0; i < sensors; i++) {
				uint32_t id = 0;
				uint32_t type = 0;
				uint32_t pin0 = 0;
				uint32_t pin1 = 0;
				
				fscanf(file, "%d%d%d%d", &id, &type, &pin0, &pin1);
				set_digital_sensor(id, type, pin0, pin1);
			}
			
			processing &= ~0b010;
		} else if (strcmp(key, "analog") == 0) {
			uint32_t sensors = 0;
			fscanf(file, "%d", &sensors);
			
			for (uint32_t i = 0; i < sensors; i++) {
				uint32_t id = 0;
				uint32_t type = 0;
				
				fscanf(file, "%d%d", &id, &type);
				set_analog_sensor(id, type);
			}
			
			processing &= ~0b100;
		}
	}
	
	fclose(file);
}
