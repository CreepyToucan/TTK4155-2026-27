#include "ir.h"
#include "adc.h"

#define FILTER_SIZE      16     // moving average length (power of 2)
#define DEBOUNCE_SAMPLES 20     // consecutive samples needed to change state
#define BLOCK_PERCENT    40     // blocked if filtered < 40% of baseline
#define UNBLOCK_PERCENT  60     // unblocked if filtered > 60% of baseline

static uint16_t samples[FILTER_SIZE];
static uint8_t  index = 0;
static uint32_t sum = 0;

static uint16_t baseline = 0;
static uint16_t thr_block = 0;
static uint16_t thr_unblock = 0;

static uint8_t  blocked = 0;
static uint8_t  event_flag = 0;
static uint16_t debounce_count = 0;

static void filter_push(uint16_t value)
{
    sum -= samples[index];
    samples[index] = value;
    sum += value;
    index = (index + 1) % FILTER_SIZE;
}

uint16_t ir_filtered_value(void)
{
    return (uint16_t)(sum / FILTER_SIZE);
}

void ir_init(void)
{
    adc_init();

    // Fill the filter with real samples
    for (uint8_t i = 0; i < FILTER_SIZE; i++) {
        samples[i] = adc_read_A0();
        sum += samples[i];
        for (volatile uint16_t d = 0; d < 5000; d++);  // let the ADC convert again
    }

    // Calibrate: average over many samples with the beam unblocked
    uint32_t acc = 0;
    for (uint16_t i = 0; i < 256; i++) {
        acc += adc_read_A0();
        for (volatile uint16_t d = 0; d < 5000; d++);
    }
    baseline = (uint16_t)(acc / 256);

    thr_block   = (uint32_t)baseline * BLOCK_PERCENT   / 100;
    thr_unblock = (uint32_t)baseline * UNBLOCK_PERCENT / 100;

    blocked = 0;
    event_flag = 0;
    debounce_count = 0;
}

void ir_update(void)
{
    filter_push(adc_read_A0());
    uint16_t value = ir_filtered_value();

    uint8_t wants_blocked = blocked;
    if (value < thr_block)        wants_blocked = 1;
    else if (value > thr_unblock) wants_blocked = 0;

    if (wants_blocked != blocked) {
        // Only accept the change after it persists for DEBOUNCE_SAMPLES
        if (++debounce_count >= DEBOUNCE_SAMPLES) {
            blocked = wants_blocked;
            debounce_count = 0;
            if (blocked) event_flag = 1;   // a new "beam broken" event
        }
    } else {
        debounce_count = 0;
    }
}

uint8_t ir_beam_blocked(void)
{
    return blocked;
}

uint8_t ir_blocked_event(void)
{
    if (event_flag) {
        event_flag = 0;
        return 1;
    }
    return 0;
}
