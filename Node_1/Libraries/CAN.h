#ifndef CAN_H_
#define CAN_H_

#include <stdint.h>
#include "MCP2515.h"

// MCP2515 Register Addresses used for CAN operations
#define MCP_TXB0CTRL 0x30
#define MCP_TXB0SIDH 0x31
#define MCP_TXB0SIDL 0x32
#define MCP_TXB0DLC  0x35
#define MCP_TXB0D0   0x36

#define MCP_RXB0CTRL 0x60
#define MCP_RXB0SIDH 0x61
#define MCP_RXB0SIDL 0x62
#define MCP_RXB0DLC  0x65
#define MCP_RXB0D0   0x66

#define MCP_RXB1CTRL 0x70
#define MCP_RXB1SIDH 0x71
#define MCP_RXB1SIDL 0x72
#define MCP_RXB1DLC  0x75
#define MCP_RXB1D0   0x76

#define MCP_CANINTF  0x2C

#define MCP_CANCTRL 0x0F 


/**
 * @brief Available CAN operation modes.
 * Mapped to the REQOP<2:0> bits in the CANCTRL register.
 */
typedef enum {
    CAN_MODE_NORMAL   = 0x00, /**< 000: Normal Operation mode */
    CAN_MODE_LOOPBACK = 0x40  /**< 010: Loopback mode (0x40 = 0100 0000 in binary) */
} can_mode_t;


/**
 * @brief Standard CAN message structure
 */
typedef struct {
    uint16_t id;       /**< 11-bit Standard Identifier */
    uint8_t length;    /**< Data Length Code (0-8) */
    uint8_t data[8];   /**< Message payload */
} can_message_t;

/**
 * @brief Transmits a CAN message using Transmit Buffer 0 (TXB0).
 * 
 * @param msg Pointer to the CAN message to be sent.
 * @return uint8_t 1 if successfully queued, 0 if buffer is busy.
 */
uint8_t CAN_transmit(const can_message_t *msg);

/**
 * @brief Checks for received messages and reads them into the provided struct.
 * 
 * @param msg Pointer to the CAN message struct to populate.
 * @return uint8_t 1 if a message was received, 0 if no message is available.
 */
uint8_t CAN_receive(can_message_t *msg);


/**
 * @brief Sets the operational mode of the MCP2515.
 * 
 * @param mode The desired mode (Normal or Loopback).
 */
void CAN_set_mode(can_mode_t mode);

#endif /* CAN_H_ */