#include <avr/io.h>
#include "SPI.h"

// ATmega162 SPI Port Definitions
#define SPI_PORT PORTB
#define SPI_DDR  DDRB
#define SS_PIN   PB4
#define MOSI_PIN PB5
#define MISO_PIN PB6
#define SCK_PIN  PB7


void spi_init(void) {
    // Set MOSI, SCK, and SS as output, leaving MISO as input
    SPI_DDR |= (1 << MOSI_PIN) | (1 << SCK_PIN) | (1 << SS_PIN);
    SPI_DDR &= ~(1 << MISO_PIN);
    
    // Set default state of default SS pin to high (deselected)
    SPI_PORT |= (1 << SS_PIN);

    // Enable SPI, set as Master, and configure clock rate to fosc/16
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

void slave_init(uint8_t pin){
    
    DDRB |= (1 << pin);
    PORTB |= (1 << pin);
}

void spi_select_slave(volatile uint8_t *port, uint8_t pin, uint8_t select) {
    if (select) {
        *port &= ~(1 << pin); // Pull CS low to activate slave
    } else {
        *port |= (1 << pin);  // Pull CS high to deactivate slave
    }
}

void spi_write_byte(uint8_t data) {
    // Start transmission by writing to the SPI Data Register
    SPDR = data;
    
    // Wait for the SPI Interrupt Flag (SPIF) to be set, indicating completion
    while (!(SPSR & (1 << SPIF)));
}

uint8_t spi_read_byte(void) {
    // Transmit a dummy byte (e.g., 0xFF) to generate the clock pulses needed for reading
    SPDR = 0xFF;
    
    // Wait for the reception to complete
    while (!(SPSR & (1 << SPIF)));
    
    // Return the data shifted into the data register
    return SPDR;
}

void spi_write(const uint8_t *data, uint16_t n) {
    for (uint16_t i = 0; i < n; i++) {
        spi_write_byte(data[i]);
    }
}

void spi_read(uint8_t *buffer, uint16_t n) {
    for (uint16_t i = 0; i < n; i++) {
        buffer[i] = spi_read_byte();
    }
}