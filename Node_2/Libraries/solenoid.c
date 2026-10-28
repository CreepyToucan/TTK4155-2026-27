#include "solenoid.h"

// Digital pin D2 is physically connected to Port B, Pin 25
#define SOLENOID_PIN (1u << 25)

void solenoid_init(void) {
    // Enable the PIO controller on Port B, Pin 25
    PIOB->PIO_PER = SOLENOID_PIN; 

    // Configure the pin to act as a driven output
    PIOB->PIO_OER = SOLENOID_PIN; 

    // Set the output LOW to ensure the solenoid starts safely OFF
    PIOB->PIO_CODR = SOLENOID_PIN; 
}

void solenoid_shoot(void) {
    // Set the pin HIGH to turn on the transistor and fire the solenoid
    PIOB->PIO_SODR = SOLENOID_PIN; 

    // Wait for a short duration
    // Tune this loop count so the pulse is just long enough to hit the ball
    for (volatile uint32_t i = 0; i < 100000; i++) {
        // Block to hold the pulse active
    }

    // Set the pin LOW to turn off the transistor and end the pulse
    PIOB->PIO_CODR = SOLENOID_PIN; 
}