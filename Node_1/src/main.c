#define F_CPU 4915200UL
#define BAUD 9600
#define MYUBRR (F_CPU/16/BAUD-1)

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
#include "Timer.h"
#include "highscores.h"

// slave cs pins
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
    printf("boot\r\n");
    
    /// SETUP ///
    UART_Init(MYUBRR);
    timer_init();
    sei();
    xmem_init();
    ADC_Init();
    initButton();
    spi_init();

    // initialize cs pins for each slave
    slave_init(Oled);
    slave_init(IO_board);
    slave_init(CAN_MCP2515);

    // initialize slaves
    oled_init(&my_oled);
    io_board_init(&my_IO);
    mcp2515_init(&my_mcp2515);

    // CAN initialization
    mcp2515_reset();
    CAN_set_mode(CAN_MODE_CONFIG);
    // CNF1 = SWJ:1 BRP:9, CNF2 = BTLMODE:1 SAM:0 PHSEG1:6 PRSEG:1, CNF3 = PHSEG2:5
    CAN_init(0x09, 0xB1, 0x05);
    CAN_set_mode(CAN_MODE_NORMAL);

    highscores_init();

    /// LOOP ///
    while (1) {
        uint8_t choice = menu_run(&main_menu, 8);
        switch (choice) {
            case 0: start_new_game(); break;
            case 1: show_settings_menu(); break;
            case 2: show_high_scores(); break;
            case 3: oled_funny_graphics(); _delay_ms(2000); break;   // quit
        }
    }
    return 0;
}
