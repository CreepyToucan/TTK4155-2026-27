#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>
#include "../Drivers/pwm.h"

uint8_t servoSetPosition(pwm_pin_config_t* config, int8_t val);

#endif // SERVO_H