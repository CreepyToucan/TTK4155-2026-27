#ifndef PWM_H
#define PWM_H

#include <stdint.h>
#include "sam.h"

// Hardware Mapping Structure
typedef struct {
    Pio* pio_port;        // Pointer to the PIO controller (e.g., PIOC)
    uint32_t pin_mask;    // Bitmask for the pin (e.g., PIO_PDR_P22)
    uint32_t peripheral;  // 0 for Peripheral A, 1 for Peripheral B
    uint32_t channel;     // PWM Channel number (0-7)
    uint32_t pio_id;      // Peripheral ID for the PIO port (e.g., ID_PIOC)
} pwm_pin_config_t;

// Pre-defined Arduino Due PWM pins
// Declared as extern so they can be defined once in the .c file and used anywhere
extern const pwm_pin_config_t DUE_PIN_6;
extern const pwm_pin_config_t DUE_PIN_7;
extern const pwm_pin_config_t DUE_PIN_8;
extern const pwm_pin_config_t DUE_PIN_9;
extern const pwm_pin_config_t DUE_PIN_21;

// Function Prototypes

/**
 * @brief Initializes a specific PWM pin with a target frequency.
 * 
 * @param config Pointer to the pin configuration structure.
 * @param frequency Desired PWM frequency in Hz.
 */
void pwm_init(const pwm_pin_config_t* config, uint32_t frequency);

/**
 * @brief Sets the duty cycle for a specific PWM pin.
 * 
 * @param config Pointer to the pin configuration structure.
 * @param duty_percent Desired duty cycle percentage (0-100).
 */
void pwm_set_duty(const pwm_pin_config_t* config, uint8_t duty_percent);

void pwm_set_pulse_us(const pwm_pin_config_t* config, uint32_t pulse_us);

#endif // PWM_H