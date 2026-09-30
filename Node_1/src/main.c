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
    CAN_set_mode(CAN_MODE_LOOPBACK);


    // CAN messages definitio //
    can_message_t tx_msg;
    tx_msg.id = 0x123;         // 11-bit standard ID
    tx_msg.length = 4;         // 4 bytes of data
    tx_msg.data[0] = 0xDE;
    tx_msg.data[1] = 0xAD;
    tx_msg.data[2] = 0xBE;
    tx_msg.data[3] = 0xEF;

    can_message_t rx_msg;



    ///   LOOP  ///

    while(1) {

        // Send the message
        if (CAN_transmit(&tx_msg)) {
            // Give the MCP2515 time to process the loopback
            _delay_ms(10); 
            
            // Check if the message was successfully received back
            if (CAN_receive(&rx_msg)) {
                // Verify the received data matches what we sent
                if (rx_msg.id == 0x123 && rx_msg.data[0] == 0xDE) {
                    // Toggle an LED to indicate successful transmission and reception
                    printf("Success!\n");
                }
            }
        }
        
        _delay_ms(500); // Wait 500ms before testing again

        /*
        // Main menu
        uint8_t choice = menu_run(&main_menu);
        switch (choice) {
            case 0: start_new_game(); break;
            case 1: show_settings_menu(); break;
            case 2: show_high_scores(); break;
            case 3: break; //todo: quit or shutdown
        }

        io_buttons_t butt = io_getButtons();
        printf("%d",butt.left);

        io_info_t info;
        io_getInfo(&info);
        printf("IO_avr: %d",info.serial);
        */
    }


    return 0;
}
