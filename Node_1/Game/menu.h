#ifndef MENU_H
#define MENU_H

#include <stdint.h>
#include <avr/io.h>

// External declarations for game configuration
// 0 = Easy, 1 = Medium, 2 = Hard
extern uint8_t difficulty;
extern const uint8_t lives_per_difficulty[3];
extern const uint8_t score_multiplier[3];

typedef struct {
    const char* label;
} menu_item_t;

typedef struct {
    const menu_item_t* items;
    uint8_t item_count;
} menu_t;

// Blocks until the user selects a menu item, then returns the index of that item.
uint8_t menu_run(const menu_t* menu, uint8_t font_size);

// Core menu and UI functions
void show_settings_menu(void);
void show_high_scores(void);

// void start_new_game(void);

#endif // MENU_H