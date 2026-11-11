// menu.c

#define F_CPU 4915200UL
#define BAUD 9600
#define MYUBRR (F_CPU/16/BAUD-1)

#include <stdio.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "menu.h"
#include "Oled.h"
#include "Analogs.h"
#include "io_board.h"
#include "highscores.h"
#include "CAN.h"

#define CONTROL_CAN_ID      16     // joystick/slider/button frame to Node 2 (txPack)
#define GOAL_CAN_ID         20     // sent by Node 2 when the IR beam is broken
#define CONTROL_PERIOD_MS   5
#define QUIT_POLL_MS        50
#define BUTTON_SELECT       32     // io_getButtons().right value of the select button
#define CAL_DURATION_MS     5000
#define CAL_MIN_SPAN        80     // min raw range per axis for a valid calibration



uint8_t difficulty = 1;
const uint8_t lives_per_difficulty[3] = {5, 3, 1};
const uint8_t score_multiplier[3]     = {1, 2, 3};


// Waits for the select button to be released, pressed, and released again.
static void wait_for_click(void) {
    while (io_getButtons().right == BUTTON_SELECT) _delay_ms(20);
    _delay_ms(50);
    while (io_getButtons().right != BUTTON_SELECT) _delay_ms(20);
    while (io_getButtons().right == BUTTON_SELECT) _delay_ms(20);
}

static void menu_draw(const menu_t* menu, uint8_t selected, uint8_t font_size) {
    for (uint8_t i = 0; i < menu->item_count; i++) {
        oled_pos(i, 0, font_size);
        if (i == selected) {
            oled_print(">", font_size);   // simple cursor indicator
        } else {
            oled_print(" ", font_size);
        }
        oled_pos(i, 1, font_size);
        oled_print(menu->items[i].label, font_size);
    }
}

uint8_t menu_run(const menu_t* menu, uint8_t font_size) {
    _delay_ms(100); // add delay to avoid instantly select from previous button press
    uint8_t selected = 0;
    char last_dir = 'N'; //Neutral
    
    oled_clear();
    menu_draw(menu, selected, font_size);

    while (1) {
        //char dir = getDir(Joy_y);
        char dir = getDir(Joy_y);
        //char dir = 'N';

        // Only act on the transition INTO a direction, not while holding the joystick
        if (dir != last_dir) {
            if (dir == 'D') {
                if (selected == 0)
                    selected = menu->item_count - 1;  // wrap around
                else
                    selected--;
                menu_draw(menu, selected, font_size);
            } else if (dir == 'U') {
                selected = (selected + 1) % menu->item_count;
                menu_draw(menu, selected, font_size);
            }
            last_dir = dir;
        }

        if (io_getButtons().right == BUTTON_SELECT) {
            // Wait for release so the same click is not seen by the next screen
            while (io_getButtons().right == BUTTON_SELECT) _delay_ms(20);
            return selected;
        }

        _delay_ms(40); // simple polling rate / crude debounce (delay after an action)
    }
}

void show_dificulty_menu(void) {
    const menu_item_t difficulty_options[] = {
        {"Easy"},
        {"Medium"},
        {"Hard"}
    };
    menu_t difficulty_menu = { difficulty_options, 3 };
    uint8_t choice = menu_run(&difficulty_menu, 4);
    //todo: set difficulty here somehow
}

void start_calibration(void) {
    char buf[24];

    // Step 1: find the center with the joystick released
    oled_clear();
    oled_pos(1, 0, 5); oled_print("Release joystick", 5);
    oled_pos(3, 0, 5); oled_print("Finding center...", 5);
    _delay_ms(1500);

    uint16_t sum_x = 0, sum_y = 0;
    for (uint8_t i = 0; i < 16; i++) {
        sum_x += getRawJOY_X();
        sum_y += getRawJOY_Y();
        _delay_ms(10);
    }
    uint8_t cen_x = sum_x / 16;
    uint8_t cen_y = sum_y / 16;

    // Min/max start AT the center (not at 0) so they only ever widen
    uint8_t mn_x = cen_x, mx_x = cen_x;
    uint8_t mn_y = cen_y, mx_y = cen_y;

    // Step 2: record the extremes while the user rotates the joystick
    oled_clear();
    oled_pos(0, 0, 5); oled_print("Rotate joystick", 5);
    oled_pos(1, 0, 5); oled_print("in full circles", 5);

    uint8_t i = 0;
    while (i<5000) {
        uint8_t x = getRawJOY_X();
        uint8_t y = getRawJOY_Y();
        if (x > mx_x) mx_x = x;
        if (x < mn_x) mn_x = x;
        if (y > mx_y) mx_y = y;
        if (y < mn_y) mn_y = y;

        uint8_t secs_left = 5 - (i/1000);
        sprintf(buf, "X: %3d to %3d", mn_x, mx_x);
        oled_pos(3, 0, 5); oled_print(buf, 5);
        sprintf(buf, "Y: %3d to %3d", mn_y, mx_y);
        oled_pos(4, 0, 5); oled_print(buf, 5);
        sprintf(buf, "Time left: %ds", secs_left);
        oled_pos(6, 0, 5); oled_print(buf, 5);
        _delay_ms(100);
        i++;
    }

    // Step 3: accept only if the joystick was actually moved
    oled_clear();
    if ((mx_x - mn_x) >= CAL_MIN_SPAN && (mx_y - mn_y) >= CAL_MIN_SPAN) {
        min_joy_x = mn_x;  center_joy_x = cen_x;  max_joy_x = mx_x;
        min_joy_y = mn_y;  center_joy_y = cen_y;  max_joy_y = mx_y;
        oled_pos(3, 0, 5); oled_print("Calibration done!", 5);
    } else {
        oled_pos(2, 0, 5); oled_print("Calibration failed", 5);
        oled_pos(4, 0, 5); oled_print("Old values kept", 5);
    }
    _delay_ms(2000);
}

void show_calibration_menu(void) {
    const menu_item_t calibration_items[] = {
        {"Start Calibration"},
        {"Back"}
    };
    menu_t calibration_menu = { calibration_items, 2 };
    uint8_t choice = menu_run(&calibration_menu, 5);
    switch (choice) {
        case 0: start_calibration(); break;
        case 1: break;
    }
}

void show_settings_menu(void) {
    const menu_item_t settings_items[] = {
        {"Difficulty"},
        {"Joystick Calibration"},
        {"Back"}
    };
    menu_t settings_menu = { settings_items, 3 };
    uint8_t choice = menu_run(&settings_menu, 5);
    switch (choice) {
        case 0: show_dificulty_menu(); break;
        case 1: show_calibration_menu(); break;
        case 2: break;
    }
}

void show_high_scores(void) {
    char buf[24];

    oled_clear();
    oled_pos(0, 0, 5); oled_print("HIGH SCORES", 5);
    for (uint8_t i = 0; i < SRAM_SIZE; i++) {
        sprintf(buf, "%d. %5u", i + 1, highscores_get(i));
        oled_pos(i + 1, 0, 5);
        oled_print(buf, 5);
    }
    oled_pos(7, 0, 5); oled_print("Click to go back", 5);

    wait_for_click();
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

static void game_over_screen(uint16_t score, uint8_t rank) {
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
    wait_for_click();
}
/*
void start_new_game(void) {
    uint8_t lives = lives_per_difficulty[difficulty];
    uint8_t padX = 0;
    uint16_t seconds = 0, shown_seconds = 0xFFFF;

    can_message_t tx;
    tx.id = CONTROL_CAN_ID;
    tx.length = 3;
    can_message_t rx;

    game_countdown();

    // Drop every old msg that arrived while we weren't listening
    for (uint8_t i = 0; i < 3; i++) CAN_receive(&rx);

    oled_clear();
    oled_pos(0, 0, 5); oled_print("Playing!", 5);
    oled_pos(7, 0, 5); oled_print("Click to quit", 5);
    game_draw_lives(lives);

    uint32_t start_ms     = 0//;timer_ms();
    uint32_t last_ctrl_ms = start_ms;
    uint32_t last_quit_ms = start_ms;

    while (lives > 0) {
        uint32_t now = timer_ms();

        // Recieve Goal msg from Node 2 (the ball passed the IR beam)
        if (CAN_receive(&rx) && rx.id == GOAL_CAN_ID) {
            lives--;
            game_draw_lives(lives);
            if (lives == 0) break;
        }

        // Send controls to Node 2 every 5 ms
        if (now - last_ctrl_ms >= CONTROL_PERIOD_MS) {
            last_ctrl_ms = now;
            padX = HandlePad_X();
            tx.data[0] = (uint8_t)getPos(Joy_x);
            tx.data[1] = padX;
            tx.data[2] = getButton();
            CAN_transmit(&tx);
        }

        // Elapsed time: redraw timer once per second
        seconds = (uint16_t)((now - start_ms) / 1000);
        if (seconds != shown_seconds) {
            shown_seconds = seconds;
            game_draw_time(seconds);
        }

        // Quit button (polled slowly to keep SPI traffic low)
        if (now - last_quit_ms >= QUIT_POLL_MS) {
            last_quit_ms = now;
            if (io_getButtons().right == BUTTON_SELECT) break;
        }
    }

    // Game over
    // Neutral command so the motor and solenoid don't keep acting on the last msg
    tx.data[0] = 0;
    tx.data[1] = padX;
    tx.data[2] = 0;
    CAN_transmit(&tx);

    seconds = (uint16_t)((timer_ms() - start_ms) / 1000);
    uint16_t score = seconds * score_multiplier[difficulty];
    uint8_t rank = highscores_add(score);

    game_over_screen(score, rank);
}
*/