/**
 * @file
 * @brief
 *
 * @author  Ilya Nozhkin
 * @date    29.06.2017
 */

struct pca9685;

struct pca9685 *pca9685_start(struct i2c_bus *bus, uint8_t address);
void pca9682_servo_set(struct pca9685 *driver, uint8_t servo, uint32_t power);
