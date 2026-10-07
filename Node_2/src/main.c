#define F_CPU 84000000UL
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1


#include <stdio.h>
#include <stdarg.h>
#include "sam.h"
#include "../Drivers/uart.h"



int main()
{
     ///   SETUP   ///




    SystemInit();

    WDT->WDT_MR = WDT_MR_WDDIS; //Disable Watchdog Timer

    
    uart_init(F_CPU, BAUD);

    while (1)
    {
        printf("Hello World\n\r");

    }
    
}