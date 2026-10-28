#define F_CPU 84000000UL
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

#include <stdio.h>
#include <stdarg.h>
#include "sam.h"
#include "../Drivers/uart.h"
#include "../Drivers/can.h"
#include "../Drivers/pwm.h"
#include "../Libraries/Servo.h"
#include "../Drivers/ir.h"
#include "../Drivers/adc.h"
#include "../Libraries/encoder.h"
#include "../Libraries/motor.h"

// CAN Message IDs
#define JOYX   16
#define PADX   15

void delay_ms(uint32_t ms) {
for (volatile uint32_t i = 0; i < (ms * 8400); i++); 
}


int main()
{
    ///   SETUP   ///
    SystemInit();
    WDT->WDT_MR = WDT_MR_WDDIS; //Disable Watchdog Timer
    uart_init(F_CPU, BAUD);
    ir_init();
    can_init((CanInit){.brp = 104, .phase1 = 6, .phase2 = 5, .propag = 1 , .sjw = 0, .smp = 0}, 0);
    pwm_init(&DUE_PIN_21, 50);
    encoder_init();
    motor_init();



    uint8_t score = 0;

    ///   LOOP   ///

    while (1){


        printf("Encoder Position: %ld\r\n", encoder_read());
        motor_set_speed(50, MOTOR_DIR_FORWARD);

        delay_ms(10);
        

    }
} 
