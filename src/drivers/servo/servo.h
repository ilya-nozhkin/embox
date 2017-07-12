/**
 * @file
 * @brief
 *
 * @author  Ilya Nozhkin
 * @date    29.06.2017
 */

#ifndef DRIVERS_SERVO_H_
#define DRIVERS_SERVO_H_

#include <stdint.h>

#include <module/embox/driver/servo/api.h>

#define DIRECTION_BACKWARD 0
#define DIRECTION_FORWARD 1

struct servo;

extern struct servo *servo_by_id(uint32_t motor_id);
extern void servo_enable(struct servo *motor);
extern void servo_disable(struct servo *motor);
extern void servo_set_direction(struct servo *motor, uint32_t direction);

#endif /* DRIVERS_SERVO_H_ */
