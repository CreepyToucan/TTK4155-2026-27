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
// @return float [-1 ; 0 ; 1]  Down, Middle, Up
float HandleJOY_Y(void);

// Normalized values for joy_X readings
// @return float [-1 ; 0 ; 1]  Left, Middle, Right
float HandleJOY_X(void);

// Normalized values for pad_Y readings
// @return float [0 ; 1]  Down, Up
float HandlePad_Y(void);

// Normalized values for pad_X readings
// @return float [0 ; 1]  Left, Right
float HandlePad_X(void);

// Get the normalized position for a specific input
// @param obj The input axis to read
// @return float Normalized position value
float GetPos(InputType obj);

// Get the current direction for a specific input
// @param obj The input axis to read
// @return char 'U'(Up), 'D'(Down), 'R'(Right), 'L'(Left), 'N'(Neutral), or 'X'(Error)
char GetDir(InputType obj);

#endif // INPUT_HANDLER_H