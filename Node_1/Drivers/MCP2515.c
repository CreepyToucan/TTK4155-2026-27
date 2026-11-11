#define F_CPU 4915200UL
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

#include "MCP2515.h"
#include "SPI.h"
#include <avr/io.h>
#include <util/delay.h>

static mcp2515_config_t mcp_cfg;

void mcp2515_init(const mcp2515_config_t *config) {
    mcp_cfg = *config;
}

void mcp2515_reset(void) {
    spi_select_slave(mcp_cfg.cs_port, mcp_cfg.cs_pin, 1);
    
    // The RESET instruction is a single-byte instruction
    spi_write_byte(MCP_RESET); 
    
    spi_select_slave(mcp_cfg.cs_port, mcp_cfg.cs_pin, 0);
    
    // Wait for the oscillator start-up timer
    _delay_ms(10); 
}

uint8_t mcp2515_read(uint8_t address) {
    uint8_t data;
    spi_select_slave(mcp_cfg.cs_port, mcp_cfg.cs_pin, 1);
    
    // Send READ instruction followed by the 8-bit address
    spi_write_byte(MCP_READ);
    spi_write_byte(address);
    
    // Read the data stored in the register
    data = spi_read_byte();
    
    spi_select_slave(mcp_cfg.cs_port, mcp_cfg.cs_pin, 0);
    return data;
}

void mcp2515_write(uint8_t address, uint8_t data) {
    spi_select_slave(mcp_cfg.cs_port, mcp_cfg.cs_pin, 1);
    
    // Send WRITE instruction followed by the address and at least one byte of data
    spi_write_byte(MCP_WRITE);
    spi_write_byte(address);
    spi_write_byte(data);
    
    spi_select_slave(mcp_cfg.cs_port, mcp_cfg.cs_pin, 0);
}

void mcp2515_request_to_send(uint8_t tx_buffers) {
    spi_select_slave(mcp_cfg.cs_port, mcp_cfg.cs_pin, 1);
    
    // The last 3 bits of this command indicate which transmit buffer(s) are enabled to send
    spi_write_byte(MCP_RTS_BASE | (tx_buffers & 0x07));
    
    spi_select_slave(mcp_cfg.cs_port, mcp_cfg.cs_pin, 0);
}

uint8_t mcp2515_read_status(void) {
    uint8_t status;
    spi_select_slave(mcp_cfg.cs_port, mcp_cfg.cs_pin, 1);
    
    // Once the command byte is sent, the MCP2515 will return eight bits of data that contain the status
    spi_write_byte(MCP_READ_STATUS);
    status = spi_read_byte();
    
    spi_select_slave(mcp_cfg.cs_port, mcp_cfg.cs_pin, 0);
    return status;
}

void mcp2515_bit_modify(uint8_t address, uint8_t mask, uint8_t data) {
    spi_select_slave(mcp_cfg.cs_port, mcp_cfg.cs_pin, 1);
    
    // Command is followed by the address of the register, the mask byte, and finally the data byte
    spi_write_byte(MCP_BIT_MODIFY);
    spi_write_byte(address);
    
    // A '1' in the mask byte will allow a bit in the register to change
    spi_write_byte(mask); 
    
    // A '1' in the data byte will set the bit and a '0' will clear the bit
    spi_write_byte(data); 
    
    spi_select_slave(mcp_cfg.cs_port, mcp_cfg.cs_pin, 0);
}