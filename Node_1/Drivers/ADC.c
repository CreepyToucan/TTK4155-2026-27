#include <avr/io.h>
#include <stdint.h>

#define ADC_BASE_ADDRESS 0x1000 
#define ADC_REG (*(volatile uint8_t *)(ADC_BASE_ADDRESS))

// BUSY Pin config
#define BUSY_PORT PINB
#define BUSY_PIN  PB0


void ADC_Init(void) {

    // PWM setup for generating the clock signal for the ADC at 2.4576 MHz
    DDRD |= (1 << DDD5);
    TCCR1A |= (0 << WGM11) | (0 << WGM10) | (1 << COM1A0);
    TCCR1B |= (0 << WGM13) | (1 << WGM12) | (0 << CS12) | (0 << CS11) | (1 << CS10);
    OCR1A = 0;

    // BUSY pin as input
    DDRB &= ~(1 << BUSY_PIN);
    
    // pill-up resistor for stability
    PORTB |= (1 << BUSY_PIN); 
}


    // 0: JOYY
    // 1: JOYX
    // 2: PADY
    // 3: PADX
uint8_t ADC_ReadChannel(uint8_t channel) {
   
    if (channel > 3) return 0; 

    // D7=1 (Singol conversion), D6=0 (Single-ended), D5=0 (Unipolar)
    // D4=0 (immediatly start of conversion), D3=0 (Normal operation)
    // I bit D2, D1, D0 contain the channel number (0-3)
    uint8_t config_byte = 0b10000000 | channel;

    // Writes the configuration. This generates automatically the WR pulse.
    ADC_REG = config_byte;

    // Wait until the BUSY pin is low (conversion in progress)
    while (!(BUSY_PORT & (1 << BUSY_PIN))) {
        // Waiting loop (blocking)
    }

    // Read the result. This generates automatically the RD pulse.
    return ADC_REG;
}