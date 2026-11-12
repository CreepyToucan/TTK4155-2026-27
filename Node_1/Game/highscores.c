#include "XMEM.h"
#include "highscores.h"


void highscores_clear(void) {
    for (uint8_t i = 0; i < SRAM_SIZE; i++)
        xmem_write(0, i, BASE_ADDRESS_SRAM);
}

void highscores_init(void) {
    if (xmem_read(0, BASE_ADDRESS_SRAM) != 0x00) {
        highscores_clear();
    }
}

uint8_t highscores_get(uint16_t index) {
    if (index >= SRAM_SIZE) return 0;
    return xmem_read(index, BASE_ADDRESS_SRAM);
}

uint16_t highscores_add(uint8_t score) {
    if (score == 0) return 0;

    uint8_t table[SRAM_SIZE];
    for (uint8_t i = 0; i < SRAM_SIZE; i++)
        table[i] = xmem_read(i, BASE_ADDRESS_SRAM);

    uint16_t pos = 0;
    while (pos < SRAM_SIZE && table[pos] >= score) pos++;   // ties rank below existing
    if (pos == SRAM_SIZE) return 0;

    for (uint8_t i = SRAM_SIZE - 1; i > pos; i--) table[i] = table[i - 1];
    table[pos] = score;

for (uint8_t i = 0; i < SRAM_SIZE; i++)
        xmem_write(table[i],i, BASE_ADDRESS_SRAM);   // only writes changed words
    return pos + 1;
}