#ifndef MCP2515_H_
#define MCP2515_H_

#include <stdint.h>

/**
 * @name MCP2515 SPI Instruction Set
 * Command bytes for the MCP2515 SPI interface.
 * @{
 */
#define MCP_RESET       0xC0 /**< 1100 0000: Resets internal registers to default state, set Configuration mode */
#define MCP_READ        0x03 /**< 0000 0011: Read data from register beginning at selected address */
#define MCP_WRITE       0x02 /**< 0000 0010: Write data to register beginning at selected address */
#define MCP_RTS_BASE    0x80 /**< 1000 0nnn: Base command to instruct controller to begin message transmission */
#define MCP_READ_STATUS 0xA0 /**< 1010 0000: Quick polling command that reads status bits for TX/RX functions */
#define MCP_BIT_MODIFY  0x05 /**< 0000 0101: Allows the user to set or clear individual bits in a particular register */


/**
 * @name RTS Transmit Buffer Masks
 * Bitmasks to select which transmit buffers to send via the RTS command.
 */
#define MCP_RTS_TXB0 0x01 /**< Request-to-send for TXB0 */
#define MCP_RTS_TXB1 0x02 /**< Request-to-send for TXB1 */
#define MCP_RTS_TXB2 0x04 /**< Request-to-send for TXB2 */
/** @} */

/**
 * @brief Configuration struct for the CAN controller SPI pins.
 */
typedef struct {
    volatile uint8_t *cs_port; /**< Pointer to the Chip Select PORT register */
    uint8_t cs_pin;            /**< Pin number for the Chip Select */
} mcp2515_config_t;

/**
 * @brief Initializes the CAN driver with the specified SPI hardware pins.
 * 
 * @param config Pointer to the configuration struct defining the CS pin.
 */
void mcp2515_init(const mcp2515_config_t *config);

/**
 * @brief Resets the MCP2515 internal registers to their default state and sets Configuration mode.
 */
void mcp2515_reset(void);

/**
 * @brief Reads data from the register at the selected address.
 * 
 * @param address The 8-bit register address to read from.
 * @return uint8_t The data byte stored at the specified address.
 */
uint8_t mcp2515_read(uint8_t address);

/**
 * @brief Writes data to the register at the selected address.
 * 
 * @param address The 8-bit register address to write to.
 * @param data The data byte to write.
 */
void mcp2515_write(uint8_t address, uint8_t data);

/**
 * @brief Instructs the controller to begin the message transmission sequence for any of the transmit buffers.
 * 
 * @param tx_buffers A bitmask indicating which transmit buffer(s) are enabled to send (e.g., MCP_RTS_TXB0).
 */
void mcp2515_request_to_send(uint8_t tx_buffers);

/**
 * @brief Quick polling command that reads several status bits for transmit and receive functions.
 * 
 * @return uint8_t An 8-bit status byte returning TX/RX status flags.
 */
uint8_t mcp2515_read_status(void);

/**
 * @brief Sets or clears individual bits in a specific status or control register.
 * 
 * @param address The 8-bit address of the register to modify.
 * @param mask Determines which bits in the register will be allowed to change. A '1' allows a change.
 * @param data Determines the new value for the modified bits. A '1' sets the bit, a '0' clears it.
 */
void mcp2515_bit_modify(uint8_t address, uint8_t mask, uint8_t data);

#endif /* MCP2515_H_ */