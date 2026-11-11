#define F_CPU 4915200UL

#include "io_board.h"
#include "SPI.h"
#include <avr/io.h>
#include <util/delay.h>

static io_board_config_t io_cfg;

void io_board_init(const io_board_config_t *config) {
    io_cfg = *config;
}

io_touchpad_t io_getTouchpad(void) {
    io_touchpad_t data;
    spi_select_slave(io_cfg.cs_port, io_cfg.cs_pin, 1);

    spi_write_byte(IO_CMD_TOUCHPAD); // Command 0x01
    data.x = spi_read_byte();        // Read X
    data.y = spi_read_byte();        // Read Y
    data.size = spi_read_byte();     // Read size
    
    spi_select_slave(io_cfg.cs_port, io_cfg.cs_pin, 0);
    return data;
}

io_slider_t io_getSlider(void) {
    io_slider_t data;
    spi_select_slave(io_cfg.cs_port, io_cfg.cs_pin, 1);
    
    spi_write_byte(IO_CMD_SLIDER);   // Command 0x02
    data.x = spi_read_byte();        // Read X
    data.size = spi_read_byte();     // Read size
    
    spi_select_slave(io_cfg.cs_port, io_cfg.cs_pin, 0);
    return data;
}

io_joystick_t io_getJoystick(void) {
    io_joystick_t data;
    spi_select_slave(io_cfg.cs_port, io_cfg.cs_pin, 1);
    
    spi_write_byte(IO_CMD_JOYSTICK); // Command 0x03
    data.x = spi_read_byte();        // Read X
    data.y = spi_read_byte();        // Read Y
    data.btn = spi_read_byte();      // Read btn
    
    spi_select_slave(io_cfg.cs_port, io_cfg.cs_pin, 0);
    return data;
}

io_buttons_t io_getButtons(void) {
    io_buttons_t data;
    spi_select_slave(io_cfg.cs_port, io_cfg.cs_pin, 1);
    
    spi_write_byte(IO_CMD_BUTTONS);  // Command 0x04
    _delay_ms(5); // give the IO board time to prepare its response
    data.right = spi_read_byte();    // Read right
    data.left = spi_read_byte();     // Read left
    data.nav = spi_read_byte();      // Read nav
    
    spi_select_slave(io_cfg.cs_port, io_cfg.cs_pin, 0);
    return data;
}

void io_setLed(uint8_t led_n, uint8_t on_off) {
    spi_select_slave(io_cfg.cs_port, io_cfg.cs_pin, 1);
    
    spi_write_byte(IO_CMD_LED_ON);   // Command 0x05
    spi_write_byte(led_n);           // LED_N (0 to 5)
    spi_write_byte(on_off);          // on/off state
    
    spi_select_slave(io_cfg.cs_port, io_cfg.cs_pin, 0);
}

void io_setLed_pwm(uint8_t led_n, uint8_t width) {
    spi_select_slave(io_cfg.cs_port, io_cfg.cs_pin, 1);
    
    spi_write_byte(IO_CMD_LED_PWM);  // Command 0x06
    spi_write_byte(led_n);           // LED_N (0 to 5)
    spi_write_byte(width);           // PWM width (255 for max brightness)
    
    spi_select_slave(io_cfg.cs_port, io_cfg.cs_pin, 0);
}

void io_getInfo(io_info_t *info) {
    spi_select_slave(io_cfg.cs_port, io_cfg.cs_pin, 1);
    
    spi_write_byte(IO_CMD_INFO);     // Command 0x07
    
    // Read first 19 bytes (timestamp)
    for (uint8_t i = 0; i < 19; i++) {
        info->timestamp[i] = spi_read_byte();
    }
    
    // Read last 16 bytes (serial number)
    for (uint8_t i = 0; i < 16; i++) {
        info->serial[i] = spi_read_byte();
    }
    
    spi_select_slave(io_cfg.cs_port, io_cfg.cs_pin, 0);
}
