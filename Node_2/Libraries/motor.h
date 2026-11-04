#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>

/**
 * @brief Direction constants for the A3959 motor driver
 */
typedef enum {
    MOTOR_DIR_FORWARD = 0,
    MOTOR_DIR_REVERSE = 1
} motor_dir_t;

/**
 * @brief Initializes the motor driver pins and PWM output
 *        Configures PB12 for PWM ENABLE and PC23 for PHASE
 */
void motor_init(void);

/**
 * @brief Sets the motor speed and direction.
 * 
 * @param speed Speed percentage (0 to 100)
 * @param dir Motor direction (MOTOR_DIR_FORWARD or MOTOR_DIR_REVERSE)
 */
void motor_set_speed(uint8_t speed, motor_dir_t dir);

/**
 * @brief Stops the motor by setting the speed to 0
 */
void motor_stop(void);

#endif // MOTOR_H