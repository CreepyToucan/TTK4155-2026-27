#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>
#include <sam.h>

/**
 * @brief Initializes TC2 Channel 0 on the ATSAM3X8E for Quadrature Decoder Mode.
 *        Configures PC25 (Arduino D5 / TIOA6) and PC26 (Arduino D4 / TIOB6).
 */
void encoder_init(void);

/**
 * @brief Reads the current raw position tick count from TC2 Channel 0.
 * @return Signed 32-bit counter value (TC_CV).
 */
int32_t encoder_read(void);

/**
 * @brief Resets the encoder position counter register to 0.
 */
void encoder_reset(void);

#endif // ENCODER_H