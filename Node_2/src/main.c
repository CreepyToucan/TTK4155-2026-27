#define F_CPU 4915200UL
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

    //Uncomment after including uart above
    //uart_init(/*cpufreq*/, /*baud*/);
    //printf("Hello World\n\r");

    while (1)
    {
        /* code */
    }
    
}