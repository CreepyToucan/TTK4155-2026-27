#include "CAN.h"

void CAN_init(uint8_t cnf1, uint8_t cnf2, uint8_t cnf3) {
    // 1. Put the MCP2515 into Configuration Mode to modify CNF registers
    // REQOP<2:0> = 100 sets Configuration mode
    mcp2515_bit_modify(MCP_CANCTRL, 0xE0, 0x80);

    // 2. Set the synchronization registers (Baud rate, PropSeg, PhaseSeg1, PhaseSeg2)
    mcp2515_write(MCP_CNF1, cnf1);
    mcp2515_write(MCP_CNF2, cnf2);
    mcp2515_write(MCP_CNF3, cnf3);

    // 3. Put the MCP2515 back into Normal Mode to begin communication
    // REQOP<2:0> = 000 sets Normal mode
    mcp2515_bit_modify(MCP_CANCTRL, 0xE0, 0x00);
}



uint8_t CAN_transmit(const can_message_t *msg) {
    // Check if TXB0 is currently pending transmission by reading its status
    // Bit 2 of read_status corresponds to TXB0CNTRL.TXREQ
    uint8_t status = mcp2515_read_status();
    if (status & 0x04) {
        return 0; // Buffer is busy
    }

    // Set the Standard Identifier High (Bits 10:3)
    mcp2515_write(MCP_TXB0SIDH, (uint8_t)(msg->id >> 3));
    
    // Set the Standard Identifier Low (Bits 2:0 mapping to register bits 7:5)
    mcp2515_write(MCP_TXB0SIDL, (uint8_t)((msg->id & 0x07) << 5));

    // Set the Data Length Code (DLC)
    mcp2515_write(MCP_TXB0DLC, msg->length & 0x0F);

    // Load the payload into the data registers (TXB0Dm)
    for (uint8_t i = 0; i < msg->length && i < 8; i++) {
        mcp2515_write(MCP_TXB0D0 + i, msg->data[i]);
    }

    // Initiate transmission for TXB0 using the Request-To-Send instruction
    mcp2515_request_to_send(MCP_RTS_TXB0);
    
    return 1;
}

uint8_t CAN_receive(can_message_t *msg) {
    uint8_t rx_status = mcp2515_read_status();
    uint8_t buffer_addr_sidh = 0;
    uint8_t buffer_addr_d0 = 0;
    uint8_t rx_flag_mask = 0;

    // Determine which buffer has received a message based on the status bits
    // Bit 0 corresponds to RX0IF, Bit 1 corresponds to RX1IF
    if (rx_status & 0x01) {
        buffer_addr_sidh = MCP_RXB0SIDH;
        buffer_addr_d0   = MCP_RXB0D0;
        rx_flag_mask     = 0x01; // RX0IF bit
    } else if (rx_status & 0x02) {
        buffer_addr_sidh = MCP_RXB1SIDH;
        buffer_addr_d0   = MCP_RXB1D0;
        rx_flag_mask     = 0x02; // RX1IF bit
    } else {
        return 0; // No message pending
    }

    // Read the Standard Identifier
    uint8_t sidh = mcp2515_read(buffer_addr_sidh);
    uint8_t sidl = mcp2515_read(buffer_addr_sidh + 1); // SIDL is sequentially next
    
    // Reconstruct the 11-bit ID
    msg->id = ((uint16_t)sidh << 3) | (sidl >> 5);

    // Read Data Length Code
    msg->length = mcp2515_read(buffer_addr_sidh + 4) & 0x0F;

    // Read the payload
    for (uint8_t i = 0; i < msg->length && i < 8; i++) {
        msg->data[i] = mcp2515_read(buffer_addr_d0 + i);
    }

    // Clear the appropriate interrupt flag in CANINTF to allow the next message to be loaded
    mcp2515_bit_modify(MCP_CANINTF, rx_flag_mask, 0x00);

    return 1;
}


void CAN_set_mode(can_mode_t mode) {
    // Modify the REQOP<2:0> bits (bits 7-5, mask 0xE0) in the CANCTRL register
    mcp2515_bit_modify(MCP_CANCTRL, 0xE0, (uint8_t)mode);
}