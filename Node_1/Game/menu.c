// menu.c
#define F_CPU 4915200UL

#include <util/delay.h>
#include "menu.h"
#include "Oled.h"
#include "Analogs.h"
#include "io_board.h"


static void menu_draw(const menu_t* menu, uint8_t selected) {
    //oled_clear();
    for (uint8_t i = 0; i < menu->item_count; i++) {
        oled_pos(i, 0);
        if (i == selected) {
            oled_print(">");   // simple cursor indicator
        } else {
            oled_print(" ");
        }
        oled_pos(i, 1);
        oled_print(menu->items[i].label);
    }
}

uint8_t menu_run(const menu_t* menu) {
    uint8_t selected = 0;
    char last_dir = 'N'; //Neutral

    menu_draw(menu, selected);

    while (1) {
        char dir = getDir(Joy_y);

        // Only act on the transition INTO a direction, not while holding the joystick
        if (dir != last_dir) {
            if (dir == 'U') {
                if (selected == 0)
                    selected = menu->item_count - 1;  // wrap around
                else
                    selected--;
                menu_draw(menu, selected);
            } else if (dir == 'D') {
                selected = (selected + 1) % menu->item_count;
                menu_draw(menu, selected);
            }
            last_dir = dir;
        }

        if (io_getButtons().left) { //todo: check what "io_getButtons().left" returns
            return selected;
        }

        _delay_ms(20); // simple polling rate / crude debounce (delay after an action)
    }
}

void show_settings_menu(void) {
    const menu_item_t settings_items[] = {
        {"Difficulty"},
        {"Sound"},
        {"Back"}
    };
    menu_t settings_menu = { settings_items, 3 };

    uint8_t choice = menu_run(&settings_menu);
    if (choice == 2) return; // "Back" selected
    // handle Difficulty / Sound...
}

void show_high_scores(void) {
    //todo
}

void start_new_game(void) {
    //todo
}
