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
#define JOYX 16
#define PADX 15


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


int main(void) {


    ///   SETUP   ///

    UART_Init(MYUBRR);
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


    // CAN messages definitio //
    can_message_t tx_msg;
    tx_msg.id = 0x123;         // 11-bit standard ID
    tx_msg.length = 1;         // 1 byte of data
    tx_msg.data[0] = 0xDE;


    can_message_t rx_msg;



    ///   LOOP  ///

    while(1) {


        tx_msg.id = JOYX; 
        tx_msg.data[0] = getPos(Joy_x);
        CAN_transmit(&tx_msg);
   
        _delay_ms(10);

        tx_msg.id = PADX; 
        tx_msg.data[0] = getPos(Pad_x);
        CAN_transmit(&tx_msg);

        _delay_ms(100); 

    }


    return 0;
}
