#include <avr/io.h>
#include <stdint.h>
#include "Analogs.h"
#include "ADC.h"

float HandleJOY_Y(void){
    uint8_t raw = ADC_ReadChannel(0);

    float pos = raw-159;

    if(pos < 0)
        return pos/88;
    else
        return pos/86;
}

float HandleJOY_X(void){
    uint8_t raw = ADC_ReadChannel(1);

    float pos = raw-163;

    if(pos < 0)
        return pos/92;
    else
        return pos/82;
}

float HandlePad_Y(void){
    uint8_t raw = ADC_ReadChannel(2);
    return raw/255.0;
} 

float HandlePad_X(void){
    uint8_t raw = ADC_ReadChannel(3);
    return raw/255.0;
}


float GetPos(InputType obj) {

    switch(obj) {
        case Joy_y:
            return HandleJOY_Y();
        case Joy_x:
            return HandleJOY_X();
        case Pad_y:
            return HandlePad_Y();
        case Pad_x:
            return HandlePad_X();
        default:
            return -1.0;
    }
}


char GetDir(InputType obj){
    
    switch(obj){

        case Joy_y:
            if(HandleJOY_Y() > 0.05)
                return 'U';
            else if(HandleJOY_Y() < -0.05)
                return 'D';
            else
                return 'N';

        case Joy_x:
            if(HandleJOY_X() > 0.05)
                return 'R';
            else if(HandleJOY_X() < -0.05)
                return 'L';
            else
                return 'N';

        case Pad_y:
            if(HandlePad_Y() > 0.5)
                return 'U';
            else
                return 'D';
        
        case Pad_x:
            if(HandlePad_X() > 0.5)
                return 'R';
            else
                return 'L';
            
        default:
            return 'X';
    }
}
