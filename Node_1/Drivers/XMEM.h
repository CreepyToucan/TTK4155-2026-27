#ifndef XMEM_H_
#define XMEM_H_

#include <stdint.h>

#define BASE_ADDRESS_ADC 0x1000
#define BASE_ADDRESS_SRAM 0x1400

void xmem_init(void);
void xmem_write(uint8_t data, uint16_t addr, uint16_t base);
uint8_t xmem_read(uint16_t addr, uint16_t base);
void SRAM_test(void);
void SRAM_single_test(void);

#endif