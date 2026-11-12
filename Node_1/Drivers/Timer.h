#ifndef TIMER_H
#define TIMER_H
#include <stdint.h>

// Starts Timer0 as a free-running millisecond time base (we call sei() afterwards).
void timer_init(void);

// Milliseconds since timer_init()
uint32_t timer_ms(void);

#endif
