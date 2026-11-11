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
#include "highscores.h"

// slave cs pins
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

int8_t joyX = 0;
uint8_t padX = 0;
uint8_t button = 0;
uint8_t score = 0;

volatile uint8_t flag_5ms = 0;
volatile uint8_t flag_50ms = 0; 
volatile uint8_t flag_1000ms = 0; 
volatile uint16_t seconds = 0; // Counter for seconds




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
        seconds++; // Increment the seconds counter
    }
}


/// GAME FUNCTIONS ///


// Waits for the select button to be released, pressed, and released again.
void wait_for_click(void) {
    while (io_getButtons().right != 32) {
        printf("%d\n",io_getButtons().right);
        _delay_ms(100);
    }
}

static void game_countdown(void) {
    char buf[4];
    oled_clear();
    oled_pos(2, 0, 5); oled_print("Get ready!", 5);
    for (uint8_t n = 3; n > 0; n--) {
        sprintf(buf, "%d", n);
        oled_pos(4, 0, 8);
        oled_print(buf, 8);
        _delay_ms(1000);
    }
}

static void game_draw_lives(uint8_t lives) {
    char buf[16];
    sprintf(buf, "Lives: %d", lives);
    oled_pos(2, 0, 5);
    oled_print(buf, 5);
}

static void game_draw_time(uint16_t seconds) {
    char buf[16];
    sprintf(buf, "Time: %3us", seconds);   // fixed width, overwrites the old value
    oled_pos(4, 0, 5);
    oled_print(buf, 5);
}

void game_over_screen(uint16_t score, uint8_t rank) {
    char buf[24];
    oled_clear();
    oled_pos(0, 0, 8); oled_print("GAME OVER", 8);
    sprintf(buf, "Score: %u", score);
    oled_pos(3, 0, 5); oled_print(buf, 5);
    if (rank) {
        oled_pos(4, 0, 5); oled_print("New high score!", 5);
        sprintf(buf, "Rank: %d", rank);
        oled_pos(5, 0, 5); oled_print(buf, 5);
    } else {
        oled_pos(4, 0, 5); oled_print("Not in the top 5", 5);
    }
    oled_pos(7, 0, 5); oled_print("Click to continue", 5);
}

void initGame_oled(uint8_t lives){
    oled_clear();
    oled_pos(0, 0, 5); oled_print("Playing!", 5);
    oled_pos(7, 0, 5); oled_print("Click to quit", 5);
    game_draw_lives(lives);

}


menu_t main_menu = { main_items, 4 };

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

    // SCOREBOARD initialization //
    highscores_init();
    


    // CAN messages definition //
    can_message_t tx_msg;
    tx_msg.id = txPack;         // 11-bit standard ID
    tx_msg.length = 3;         // 3 bytes of data
    tx_msg.data[0] = 0x00;
    tx_msg.data[1] = 0x00;
    tx_msg.data[2] = 0x00;

    can_message_t rx_msg;

    ///   LOOP  ///

    while(1){

        uint8_t choice = menu_run(&main_menu, 8);
         switch (choice) {
             case 0 : break;           
             case 1: show_settings_menu(); break;
             case 2: show_high_scores(); break;
             case 3: oled_funny_graphics(); _delay_ms(2000); break;   // quit
            }

            // GAME INIT //
            uint8_t lives = lives_per_difficulty[difficulty];
            seconds = 0;
            initGame_oled(lives);
            // Drop every old msg that arrived while we weren't listening
            for (uint8_t i = 0; i < 3; i++) CAN_receive(&rx_msg);

            while((lives > 0)&&(choice == 0)){
                
                    /// RECEPTION ///
                    if(CAN_receive(&rx_msg)){
                        if(rx_msg.id == rxPack){
                            lives --;
                            if(lives == 0) break;

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

                    // OLED  UPDATE //
                    if(flag_50ms){

                        flag_50ms = 0; // Clear the flag immediately
                        game_draw_lives(lives);
                        if (io_getButtons().right == 32) break;

                    }
                    
                    // SECONDs UPDATE //
                    if(flag_1000ms){

                        flag_1000ms = 0; // Clear the flag immediately
                        game_draw_time(seconds);
                    }
                
            }


            // Game over
            // Neutral command so the motor and solenoid don't keep acting on the last msg
            tx_msg.data[0] = 0;
            tx_msg.data[1] = padX;
            tx_msg.data[2] = 0;
            CAN_transmit(&tx_msg);

            uint16_t score = seconds * score_multiplier[difficulty];
            uint8_t rank = highscores_add(score);

            game_over_screen(score, rank);
            wait_for_click();
        

    }

        return 0;


}

    // highscores_init();

    // /// LOOP ///
    // while (1) {
    //     uint8_t choice = menu_run(&main_menu, 8);
    //     switch (choice) {
    //         case 0: start_new_game(); break;
    //         case 1: show_settings_menu(); break;
    //         case 2: show_high_scores(); break;
    //         case 3: oled_funny_graphics(); _delay_ms(2000); break;   // quit
    //     }
    // }

    
