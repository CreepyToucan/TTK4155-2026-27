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
#include "Analogs.h"

int main(void) {


    ///   SETUP   ///

    UART_Init(MYUBRR);
    xmem_init();
    ADC_Init();
    SRAM_test();
    //SRAM_single_test();


    ///   LOOP  ///

    while(1) {

        //printf("Position: %d\n", GetPos(Pad_y));
        //printf("Direction: %c\n", GetDir(Pad_x));



        
        _delay_ms(100); 
    
    }

    return 0;
}