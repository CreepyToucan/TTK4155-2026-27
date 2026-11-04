#ifndef SPI_H_
#define SPI_H_

#include <stdint.h>

// Initialize the SPI bus as Master
void spi_init(void);

// Initialize the slave
/** @param pin: Pin number of the slave select line to be initialized
 */
void slave_init(uint8_t pin);

// Select or deselect a slave device (active low)
/** @param port: Pointer to the port register (e.g., &PORTB)
 *  @param pin: Pin number of the slave select line (e.g., PB1, PB2, etc.)
 *  @param select: 1 to select (activate) the slave, 0 to deselect (deactivate) the slave
 */
void spi_select_slave(volatile uint8_t *port, uint8_t pin, uint8_t select);

// Transmit a single byte
/** @param data: The byte to be transmitted
 */
void spi_write_byte(uint8_t data);

// Receive a single byte
/** @return: The received byte
 */
uint8_t spi_read_byte(void);

// Transmit n bytes from a buffer
/** @param data: Pointer to the buffer containing the bytes to be transmitted
 *  @param n: Number of bytes to transmit
 */
void spi_write(const uint8_t *data, uint16_t n);

// Receive n bytes into a buffer
/** @param buffer: Pointer to the buffer where the received bytes will be stored
 *  @param n: Number of bytes to receive
 */
void spi_read(uint8_t *buffer, uint16_t n);

#endif /* SPI_H_ */