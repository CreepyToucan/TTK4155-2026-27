#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <stdint.h>

// Enum for selecting the specific joystick or pad axis
typedef enum {
    Joy_y = 0,
    Joy_x = 1,
    Pad_y = 2,
    Pad_x = 3
} InputType;

// Normalized values for joy_Y readings
// @return int8_t [-100 ; 0 ; 100]  Down, Middle, Up
int8_t HandleJOY_Y(void);

// Normalized values for joy_X readings
// @return int8_t [-100 ; 0 ; 100]  Left, Middle, Right
int8_t HandleJOY_X(void);

// Normalized values for pad_Y readings
// @return int8_t [0 ; 100]  Down, Up
int8_t HandlePad_Y(void);

// Normalized values for pad_X readings
// @return int8_t [0 ; 100]  Left, Right
int8_t HandlePad_X(void);

// Get the normalized position for a specific input
// @param obj The input axis to read
// @return int8_t Normalized position value
int8_t GetPos(InputType obj);

// Get the current direction for a specific input
// @param obj The input axis to read
// @return char 'U'(Up), 'D'(Down), 'R'(Right), 'L'(Left), 'N'(Neutral), or 'X'(Error)
char GetDir(InputType obj);

#endif // INPUT_HANDLER_H