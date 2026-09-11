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
    
    // 3. Enable global interrupts
    sei();
    
    // 4. Print startup messages
    printf("----------------------------------\n");
    printf("ATmega162 UART Driver Initialized!\n");
    printf("Baud Rate: %d\n", BAUD);
    printf("----------------------------------\n");
    
    // 5. Main application loop
    while (1) {
        
        printf("System is running... Send me a character!\n");
        
        _delay_ms(5000);
        
    }
    
    return 0;
}