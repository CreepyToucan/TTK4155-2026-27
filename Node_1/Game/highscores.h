#ifndef HIGHSCORES_H
#define HIGHSCORES_H
#include <stdint.h>

#define SRAM_SIZE 30  //SRAM size
#define BASE_ADDRESS_SRAM 0x1400 //SRAM base address

void     highscores_init(void);
uint8_t highscores_get(uint16_t index);   // index 0 = best
uint16_t  highscores_add(uint8_t score);  // returns rank 1..SRAM_SIZE, or 0 if not good enough
void     highscores_clear(void);

#endif
