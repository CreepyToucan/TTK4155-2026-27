#include <avr/io.h>
#include <avr/interrupt.h>

#include "UART.h"
#include <avr/io.h>
#include <avr/interrupt.h>


// Initialize the UART interface
void UART_Init(unsigned int ubrr) {
    // Set baud rate
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;
    
    // Enable receiver, transmitter, and receive interrupts
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
    
    // Set frame format: 8 data bits, 1 stop bit
    // Note: On the ATmega162, URSEL0 must be written to 1 when writing to UCSR0C
    UCSR0C = (1 << URSEL0) | (3 << UCSZ00);

    /*
    We asked gemini AI how to Link standard I/O (printf) to our UART transmission wrapper.
    */
    fdevopen(UART_Transmit_printf, UART_Receive); // Link standard I/O (printf) to our UART transmission wrapper
}

// Transmit a single character 
void UART_Transmit(unsigned char data) {
    // Wait until the transmit buffer is empty
    while (!(UCSR0A & (1 << UDRE0)));
    
    // Load data into the buffer, triggering transmission
    UDR0 = data;
}

// Wrapper function to link UART_Transmit to standard I/O (printf)
int UART_Transmit_printf(char data, FILE *stream) {
    // Convert newline to carriage return + newline for standard terminal behavior
    if (data == '\n') {
        UART_Transmit('\r');
    }
    
    UART_Transmit(data);
    return 0; // Return 0 to indicate successful transmission
}

int UART_Receive(){
	if(!(UCSR0A & (1<<RXC0))){
		return 0;
	}
	return UDR0;
}