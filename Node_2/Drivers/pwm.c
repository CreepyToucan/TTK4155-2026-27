#include "sam.h"
#include "pwm.h"


// Pre-define common Arduino Due PWM pins for easy use
// You can find these mappings in the SAM3X8E datasheet
const pwm_pin_config_t DUE_PIN_6 = { PIOC, PIO_PC24, 1, 7, ID_PIOC }; // PC24, Periph B, Ch 7
const pwm_pin_config_t DUE_PIN_7 = { PIOC, PIO_PC23, 1, 6, ID_PIOC }; // PC23, Periph B, Ch 6
const pwm_pin_config_t DUE_PIN_8 = { PIOC, PIO_PC22, 1, 5, ID_PIOC }; // PC22, Periph B, Ch 5
const pwm_pin_config_t DUE_PIN_9 = { PIOC, PIO_PC21, 1, 4, ID_PIOC }; // PC21, Periph B, Ch 4
const pwm_pin_config_t DUE_PIN_21 = { PIOB, PIO_PB13, 1, 1, ID_PIOB };


// Initialize the global PWM clock and the specific pin/channel
void pwm_init(const pwm_pin_config_t* config, uint32_t frequency) {
    // Enable the clock for the specific PIO port
    PMC->PMC_PCER0 = (1 << config->pio_id);
    // Enable the clock for the PWM controller (ID 36)
    PMC->PMC_PCER1 = (1 << (ID_PWM - 32));

    // Configure the PIO multiplexer for the given pin
    config->pio_port->PIO_PDR = config->pin_mask; // Disable PIO control
    
    if (config->peripheral == 1) {
        config->pio_port->PIO_ABSR |= config->pin_mask;  // Select Peripheral B
    } else {
        config->pio_port->PIO_ABSR &= ~config->pin_mask; // Select Peripheral A
    }

    // Configure the PWM Global Clock (Clock A) to 1 MHz
    // Note: If calling this multiple times, Clock A is overwritten
    // This assumes all initialized channels use the same base clock
    PWM->PWM_CLK = PWM_CLK_PREA(0) | PWM_CLK_DIVA(84); // 84MHz / 84 = 1MHz

    // Configure the specific PWM Channel
    // Use Clock A (CPRE_CLKA) and start the waveform high (CPOL)
    PWM->PWM_CH_NUM[config->channel].PWM_CMR = PWM_CMR_CPRE_CLKA | PWM_CMR_CPOL;

    // Set the PWM Period based on the requested frequency
    uint32_t period = 1000000 / frequency;
    PWM->PWM_CH_NUM[config->channel].PWM_CPRD = period;

    // Initialize Duty Cycle to 0
    PWM->PWM_CH_NUM[config->channel].PWM_CDTY = 0;

    // Enable the specific channel
    PWM->PWM_ENA = (1 << config->channel);
}


// Update the duty cycle for a specific configured pin
void pwm_set_duty(const pwm_pin_config_t* config, uint8_t duty_percent) {
    if (duty_percent > 100) duty_percent = 100;

    // Calculate the duty cycle value based on the channel's current period
    uint32_t period = PWM->PWM_CH_NUM[config->channel].PWM_CPRD;
    uint32_t duty_val = (period * duty_percent) / 100;

    // Write to the Update Register to prevent waveform glitching
    PWM->PWM_CH_NUM[config->channel].PWM_CDTYUPD = duty_val;
}


// Updates the duty cycle using microseconds for precise servo control
void pwm_set_pulse_us(const pwm_pin_config_t* config, uint32_t pulse_us) {
    // Ensure pulse_us is within this range
    // The base clock is 1 MHz, so 1 tick = 1 microsecond
    PWM->PWM_CH_NUM[config->channel].PWM_CDTYUPD = pulse_us;
}