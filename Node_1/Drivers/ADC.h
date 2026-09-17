#ifndef ADC_H
#define ADC_H

#include <avr/io.h>
#include <stdint.h>

#define ADC_BASE_ADDRESS 0x1000 
#define ADC_REG (*(volatile uint8_t *)(ADC_BASE_ADDRESS))

#define BUSY_PORT PINB
#define BUSY_PIN  PB0

void ADC_Init(void);
uint8_t ADC_ReadChannel(uint8_t channel);

#endif