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

#define IO_board PB3
#define Oled PB2    //slave cs pin


oled_config_t my_oled = {
        .ctrl_port = &PORTD,
        .ctrl_ddr  = &DDRD,
        .dc_pin    = PD2,
        .cs_port   = &PORTB,
        .cs_pin    = PB2
    };


int main(void) {


    ///   SETUP   ///

    UART_Init(MYUBRR);
    xmem_init();
    ADC_Init();
    initButton();
    spi_init();
    //slave_init(IO_board);
    slave_init(Oled);
    oled_init(&my_oled);
  

    ///   LOOP  ///

    while(1) {


        char c = getDir(Joy_x);
        char str_buff[2] = {c,'\0'};

        //printf("val: %c\n",str);

        oled_pos(0,0);  
        oled_print((char[]){getDir(Joy_x),'\0'});
        oled_pos(2,0);
        oled_print((char[]){getDir(Joy_y),'\0'});


    }

    return 0;
}