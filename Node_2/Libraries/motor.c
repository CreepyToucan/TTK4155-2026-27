#include "motor.h"
#include "../Drivers/pwm.h"
#include "sam.h"
#include "time.h"
#include "../Libraries/encoder.h"

// The ENABLE/PWM pin is connected to Arduino pin D20 / ATSAM3X8E pin PB12
const pwm_pin_config_t MOTOR_ENABLE_PIN = { PIOB, PIO_PB12, 1, 0, ID_PIOB };

void motor_init(void) {
    
    // 1. Initialize the ENABLE/PWM pin.
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



/*
 We used Gemini AI to help us implement a robust calibration routine.
*/
void calibrate_encoder_to_left_stop(void) {
    // Start moving left
    motor_set_speed(CALIBRATION_SPEED, MOTOR_DIR_FORWARD);

    int32_t last_position = encoder_read();
    uint64_t stall_start_time = time_now(); 

    // Loop until a physical stall is detected
    while (1) {
        // Delay before taking the next reading using CPU spin
        time_spinFor(msecs(CHECK_INTERVAL_MS));
        
        int32_t current_position = encoder_read();
        int32_t delta = abs(current_position - last_position);

        if (delta > STALL_TOLERANCE_TICKS) {
            // reset the stall timer
            stall_start_time = time_now(); 
        } else {
            // The motor hasn't moved significantly
            // Check if it has been stalled long enough to confirm we hit the end
            if ((time_now() - stall_start_time) >= msecs(STALL_DURATION_MS)) { 
                break; // Hard stop confirmed
            }
        }
        
        last_position = current_position;
    }

    // Stop the motor
    motor_stop();

    // Wait a brief moment to allow mechanical vibrations/rebound to settle
    time_spinFor(msecs(200));

    // Reset the hardware encoder counter to 0 at this leftmost position
    encoder_reset();
}
