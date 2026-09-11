#define F_CPU 4915200UL
#define BAUD 9600     
#define MYUBRR (F_CPU/16/BAUD-1) 

#include <avr/io.h>
#include <util/delay.h>

// Funzione per inizializzare la USART0 (Pin 10 e 11)
void USART0_Init(unsigned int ubrr) {
    // Imposta il baud rate
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;
    
    // Abilita la trasmissione (TX sul pin 11)
    UCSR0B = (1 << TXEN0);
    
    // Imposta il formato del frame: 8 bit di dati, 1 bit di stop
    // URSEL0 a 1 è richiesto sull'ATmega162 per scrivere in UCSR0C
    UCSR0C = (1 << URSEL0) | (1 << UCSZ01) | (1 << UCSZ00);
}

// Funzione per inviare un singolo carattere tramite USART0
void USART0_Transmit(unsigned char data) {
    // Aspetta finché il buffer di trasmissione non è vuoto
    while (!(UCSR0A & (1 << UDRE0))); // Attende
        
    // Scrive il dato nel registro per inviarlo
    UDR0 = data;
}

// Funzione per inviare una stringa completa
void USART0_SendString(const char* testo) {
    while (*testo) {
        USART0_Transmit(*testo++);
    }
}

int main(void) {
    // Inizializza la Seriale 0
    USART0_Init(MYUBRR);

    while (1) {
        
        USART0_SendString("A"); 
        
        _delay_ms(1000); 
    }
    
    return 0;
}