#ifndef SOLENOID_H
#define SOLENOID_H

#include <stdint.h>
#include "sam.h"

/**
 * @brief Initializes the designated I/O pin to control the solenoid's transistor base.
 *        Configures the pin as a standard PIO output and ensures it starts in a LOW state.
 */
void solenoid_init(void);

/**
 * @brief Generates a short hardware pulse to actuate the solenoid.
 *        The pulse duration is deliberately kept short to prevent the electromagnet
 *        from overheating and being damaged.
 */
void solenoid_shoot(void);

#endif // SOLENOID_H