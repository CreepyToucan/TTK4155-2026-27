#define F_CPU 4915200UL
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include "UART.h"


// Define desired baud rate and calculate the UBRR value
#define BAUD 9600
#define MY_UBRR (F_CPU / 16 / BAUD - 1)

int main(void) {
    // 1. Initialize the UART hardware
    UART_Init(MY_UBRR);
    
    // 2. Link standard I/O (printf) to our UART transmission wrapper
    fdevopen(UART_Transmit_printf, NULL);
    
    // 3. Enable global interrupts (Crucial! The Receive ISR will not fire without this)
    sei();
    
    // 4. Print startup messages
    printf("----------------------------------\n");
    printf("ATmega162 UART Driver Initialized!\n");
    printf("Baud Rate: %d\n", BAUD);
    printf("----------------------------------\n");
    
    // 5. Main application loop
    while (1) {
        // Print a periodic message to the terminal
        printf("System is running... Send me a character!\n");
        
        // Delay for 5000 milliseconds (5 seconds)
        _delay_ms(5000);
        
        // Note: Even while trapped in this delay, if a character arrives 
        // via serial, the ATmega162 will pause the delay, jump to the ISR 
        // in uart.c, process the character, and return right back here!
    }
    
    return 0;
}