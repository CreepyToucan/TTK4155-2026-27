
#define F_CPU 4915200UL

#include <avr/io.h>
#include <util/delay.h>

int main(void) {
    // 1. Configure Port A (Pins 39-32) as output
    // 0xFF in binary is 11111111, making all 8 pins outputs.
    DDRA = 0xFF; 
    DDRC = 0xFF;

    while (1) {
        // 2. Set your defined pins HIGH. 
        // Let's test by enabling PA0 (Pin 39), PA2 (37), PA4 (35), and PA6 (33).
        // Binary: 01010101 (0x55 in Hex)
        PORTA = 0xFF; 

        PORTC = 0x80;

        _delay_ms(1000); // Wait for 1 second
        PORTA = 0x15; 
        PORTC = 0x00;

        _delay_ms(1000); // Wait for 1 second
    
    }

    return 0;
}