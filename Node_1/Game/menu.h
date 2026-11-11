#include <avr/io.h>
#ifndef MENU_H
#define MENU_H

typedef struct {
    const char* label;   // must point to a string in flash (PROGMEM), it is drawn with oled_print_P
} menu_item_t;

typedef struct {
    const menu_item_t* items;
    uint8_t item_count;
} menu_t;

// Blocks until the user selects a menu item, then returns the index of that item.
uint8_t menu_run(const menu_t* menu, uint8_t font_size);

void start_new_game(void);
void show_settings_menu(void);
void show_high_scores(void);

#endif
