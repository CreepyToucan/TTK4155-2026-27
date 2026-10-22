#include <stdio.h>
#include "sam.h"
#include "../Drivers/pwm.h"

// -100 : 2.1ms 
//  100 : 0.9ms
uint8_t servoSetPosition(pwm_pin_config_t* config,int8_t val){

    uint16_t pulse;

    if(val > 100)
        val = 100;

    if(val < -100)
        val = -100;

    pulse = (-0.006*val + 1.5) * 1000; 

    if((pulse < 900) || (pulse > 2100)){
        return 0;
    }
    else{
        pwm_set_pulse_us(config,pulse);
        return 1;
    }

}
