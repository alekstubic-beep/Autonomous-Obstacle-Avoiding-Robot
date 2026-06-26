
#include <p30fxxxx.h>
#include "timer1.h"

void Init_T1(void)
{
    TMR1 = 0;
    PR1 = 65535;

    T1CONbits.TCS = 0;      // interni takt
    T1CONbits.TCKPS = 1;    // preskaler 1:8
    T1CONbits.TON = 1;      // ukljuci timer1
}
