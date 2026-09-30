// menu.c
#define F_CPU 4915200UL

#include <stdio.h>
#include <util/delay.h>
#include "menu.h"
#include "Oled.h"
#include "Analogs.h"
#include "io_board.h"


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
        char dir = getDir(Joy_y);

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

        if (io_getButtons().right == 32) {
            printf("%d", selected);
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

void show_sound_menu(void) {
    //todo
}

void show_settings_menu(void) {
    const menu_item_t settings_items[] = {
        {"Difficulty"},
        {"Sound"},
        {"Back"}
    };
    menu_t settings_menu = { settings_items, 3 };
    uint8_t choice = menu_run(&settings_menu, 5);
    switch (choice) {
        case 0: show_dificulty_menu(); break;
        case 1: show_sound_menu(); break;
        case 3: break;
    }
}


void show_high_scores(void) {
    //todo
}

void start_new_game(void) {
    //todo
}
