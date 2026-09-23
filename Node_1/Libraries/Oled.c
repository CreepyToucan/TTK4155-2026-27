#include "Oled.h"
#include "SPI.h"
#include <avr/io.h>
#include <util/delay.h>

// Internal copy of the configuration to be used by all functions
static oled_config_t oled_cfg;

// Minimal 5x8 font array (Space, 'A', 'B', 'C')
const uint8_t font5x8[][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // Space
    {0x7E, 0x11, 0x11, 0x11, 0x7E}, // A
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // B
    {0x3E, 0x41, 0x41, 0x41, 0x22}  // C
};

void oled_write_cmd(uint8_t cmd) {
    *(oled_cfg.ctrl_port) &= ~(1 << oled_cfg.dc_pin); // Pull D/C# LOW for Command
    spi_select_slave(oled_cfg.cs_port, oled_cfg.cs_pin, 1);
    spi_write_byte(cmd);
    spi_select_slave(oled_cfg.cs_port, oled_cfg.cs_pin, 0);
}

void oled_write_data(uint8_t data) {
    *(oled_cfg.ctrl_port) |= (1 << oled_cfg.dc_pin);  // Pull D/C# HIGH for Data
    spi_select_slave(oled_cfg.cs_port, oled_cfg.cs_pin, 1);
    spi_write_byte(data);
    spi_select_slave(oled_cfg.cs_port, oled_cfg.cs_pin, 0);
}

void oled_init(const oled_config_t *config) {
    // Store configuration locally
    oled_cfg = *config;

    // Configure DC and RES pins as outputs
    *(oled_cfg.ctrl_ddr) |= (1 << oled_cfg.dc_pin);

    // SSD1309 Initialization Sequence
    oled_write_cmd(0xAE); // Display OFF (sleep mode)
    oled_write_cmd(0x20); // Set Memory Addressing Mode
    oled_write_cmd(0x02); // 02h = Page Addressing Mode (default)
    oled_write_cmd(0x81); // Set Contrast Control
    oled_write_cmd(0x7F); // Default contrast 127
    oled_write_cmd(0xA4); // Entire Display ON (Resume to RAM content)
    oled_write_cmd(0xA6); // Set Normal Display (0 in RAM = OFF, 1 = ON)
    oled_write_cmd(0xAF); // Display ON in normal mode
}

void oled_clear(void) {
    for (uint8_t page = 0; page < 8; page++) {
        oled_goto_line(page);   // set page address
        oled_goto_column(0);    // reset column address to 0
        for (uint8_t col = 0; col < 128; col++) {
            oled_write_data(0x00);
        }
    }
}

void oled_goto_line(uint8_t line) {
    // In Page Addressing Mode, the GDDRAM is divided into 8 pages (0-7)[cite: 2]
    oled_write_cmd(0xB0 | (line & 0x07)); 
}

void oled_goto_column(uint8_t column) {
    // Command 00h~0Fh specifies the lower nibble of the column start address[cite: 2]
    oled_write_cmd(0x00 | (column & 0x0F)); 
    // Command 10h~1Fh specifies the higher nibble of the column start address[cite: 2]
    oled_write_cmd(0x10 | ((column >> 4) & 0x0F)); 
}

void oled_print(const char* str) {
    while (*str) {
        char c = *str;
        uint8_t font_index = 0; 
        
        if (c >= 'A' && c <= 'C') {
            font_index = c - 'A' + 1;
        }

        for (uint8_t i = 0; i < 5; i++) {
            oled_write_data(font5x8[font_index][i]);
        }
        
        oled_write_data(0x00); // 1-pixel spacing
        str++;
    }
}

void oled_funny_graphics(void) {
    for (uint8_t page = 0; page < 8; page++) {
        oled_goto_line(page);
        oled_goto_column(0);
        for (uint8_t col = 0; col < 128; col++) {
            if ((page % 2) == 0) {
                oled_write_data((col % 2 == 0) ? 0xAA : 0x55); 
            } else {
                oled_write_data((col % 2 == 0) ? 0x55 : 0xAA);
            }
        }
    }
}