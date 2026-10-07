#define F_CPU 84000000UL
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1


#include <stdio.h>
#include <stdarg.h>
#include "sam.h"
#include "../Drivers/uart.h"
#include "../Drivers/can.h"



int main()
{
     ///   SETUP   ///
     SystemInit();
     WDT->WDT_MR = WDT_MR_WDDIS; //Disable Watchdog Timer
     uart_init(F_CPU, BAUD);
     can_init((CanInit){.brp = 104, .phase1 = 6, .phase2 = 5, .propag = 1 , .sjw = 0, .smp = 0}, 0);
     


    while (1)
    {
        CanMsg msg;

        if(can_rx(&msg)){
            can_printmsg(msg);
        }
    }
    
}