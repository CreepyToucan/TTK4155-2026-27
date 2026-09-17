#define F_CPU 4915200UL
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "UART.h"
#include "XMEM.h"
#include "ADC.h"

int main(void) {
    UART_Init(MYUBRR);
    xmem_init();
    ADC_Init();
    //SRAM_test();
    //SRAM_single_test();

    while(1) {
        printf("ADC Value: %d\n", ADC_ReadChannel(0)); 
        _delay_ms(10); 
    }

    return 0;
}