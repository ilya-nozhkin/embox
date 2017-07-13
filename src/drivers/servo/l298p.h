/**
 * @file
 * @brief
 *
 * @author  Ilya Nozhkin
 * @date    13.07.2017
 */

#include <stdint.h>

struct l298p;

struct l298p *l298p_start(uint8_t pwma, uint8_t pwmb, uint8_t dira, uint8_t dirb);
void l298p_servo_set(struct l298p *driver, uint8_t servo, int8_t power);
