#ifndef DRIVERS_SENSORS_DHT11_H_
#define DRIVERS_SENSORS_DHT11_H_

struct dht11_response {
    int integral_temp;
    int float_temp;
    int integral_rh;
    int float_rh;
    int ok;
};

void dht11_request(void);
struct dht11_response dht11_read_response();

#endif /* DRIVERS_SENSORS_DHT11_H_ */
