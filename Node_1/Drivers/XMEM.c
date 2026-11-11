#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "XMEM.h"

// Functions for using external memory.
// The test messages are stored in flash (printf_P + PSTR): as normal string literals
// they used about 370 of the 1024 bytes of internal RAM even though the tests are never called.

void xmem_init ( void ){
	MCUCR |= (1 << SRE ); // enable XMEM
	SFIOR |= (1 << XMM2 ); // mask unused bits
}

// Write a byte to external memory at a given address offset and base address.
void xmem_write ( uint8_t data , uint16_t addr, uint16_t base){
	volatile char * ext_mem = ( char *) base ;
	ext_mem [ addr ]= data ;
}
// Read a byte from external memory at a given address offset and base address.
uint8_t xmem_read ( uint16_t addr, uint16_t base ){
	volatile char * ext_mem = ( char *) base;
	uint8_t ret_val = ext_mem [ addr ];
	return ret_val ;
}


void SRAM_test(void)
{
    volatile char *ext_ram = (char *) BASE_ADDRESS_SRAM; // Start address for the SRAM
    uint16_t ext_ram_size = 0xC00;
    uint16_t write_errors = 0;
    uint16_t retrieval_errors = 0;
    printf_P(PSTR("Starting SRAM test...\n"));
    // rand() stores some internal state, so calling this function in a loop will
    // yield different seeds each time (unless srand() is called before this function)
    uint16_t seed = rand();
    // Write phase: Immediately check that the correct value was stored
    srand(seed);
    for (uint16_t i = 0; i < ext_ram_size; i++) {
        uint8_t some_value = rand();
        ext_ram[i] = some_value;
        uint8_t retreived_value = ext_ram[i];
        if (retreived_value != some_value) {
            printf_P(PSTR("Write phase error: ext_ram[%4d] = %02X (should be %02X)\n"), i, retreived_value, some_value);
            write_errors++;
        }
    }
    // Retrieval phase: Check that no values were changed during or after the write phase
    srand(seed);
    // reset the PRNG to the state it had before the write phase
    for (uint16_t i = 0; i < ext_ram_size; i++) {
        uint8_t some_value = rand();
        uint8_t retreived_value = ext_ram[i];
        if (retreived_value != some_value) {
            printf_P(PSTR("Retrieval phase error: ext_ram[%4d] = %02X (should be %02X)\n"), i, retreived_value, some_value);
            retrieval_errors++;
        }
    }
    printf_P(PSTR("SRAM test completed with \n%4d errors in write phase and \n%4d errors in retrieval phase\n\n"), write_errors, retrieval_errors);
}


void SRAM_single_test(void) {

    volatile uint8_t *single_address = (uint8_t *) 0x1600; 
    
    uint8_t test_value = 0x1A; 
    
    printf_P(PSTR("Testing single address 0x1600...\n"));

    *single_address = test_value;
    
    uint8_t read_value = *single_address;
    printf_P(PSTR("Read value from address 0x1600: %02X\n"), read_value);
    
    if (read_value == test_value) {
        printf_P(PSTR("SUCCESS: Wrote %02X, Read %02X\n\n"), test_value, read_value);
    } else {
        printf_P(PSTR("FAILURE: Wrote %02X, but Read %02X\n\n"), test_value, read_value);
    }
       
}
