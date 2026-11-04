#include "motor.h"
#include "../Drivers/pwm.h"
#include "sam.h"

// The ENABLE/PWM pin is connected to Arduino pin D20 / ATSAM3X8E pin PB12
// PWM out on PB12 uses alternate function B (Peripheral B) on PIOB, PWM channel 0
const pwm_pin_config_t MOTOR_ENABLE_PIN = { PIOB, PIO_PB12, 1, 0, ID_PIOB };

void motor_init(void) {
    // Initialize the ENABLE/PWM pin
    // 20 kHz frequency to avoid audible switching noise
    pwm_init(&MOTOR_ENABLE_PIN, 20000); 

    // Initialize the PHASE/DIR pin (Arduino D7 / ATSAM3X8E PC23)
    // Enable the clock for PIOC
    PMC->PMC_PCER0 = (1 << ID_PIOC);
    
    // Enable PIO control on PC23
    PIOC->PIO_PER = PIO_PC23;
    
    // Set PC23 as an output
    PIOC->PIO_OER = PIO_PC23;
    
    // Set initial direction to forward (Low)
    PIOC->PIO_CODR = PIO_PC23; 
}

void motor_set_speed(uint8_t speed, motor_dir_t dir) {
    // Set the direction (PHASE pin).
    if (dir == MOTOR_DIR_FORWARD) {
        PIOC->PIO_CODR = PIO_PC23; // Set PC23 Low
    } else {
        PIOC->PIO_SODR = PIO_PC23; // Set PC23 High
    }

    // Set the speed (ENABLE pin duty cycle)
    // When ENABLE is high, the selected sink-source pair turns on
    // The provided pwm_set_duty function clamps the speed to 100%
    pwm_set_duty(&MOTOR_ENABLE_PIN, speed);
}

void motor_stop(void) {
    // Setting duty cycle to 0 stops the motor by keeping ENABLE low
    pwm_set_duty(&MOTOR_ENABLE_PIN, 0);
}