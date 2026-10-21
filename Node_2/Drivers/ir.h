#ifndef IR_H
#define IR_H

#include <stdint.h>

// Initializes the ADC and calibrates the "beam unblocked" level.
// Keep the beam clear while this runs.
void ir_init(void);

// Call at a regular rate (e.g. every 1 ms). Updates the filter and state.
void ir_update(void);

// Returns 1 if the beam is currently considered blocked, 0 otherwise.
uint8_t ir_beam_blocked(void);

// Returns 1 exactly once each time the beam becomes blocked
// (rising edge of "blocked"), then clears itself.
uint8_t ir_blocked_event(void);

// Filtered ADC value, useful for tuning/printing.
uint16_t ir_filtered_value(void);

#endif
