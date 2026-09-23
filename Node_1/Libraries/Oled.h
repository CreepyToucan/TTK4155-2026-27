#ifndef OLED_H_
#define OLED_H_

#include <stdint.h>

// Structure to hold the OLED pin configuration
typedef struct {
    volatile uint8_t *ctrl_port; // Port for D/C# and RES# (e.g., &PORTC)
    volatile uint8_t *ctrl_ddr;  // Data Direction Register for D/C# and RES# (e.g., &DDRC)
    uint8_t dc_pin;              // Data/Command pin number
    
    volatile uint8_t *cs_port;   // Port for Chip Select (e.g., &PORTB)
    uint8_t cs_pin;              // Chip Select pin number
} oled_config_t;


void oled_init(const oled_config_t *config);
void oled_goto_line(uint8_t line);
void oled_goto_column(uint8_t column);
void oled_pos(uint8_t line, uint8_t column);
void oled_clear_line(uint8_t line);
void oled_clear(void);
void oled_print(const char* str);
void oled_funny_graphics(void);
void oled_clear(void);
void oled_write_cmd(uint8_t cmd);
void oled_write_data(uint8_t data);

#endif /* OLED_H_ */