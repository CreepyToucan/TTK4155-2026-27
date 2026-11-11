// menu.c
#define F_CPU 4915200UL

#include <stdio.h>
#include <util/delay.h>
#include "menu.h"
#include "Oled.h"
#include "Analogs.h"
#include "io_board.h"
#include "highscores.h"

#define BUTTON_SELECT       32     // io_getButtons().right value of the select button
#define CAL_DURATION_MS     5000
#define CAL_MIN_SPAN        80     // min raw range per axis for a valid calibration

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

    uint32_t start_ms = timer_ms();
    uint32_t last_ui  = 0;
    uint32_t elapsed;

    while ((elapsed = timer_ms() - start_ms) < CAL_DURATION_MS) {
        uint8_t x = getRawJOY_X();
        uint8_t y = getRawJOY_Y();
        if (x > mx_x) mx_x = x;
        if (x < mn_x) mn_x = x;
        if (y > mx_y) mx_y = y;
        if (y < mn_y) mn_y = y;

        if (elapsed - last_ui >= 200) {   // redraw only ~5x per second
            last_ui = elapsed;
            uint8_t secs_left = (CAL_DURATION_MS - elapsed + 999) / 1000;
            sprintf(buf, "X: %3d to %3d", mn_x, mx_x);
            oled_pos(3, 0, 5); oled_print(buf, 5);
            sprintf(buf, "Y: %3d to %3d", mn_y, mx_y);
            oled_pos(4, 0, 5); oled_print(buf, 5);
            sprintf(buf, "Time left: %ds", secs_left);
            oled_pos(6, 0, 5); oled_print(buf, 5);
        }
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
    for (uint8_t i = 0; i < HS_COUNT; i++) {
        sprintf(buf, "%d. %5u", i + 1, highscores_get(i));
        oled_pos(i + 1, 0, 5);
        oled_print(buf, 5);
    }
    oled_pos(7, 0, 5); oled_print("Click to go back", 5);

    wait_for_click();
}

void start_new_game(void) {
    //todo
}
