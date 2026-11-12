#ifndef OLED_H_
#define OLED_H_

#include <stdint.h>

// Structure to hold the OLED pin configuration
typedef struct {
    volatile uint8_t *ctrl_port; // Port for D/C#
    volatile uint8_t *ctrl_ddr;  // Data Direction Register for D/C#
    uint8_t dc_pin;              // Data/Command pin number
    
    volatile uint8_t *cs_port;   // Port for Chip Select 
    uint8_t cs_pin;              // Chip Select pin number
} oled_config_t;


void oled_init(const oled_config_t *config);
void oled_goto_line(uint8_t line);
void oled_goto_column(uint8_t column);
void oled_pos(uint8_t line, uint8_t column, uint8_t font_size);
void oled_absolute_pos(uint8_t line, uint8_t column);
void oled_clear_line(uint8_t line);
void oled_clear(void);

// Prints a string stored in RAM (e.g. a buffer filled by sprintf)
void oled_print(const char* str, uint8_t font_size);

// Prints a string stored in flash: oled_print_P(PSTR("text"), 5);
// Use this for every fixed text so it does not occupy the 1 kB of internal RAM.
void oled_print_P(const char* str, uint8_t font_size);

void oled_funny_graphics(void);
void oled_write_cmd(uint8_t cmd);
void oled_write_data(uint8_t data);

#endif /* OLED_H_ */
