#include <avr/io.h>
#include <stdint.h>
#include "Analogs.h"
#include "ADC.h"
#include "UART.h"

#define F_CPU 4915200UL
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

#include <util/delay.h>


int8_t HandleJOY_Y(void){
    uint8_t raw = ADC_ReadChannel(0);

    int8_t pos = raw-159;

    if(pos < 0)
        return (pos*100/88);
    else
        return (pos*100/86);
}

int8_t HandleJOY_X(void){
    uint8_t raw = ADC_ReadChannel(1);

    if(raw <= 165 && raw >= 161)
        raw = 163;

        
    int8_t pos = raw-163;

    if(pos < 0)
        return (pos*100/92);
    else
        return (pos*100/82);
}

uint8_t HandlePad_Y(void){
    return ADC_ReadChannel(2);;
} 

uint8_t HandlePad_X(void){
    return ADC_ReadChannel(3);
}


int8_t getPos(InputType obj) {

    switch(obj) {
        case Joy_y:
            return HandleJOY_Y();
        case Joy_x:
            return HandleJOY_X();

        default:
            return -1;
    }
}

char getDir(InputType obj){
    
    switch(obj){

        case Joy_y:
            if(HandleJOY_Y() > 2)
                return 'U';
            else if(HandleJOY_Y() < -2)
                return 'D';
            else
                return 'N';

        case Joy_x:
            if(HandleJOY_X() > 2)
                return 'R';
            else if(HandleJOY_X() < -2)
                return 'L';
            else
                return 'N';
              
        default:
            return 'X';
    }
}


void initButton(void){
    DDRD &= ~(1 << DDD4);
    PORTD |= (1 << PORTD4);
}


uint8_t getButton(void){

    if (!(PIND & (1 << PIND4))) {
        return 1; // Button is pressed
    } else {
        return 0; // Button is unpressed
    }
}

