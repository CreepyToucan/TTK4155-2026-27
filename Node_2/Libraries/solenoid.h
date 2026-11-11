#ifndef SOLENOID_H
#define SOLENOID_H

#include <stdint.h>
#include "sam.h"

/**
 * @brief Initializes the designated I/O pin to control the solenoid's transistor base
 *        Configures the pin as a standard PIO output and ensures it starts in a LOW state
 */
void solenoid_init(void);

/**
 * @brief Turns on the solenoid for a short duration
 */
void solenoid_shoot(void);

/**
 * @brief Turns off the solenoid
 */
void solenoid_rest(void);

#endif // SOLENOID_H