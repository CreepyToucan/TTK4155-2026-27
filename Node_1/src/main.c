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

//slave cs pin
#define IO_board PB1
#define Oled PB2    

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
    slave_init(Oled);
    slave_init(IO_board);
    oled_init(&my_oled);
    io_board_init(&my_IO);


    ///   LOOP  ///

    while(1) {
        // while(1){
        //     io_buttons_t butt = io_getButtons();
        //     printf("%d\n",butt.right);
        //     _delay_ms(20);
        // }

        // Main menu
        uint8_t choice = menu_run(&main_menu);
        switch (choice) {
            case 0: start_new_game(); break;
            case 1: show_settings_menu(); break;
            case 2: show_high_scores(); break;
            case 3: break; //todo: quit or shutdown
        }
    }
    return 0;
}
