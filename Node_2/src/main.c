#define F_CPU 84000000UL
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

#include <stdio.h>
#include <stdarg.h>
#include "sam.h"
#include "../Libraries/time.h"
#include "../Drivers/uart.h"
#include "../Drivers/can.h"
#include "../Drivers/pwm.h"
#include "../Libraries/Servo.h"
#include "../Drivers/ir.h"
#include "../Drivers/adc.h"
#include "../Libraries/encoder.h"
#include "../Libraries/motor.h"
#include "../Libraries/solenoid.h"
#include "../Libraries/pid.h"

// CAN Message IDs
#define rxPack 16
#define txPack 17


// Set this to the maximum encoder count corresponding to full right travel
#define ENCODER_MAX_TICKS 2776
#define SAMPLE_TIME_MS    10
#define SAMPLE_TIME_SEC   (SAMPLE_TIME_MS / 1000.0f)

void delay_ms(uint32_t ms) {
for (volatile uint32_t i = 0; i < (ms * 8400); i++); 
}


uint32_t last_1ms_tick = 0;
uint32_t last_5ms_tick = 0;
uint32_t last_10ms_tick = 0;
uint32_t last_20ms_tick = 0;
uint32_t last_50ms_tick = 0;


int main(){

    ///   SETUP   ///
    SystemInit();
    WDT->WDT_MR = WDT_MR_WDDIS; //Disable Watchdog Timer
    uart_init(F_CPU, BAUD);
    ir_init();
    can_init((CanInit){.brp = 104, .phase1 = 6, .phase2 = 5, .propag = 1 , .sjw = 0, .smp = 0}, 0);
    pwm_init(&DUE_PIN_21, 50);
    motor_init();   // Left forward, Right reverse
    encoder_init(); // Initialize quadrature encoder on the far left
    solenoid_init();


    // Initialize PI controller
    PIDController pos_pid;
    // P-I-D
    pid_init(&pos_pid, 0.06f, 0.01f, 0.0f, -100.0f, 100.0f);
    calibrate_encoder_to_left_stop();


    uint8_t score = 0;
    CanMsg RXmsg;
    CanMsg TXmsg;


    int8_t joyX = 0;
    uint8_t padX = 0;
    uint8_t button = 0;

    ///   LOOP   ///

    while (1){

        uint32_t current_time = totalMsecs(time_now());


        // Read joystick and trackpad inputs from CAN as fast as possible
        if(can_rx(&RXmsg)){
            joyX = (int8_t)RXmsg.byte[0];
            padX = RXmsg.byte[1];
            button = RXmsg.byte[2];

        }

        //printf("Joy X: %d, Pad X: %d, Button: %d\r\n", (int8_t)RXmsg.byte[0], RXmsg.byte[1], RXmsg.byte[2]);  //to be reomoved : is fucking up the scheduler


        if(current_time - last_1ms_tick >= 1) {
            last_1ms_tick = current_time;
            ir_update();   //ir update every 1ms 

        }

        if (current_time - last_5ms_tick >= 5) {
                last_5ms_tick = current_time;

                if(ir_blocked_event()){
                    score++;
                    //printf("IR blocked! Score: %d\r\n", score);
                    TXmsg.id = txPack;
                    TXmsg.length = 1;
                    TXmsg.byte[0] = score;
                    can_tx(TXmsg);
                }
               
            }


        if (current_time - last_10ms_tick >= 10) {

            //printf("PAD: %d\r\n", padX);

            last_10ms_tick = current_time;

            // Map trackpad input (0..255) to target encoder ticks (0..ENCODER_MAX_TICKS)
            float setpoint_ticks = -1 * ((float)padX / 255.0f) * ENCODER_MAX_TICKS;

            // Read current encoder position
            int32_t current_ticks = encoder_read(); 

            // Compute PI output
            float control_output = pid_update(&pos_pid, setpoint_ticks, (float)current_ticks, SAMPLE_TIME_SEC);

            // Apply speed and direction to motor driver
            if (control_output >= 0.0f) {
                motor_set_speed((uint8_t)control_output, MOTOR_DIR_FORWARD);
            } else {
                motor_set_speed((uint8_t)(-control_output), MOTOR_DIR_REVERSE);
            }   
            
            
        }

       if (current_time - last_20ms_tick >= 20) {

            last_20ms_tick = current_time;

            servoSetPosition(&DUE_PIN_21, joyX); 

            if(button == 1){
                solenoid_shoot();
            }
            else{
                solenoid_rest();
            }
            
        }    
        
    }
} 
