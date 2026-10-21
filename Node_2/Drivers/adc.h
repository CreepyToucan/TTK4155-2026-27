#ifndef ADC_H
#define ADC_H

#include <stdint.h>

// Initializes the ADC on channel 7 (pin A0)).
void adc_init(void);

// Returns the latest 12-bit conversion result (0-4095) for the pin A0
uint16_t adc_read_A0(void);

#endif
