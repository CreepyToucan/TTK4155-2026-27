#include <avr/eeprom.h>
#include "highscores.h"

#define HS_MAGIC 0xA5   // marks the EEPROM as initialised (blank EEPROM reads 0xFF)

static uint8_t  EEMEM ee_magic;
static uint16_t EEMEM ee_scores[HS_COUNT];

void highscores_clear(void) {
    for (uint8_t i = 0; i < HS_COUNT; i++)
        eeprom_update_word(&ee_scores[i], 0);
    eeprom_update_byte(&ee_magic, HS_MAGIC);
}

void highscores_init(void) {
    if (eeprom_read_byte(&ee_magic) != HS_MAGIC)
        highscores_clear();
}

uint16_t highscores_get(uint8_t index) {
    if (index >= HS_COUNT) return 0;
    return eeprom_read_word(&ee_scores[index]);
}

uint8_t highscores_add(uint16_t score) {
    if (score == 0) return 0;

    uint16_t table[HS_COUNT];
    for (uint8_t i = 0; i < HS_COUNT; i++)
        table[i] = eeprom_read_word(&ee_scores[i]);

    uint8_t pos = 0;
    while (pos < HS_COUNT && table[pos] >= score) pos++;   // ties rank below existing
    if (pos == HS_COUNT) return 0;

    for (uint8_t i = HS_COUNT - 1; i > pos; i--) table[i] = table[i - 1];
    table[pos] = score;

    for (uint8_t i = 0; i < HS_COUNT; i++)
        eeprom_update_word(&ee_scores[i], table[i]);   // only writes changed words
    return pos + 1;
}
