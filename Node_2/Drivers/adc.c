#include "sam.h"
#include "adc.h"

void adc_init(void) {
    // Enable the peripheral clock for the ADC
    // The ADC Peripheral ID is 37. IDs 32-63 are enabled via PMC_PCER1
    PMC->PMC_PCER1 = (1 << (ID_ADC - 32));

    // Reset and configure the ADC Mode Register (ADC_MR)
    // Clear the register, then set the clock prescaler (PRESCAL)
    // The ADC clock must be between 1 MHz and 20 MHz
    // With 84 MHz master clock, a prescaler of 7 gives a 5.25 MHz clock
    ADC->ADC_MR = ADC_MR_PRESCAL(7); 

    // Enable ADC Channel 7 (Physical pin A0 on the Arduino Due)
    ADC->ADC_CHER = ADC_CHER_CH7; 
}


uint16_t adc_read_A0(void) {
    // Start the conversion cycle via the Control Register (ADC_CR)
    ADC->ADC_CR = ADC_CR_START;

    // Poll the Interrupt Status Register (ADC_ISR) until Channel 7 finishes converting
    // ADC_ISR_EOC7 is the "End of Conversion" bit for Channel 7
    while (!(ADC->ADC_ISR & ADC_ISR_EOC7));

    // Return the 12-bit native conversion value (0 - 4095) of pin A0
    return (uint16_t)(ADC->ADC_CDR[7]);
}
