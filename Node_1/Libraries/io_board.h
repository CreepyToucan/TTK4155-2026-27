#ifndef IO_BOARD_H_
#define IO_BOARD_H_

#include <stdint.h>

/**
 * @name I/O Board Command Definitions
 * Command bytes for each operation supported by the I/O board in SPI slave mode.
 */
#define IO_CMD_TOUCHPAD 0x01
#define IO_CMD_SLIDER   0x02
#define IO_CMD_JOYSTICK 0x03
#define IO_CMD_BUTTONS  0x04
#define IO_CMD_LED_ON   0x05
#define IO_CMD_LED_PWM  0x06
#define IO_CMD_INFO     0x07
/**  */

/**
 * @brief Configuration struct for the I/O board SPI pin
 */
typedef struct {
    volatile uint8_t *cs_port; /**< Pointer to the Chip Select PORT register */
    uint8_t cs_pin;            /**< Pin number for the Chip Select */
} io_board_config_t;

/**
 * @brief Data structure for reading the touch pad
 */
typedef struct {
    uint8_t x;    /**< X coordinate of the touch */
    uint8_t y;    /**< Y coordinate of the touch */
    uint8_t size; /**< Signal strength of the touch pad, approximately corresponds to the proportion of the pad being touched */
} io_touchpad_t;

/**
 * @brief Data structure for reading the touch slider
 */
typedef struct {
    uint8_t x;    /**< Position on the slider */
    uint8_t size; /**< Signal strength / touch proportion */
} io_slider_t;

/**
 * @brief Data structure for reading the joystick
 */
typedef struct {
    uint8_t x;   /**< X-axis position */
    uint8_t y;   /**< Y-axis position */
    uint8_t btn; /**< Joystick button state */
} io_joystick_t;

/**
 * @brief Data structure for reading the buttons
 */
typedef struct {
    uint8_t right; /**< Right buttons bitmask, MSB to LSB: [0 0 R6 R5 R4 R3 R2 R1] */
    uint8_t left;  /**< Left buttons bitmask, MSB to LSB: [0 L7 L6 L5 L4 L3 L2 L1] */
    uint8_t nav;   /**< Navigation buttons bitmask, MSB to LSB: [0 0 0 Up Down Left Right Btn] */
} io_buttons_t;

/**
 * @brief Data structure for reading board information (35 bytes total)
 */
typedef struct {
    char timestamp[19]; /**< Firmware compile timestamp in the format (YYYY-MM-DDTHH:mm:ss) */
    char serial[16];    /**< AVR serial number */
} io_info_t;

/**
 * @brief Initializes the I/O board driver with the specified hardware pins.
 * @param config Pointer to the configuration struct defining the CS pin.
 */
void io_board_init(const io_board_config_t *config);

/**
 * @brief Reads the current state of the touch pad.
 * @return io_touchpad_t Struct containing X, Y, and touch size.
 */
io_touchpad_t io_getTouchpad(void);

/**
 * @brief Reads the current state of the touch slider.
 * @return io_slider_t Struct containing X and touch size.
 */
io_slider_t io_getSlider(void);

/**
 * @brief Reads the current position and button state of the joystick.
 * @return io_joystick_t Struct containing X, Y, and button state.
 */
io_joystick_t io_getJoy(void);

/**
 * @brief Reads the state of the right, left, and navigation buttons.
 * @return io_buttons_t Struct containing the bitmasks for all buttons.
 */
io_buttons_t io_getButtons(void);

/**
 * @brief Reads the firmware compile timestamp and AVR serial number.
 * @param info Pointer to the io_info_t struct to populate with board info.
 */
void io_getInfo(io_info_t *info);

/**
 * @brief Turns a specific LED on or off.
 * @param led_n The target LED, ranging from 0 to 5.
 * @param on_off A value other than 0 will turn on the LED, 0 will turn it off.
 */
void io_setLed(uint8_t led_n, uint8_t on_off);

/**
 * @brief Sets the PWM brightness of a specific LED.
 * @param led_n The target LED, ranging from 0 to 5.
 * @param width The PWM width where 255 is maximum brightness.
 */
void io_setLed_pwm(uint8_t led_n, uint8_t width);

#endif /* IO_BOARD_H_ */