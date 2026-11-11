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
#include "SPI.h"
#include "Oled.h"
#include "io_board.h"
#include "menu.h"
#include "MCP2515.h"
#include "CAN.h"

//slave cs pin
#define IO_board PB1
#define Oled PB2    
#define CAN_MCP2515 PB3


// CAN IDs
#define txPack 16
#define rxPack 17


oled_config_t my_oled = {
    .ctrl_port = &PORTD,
    .ctrl_ddr  = &DDRD,
    .dc_pin    = PD2,
    .cs_port   = &PORTB,
    .cs_pin    = PB2
};


io_board_config_t my_IO = {
    .cs_port = &PORTB,
    .cs_pin = IO_board
};

mcp2515_config_t my_mcp2515 = {
    .cs_port = &PORTB,
    .cs_pin = CAN_MCP2515
};

const menu_item_t main_items[] = {
    {"New Game"},
    {"Settings"},
    {"High Scores"},
    {"Quit"}
};

menu_t main_menu = { main_items, 4 };

int8_t joyX = 0;
uint8_t padX = 0;
uint8_t button = 0;
uint8_t score = 0;

volatile uint8_t flag_5ms = 0;
volatile uint8_t flag_50ms = 0; 
volatile uint8_t flag_1000ms = 0; 



void Timer0_Init(void) {
    // Configure Timer0 in CTC mode (Clear Timer on Compare Match)
    TCCR0 = (1 << WGM01);
    
    // Set Prescaler to 64
    TCCR0 |= (1 << CS01) | (1 << CS00);
    
    // Calculate Compare Match value for 1ms interrupt
    //OCR0 = (F_CPU / (Prescaler * Target_Frequency)) - 1
    OCR0 = (F_CPU / (64 * 1000)) - 1; 
    
    // Enable Timer0 Compare Match Interrupt
    TIMSK |= (1 << OCIE0);
}


ISR(TIMER0_COMP_vect) {
    static uint8_t count_5ms = 0;
    static uint8_t count_50ms = 0;
    static uint16_t count_1000ms = 0;

    count_5ms++;
    count_50ms++;
    count_1000ms++;

    // Check if 5ms has passed
    if(count_5ms >= 5) {
        flag_5ms = 1;     
        count_5ms = 0;    
    }
    
    // Check if 50ms has passed
    if(count_50ms >= 50) {
        flag_50ms = 1;
        count_50ms = 0;
    }

    // Check if 1000ms (1 second) has passed
    if(count_1000ms >= 1000) {
        flag_1000ms = 1;
        count_1000ms = 0;
    }
}



int main(void) {


    ///   SETUP   ///

    UART_Init(MYUBRR);
    Timer0_Init();
    sei();
    xmem_init();
    ADC_Init();
    initButton();
    spi_init();
    // initialize cs pins for each slave //
    slave_init(Oled);
    slave_init(IO_board);
    slave_init(CAN_MCP2515);
    // initialize slaves SPI pins //
    oled_init(&my_oled);
    io_board_init(&my_IO);
    mcp2515_init(&my_mcp2515);

    // CAN initialization //
    mcp2515_reset();
    CAN_set_mode(CAN_MODE_CONFIG);
    // CNF1 = SWJ:1 BRP:9, CNF2 = BTLMODE:1 SAM:0 PHSEG1:6 PRSEG:1, CNF3 = PHSEG2:5
    CAN_init(0x09, 0xB1, 0x05); 
    CAN_set_mode(CAN_MODE_NORMAL);

    


    // CAN messages definition //
    can_message_t tx_msg;
    tx_msg.id = txPack;         // 11-bit standard ID
    tx_msg.length = 3;         // 3 bytes of data
    tx_msg.data[0] = 0x00;
    tx_msg.data[1] = 0x00;
    tx_msg.data[2] = 0x00;

    can_message_t rx_msg;


    ///   LOOP  ///

    while(1) {



        /// RECEPTION ///
        if(CAN_receive(&rx_msg)){
            if(rx_msg.id == rxPack){
                score = rx_msg.data[0]; // Update score from received CAN message
                //printf("Score: %d\n", score);
            }
        }



        if(flag_5ms){

            flag_5ms = 0; // Clear the flag immediately

            /// SAMPLING ///
            joyX = getPos(Joy_x);
            padX = HandlePad_X();
            button = getButton();
            //printf("Joy X: %d, Pad X: %d, Button: %d\n", joyX, padX, button); //need to remove to keep the 5ms loop fast enough for CAN transmission

            /// TRANSMISSION ///
            tx_msg.id = txPack; 
            tx_msg.data[0] = (uint8_t)joyX;
            tx_msg.data[1] = padX;
            tx_msg.data[2] = button;
            CAN_transmit(&tx_msg); 

        }

        if(flag_50ms){

            flag_50ms = 0; // Clear the flag immediately

            char string_buffer[20]; 
            snprintf(string_buffer, sizeof(string_buffer), "%d", score);

            /// DISPLAY ///
            //oled_clear();
            oled_pos(0, 0,8);
            oled_print("Score: ", 8);
            oled_print(string_buffer, 8);

        }

        if(flag_1000ms){

            flag_1000ms = 0; // Clear the flag immediately
            xmem_write(score,0x0000,0x1400 ); // Write the score to external memory at address 0x0000
            printf("score: %d\n", xmem_read(0x0000,0x1400)); 

        }


    }
    

    return 0;
}
