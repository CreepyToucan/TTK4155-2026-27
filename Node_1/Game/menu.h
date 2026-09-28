#include <avr/io.h>
#ifndef MENU_H
#define MENU_H

typedef struct {
    const char* label;
} menu_item_t;

typedef struct {
    const menu_item_t* items;
    uint8_t item_count;
} menu_t;

// Blocks until the user selects a menu item, then returns the index of that item.
uint8_t menu_run(const menu_t* menu);

#endif
