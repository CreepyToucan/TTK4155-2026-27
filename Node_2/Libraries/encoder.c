#include "encoder.h"

static int32_t encoder_offset = 0;

void encoder_init(void) {
    // Enable peripheral clock for TC2 Channel 0 (TC6, Peripheral ID 33)
    // PMC_PCER1 controls PIDs 32 to 63. We shift by (ID_TC6 - 32).
    PMC->PMC_PCER1 = 1 << (ID_TC6 - 32);

    // Disable PIO control on PC25 and PC26 to allow peripheral ownership
    PIOC->PIO_PDR = PIO_PC25 | PIO_PC26;

    // Switch PC25 (Ch A) and PC26 (Ch B) to Peripheral B (TIOA6 and TIOB6)
    PIOC->PIO_ABSR |= PIO_PC25 | PIO_PC26;

    // Configure TC2 Block Mode Register (TC_BMR):
    // Enable Quadrature Decoder Mode (QDEN)
    // Enable Position Measurement (POSEN)
    TC2->TC_BMR = TC_BMR_QDEN | TC_BMR_POSEN;

    // Configure TC2 Channel 0 Mode Register (TC_CMR):
    // Select XC0 clock source outputted by the Quadrature Decoder logic
    TC2->TC_CHANNEL[0].TC_CMR = TC_CMR_TCCLKS_XC0;

    // Enable counter clock and send a software trigger to initialize the counter
    TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
    
    // Initialize offset to 0
    encoder_offset = 0;
}

int32_t encoder_read(void) {
    // Read the raw hardware counter
    int32_t raw_ticks = (int32_t)TC2->TC_CHANNEL[0].TC_CV;
    
    // Return the position relative to our calibrated zero point
    return raw_ticks - encoder_offset;
}

void encoder_reset(void) {
    // Instead of fighting the hardware reset requirement for clock edges,
    // we simply record the stationary position as our new mathematical zero.
    encoder_offset = (int32_t)TC2->TC_CHANNEL[0].TC_CV;
}