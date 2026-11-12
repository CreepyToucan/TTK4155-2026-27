#define F_CPU 4915200UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "Timer.h"

static volatile uint32_t ms_ticks = 0;

void timer_init(void) {
    OCR0   = (F_CPU / (64UL * 1000UL)) - 1;   // = 75 -> one interrupt per 76 counts
    TIMSK |= (1 << OCIE0);
    TCCR0  = (1 << WGM01) | (1 << CS01) | (1 << CS00);  // CTC, prescaler 64
}

ISR(TIMER0_COMP_vect) {
    // 4.9152 MHz / 64 / 76 gives an interrupt every 0.9896 ms (= 760/768 ms), not 1 ms.
    // Each interrupt adds 760/768 of a millisecond; a tick is counted whenever a whole
    // millisecond has accumulated, so about 1 interrupt in 96 does not count a tick.
    static uint16_t acc = 0;
    acc += 760;
    if (acc >= 768) {
        acc -= 768;
        ms_ticks++;
    }
}

uint32_t timer_ms(void) {
    uint32_t t;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { t = ms_ticks; }   // 32-bit read is not atomic on AVR
    return t;
}