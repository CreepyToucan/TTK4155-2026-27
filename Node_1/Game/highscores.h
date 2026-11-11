#ifndef HIGHSCORES_H
#define HIGHSCORES_H
#include <stdint.h>

#define HS_COUNT 5

void     highscores_init(void);
uint16_t highscores_get(uint8_t index);   // index 0 = best
uint8_t  highscores_add(uint16_t score);  // returns rank 1..HS_COUNT, or 0 if not good enough
void     highscores_clear(void);

#endif
