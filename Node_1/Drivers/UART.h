#ifndef UART_H_
#define UART_H_

#include <stdio.h> 

// Function prototypes
void UART_Init(unsigned int ubrr);
void UART_Transmit(unsigned char data);
int UART_Transmit_printf(char data, FILE *stream);
int UART_Receive();


#endif /* UART_H_ */