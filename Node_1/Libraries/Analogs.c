#include <avr/io.h>
#include <stdint.h>
#include "Analogs.h"
#include "ADC.h"

// Calibration data in RAW 8-bit ADC counts. The defaults are the values that
// were hard-coded before, so the joystick works without calibrating.
uint8_t min_joy_x    = 71;
uint8_t center_joy_x = 163;
uint8_t max_joy_x    = 245;
uint8_t min_joy_y    = 71;
uint8_t center_joy_y = 159;
uint8_t max_joy_y    = 245;

#define JOY_DEADZONE 3   // raw counts around the center that count as 0

uint8_t getRawJOY_X(void) { return ADC_ReadChannel(1); }
uint8_t getRawJOY_Y(void) { return ADC_ReadChannel(0); }

// Maps a raw reading to -100..100 using the calibrated min/center/max.
// The two halves are scaled separately because the center is usually not
// exactly halfway between min and max.
static int8_t scale_axis(uint8_t raw, uint8_t min, uint8_t center, uint8_t max) {
    int16_t diff = (int16_t)raw - center;
    int16_t out;

    if (diff >= -JOY_DEADZONE && diff <= JOY_DEADZONE) return 0;

    if (diff > 0) {
        if (max <= center) return 0;
        out = (diff * 100) / (max - center);
    } else {
        if (center <= min) return 0;
        out = (diff * 100) / (center - min);   // diff < 0, so out < 0
    }

    if (out > 100)  out = 100;
    if (out < -100) out = -100;
    return (int8_t)out;
}

int8_t HandleJOY_X(void) {
    return scale_axis((uint8_t)ADC_ReadChannel(1), min_joy_x, center_joy_x, max_joy_x);
}

int8_t HandleJOY_Y(void) {
    return scale_axis((uint8_t)ADC_ReadChannel(0), min_joy_y, center_joy_y, max_joy_y);
}

uint8_t HandlePad_Y(void) { 
    return ADC_ReadChannel(2);
}

uint8_t HandlePad_X(void) { 
    return ADC_ReadChannel(3); 
}

int8_t getPos(InputType obj) {
    switch(obj) {
        case Joy_y:
            return HandleJOY_Y();
        case Joy_x:
            return HandleJOY_X();
        default:
            return -1;
    }
}

char getDir(InputType obj) {
    switch (obj) {
        case Joy_y: {
            int8_t v = HandleJOY_Y();   // read once, not once per comparison
            if (v > 2)  return 'U';
            if (v < -2) return 'D';
            return 'N';
        }
        case Joy_x: {
            int8_t v = HandleJOY_X();
            if (v > 2)  return 'R';
            if (v < -2) return 'L';
            return 'N';
        }
        default: return 'X';
    }
}

void initButton(void){
    DDRD &= ~(1 << DDD4);
    PORTD |= (1 << PORTD4);
}

uint8_t getButton(void){
    if (!(PIND & (1 << PIND4))) {
        return 1; // Button is pressed
    } else {
        return 0; // Button is unpressed
    }
}
