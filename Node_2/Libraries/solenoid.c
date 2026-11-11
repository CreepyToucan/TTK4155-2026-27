#include "solenoid.h"


#define SOLENOID_PIN (1u << 25)

void solenoid_init(void) {
    
    PIOB->PIO_PER = SOLENOID_PIN; 
    PIOB->PIO_OER = SOLENOID_PIN; 
    PIOB->PIO_CODR = SOLENOID_PIN; 
}

void solenoid_shoot(void) {
    // Set the pin HIGH to turn on the transistor and fire the solenoid
    PIOB->PIO_SODR = SOLENOID_PIN; 
}

void solenoid_rest(void){
      // Set the pin LOW to turn off the transistor and end the pulse
    PIOB->PIO_CODR = SOLENOID_PIN; 
}