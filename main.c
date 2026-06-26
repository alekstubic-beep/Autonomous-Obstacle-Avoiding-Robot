#include <p30fxxxx.h>
#include <stdio.h>
#include <stdlib.h>
#define FCY 10000000UL
#include <libpic30.h>
#include "timer1.h"
#include "timer2.h"
       
_FOSC(CSW_FSCM_OFF & XT_PLL4);
_FWDT(WDT_OFF);
_FBORPOR(MCLR_EN & PWRT_OFF);
_FGS(CODE_PROT_OFF);

#define PWM_PERIODA  2499

typedef enum {
    STANJE_STOP,
    STANJE_NAPRED,
    SKRENI_LEVO,
    SKRENI_DESNO
} stanje_t;

stanje_t stanje = STANJE_STOP;
unsigned char tempRX;
unsigned int broj1, broj2;

// U1RX = RF2 (ulaz)
// U1TX = RF3 (izlaz)
unsigned char tempRX;
char buf[6];
int n = 0;
void korekcija_pravca(void);
float meri_lijevi_cm(void);
float meri_desni_cm(void);


void initUART1(void)
{
    TRISFbits.TRISF3 = 0;
    TRISFbits.TRISF2 = 1;
    U1BRG = 0x0040;
    U1MODEbits.ALTIO = 0;
    IEC0bits.U1RXIE = 0;
    U1STA &= 0xfffc;
    U1MODEbits.UARTEN = 1;
    U1STAbits.UTXEN = 1;
}

void __attribute__((__interrupt__, no_auto_psv)) _U1RXInterrupt(void)
{
    IFS0bits.U1RXIF = 0;
    tempRX = U1RXREG;
}

void WriteUART1(unsigned int data)
{
    while(!U1STAbits.TRMT);
    if(U1MODEbits.PDSEL == 3)
        U1TXREG = data;
    else
        U1TXREG = data & 0xFF;
}

void WriteStringUART1(const char* str)
{
    while(*str) { WriteUART1((unsigned int)*str); str++; }
}

void motori_init(void)
{
    TRISCbits.TRISC13 = 0; TRISCbits.TRISC14 = 0;
    TRISDbits.TRISD2  = 0; TRISDbits.TRISD3  = 0;
    TRISDbits.TRISD0  = 0; TRISDbits.TRISD1  = 0;
    LATDbits.LATD0 = 1; LATDbits.LATD1 = 1;
    LATCbits.LATC13 = 0; LATCbits.LATC14 = 0;
    LATDbits.LATD2  = 0; LATDbits.LATD3  = 0;
    TRISBbits.TRISB12 = 0;
}

void pwm_init(void)
{
    TRISDbits.TRISD0 = 0;
    TRISDbits.TRISD1 = 0;

    OpenTimer2(T2_ON & T2_IDLE_CON & T2_GATE_OFF & T2_PS_1_1 & T2_SOURCE_INT, PWM_PERIODA);

    OC1R = 0;
    OC1RS = 0;
    OC1CONbits.OCTSEL = 0;
    OC1CONbits.OCM = 0b110;

    OC2R = 0;
    OC2RS = 0;
    OC2CONbits.OCTSEL = 0;
    OC2CONbits.OCM = 0b110;
}

void postavi_pwm(unsigned int levi, unsigned int desni)
{
    OC1RS = levi;
    OC2RS = desni;
}

void motori_stop(void)
{
    postavi_pwm(0,0);
    LATCbits.LATC13 = 0; LATCbits.LATC14 = 0;
    LATDbits.LATD2  = 0; LATDbits.LATD3  = 0;
    LATBbits.LATB12 = 0;
}

void motori_naprijed(void)
{
 
    LATCbits.LATC13 = 1;
    LATCbits.LATC14 = 0;
    LATDbits.LATD2  = 1;
    LATDbits.LATD3  = 0;
    postavi_pwm(2000,2000);
}

void skreni_levo(void)
{
    LATCbits.LATC13 = 0;
    LATCbits.LATC14 = 1;

    LATDbits.LATD2 = 1;
    LATDbits.LATD3 = 0;
    postavi_pwm(2000,2000);
}

void skreni_desno(void)
{
    LATCbits.LATC13 = 1;
    LATCbits.LATC14 = 0;

    LATDbits.LATD2 = 0;
    LATDbits.LATD3 = 1;
    postavi_pwm(2000,2000);
}

void korekcija_pravca(){
    float  trenutno = meri_lijevi_cm();
    float  prethodno = 10000;
    if(trenutno > 4){
        WriteStringUART1("Korigujem pravac\r\n");
        while(prethodno > trenutno){
            prethodno = trenutno;
            skreni_levo();
            __delay_ms(100);
            motori_naprijed();
            __delay_ms(100);
            motori_stop();
            __delay_ms(1000);
            trenutno = meri_lijevi_cm();
        }
    }
   
}

// LIJEVI senzor (P6): TRIG=RF0, ECHO=RD9
// DESNI  senzor (P5): TRIG=RF1, ECHO=RD8

float meri_lijevi_cm(void)
{
    unsigned int t;

    LATFbits.LATF0 = 0;
    __delay_us(2);
    LATFbits.LATF0 = 1;
    __delay_us(10);
    LATFbits.LATF0 = 0;

    TMR1 = 0;
    while(PORTDbits.RD9 == 0)
    {
        if(TMR1 > 60000) return -1.0;
    }

    TMR1 = 0;
    while(PORTDbits.RD9 == 1)
    {
        if(TMR1 > 60000) return -1.0;
    }

    t = TMR1;
    return 0.01372 * t;
}

float meri_napred_cm(void)
{
    unsigned int t;

    LATFbits.LATF1 = 0;
    __delay_us(2);
    LATFbits.LATF1 = 1;
    __delay_us(10);
    LATFbits.LATF1 = 0;

    TMR1 = 0;
    while(PORTDbits.RD8 == 0)
    {
        if(TMR1 > 60000) return -1.0;
    }

    TMR1 = 0;
    while(PORTDbits.RD8 == 1)
    {
        if(TMR1 > 60000) return -1.0;
    }

    t = TMR1;
    return 0.01372 * t;
}

int main(int argc, char** argv)
{
    ADPCFG = 0xFFFF;
    // LIJEVI senzor (P6)
    TRISFbits.TRISF0 = 0;
    LATFbits.LATF0 = 0;
    TRISDbits.TRISD9 = 1;
    // DESNI senzor (P5)
    TRISFbits.TRISF1 = 0;
    LATFbits.LATF1 = 0;
    TRISDbits.TRISD8 = 1;
    Init_T1();
    initUART1();
    motori_init();
    pwm_init();
    char c;
    __delay_ms(2000);
    WriteStringUART1("Spreman!\r\n");

    stanje_t next;
    stanje = STANJE_NAPRED;
    unsigned char autonomno = 0;

    while(1)
    {
        if(U1STAbits.URXDA)
        {
            c = U1RXREG;
            if(c == 's' || c == 'S')
            {
                autonomno = 1;
                stanje = STANJE_NAPRED;
                WriteStringUART1("Autonomni mod: START\r\n");
            }
            else if(c == 'x' || c == 'X')
            {
                autonomno = 0;
                motori_stop();
                WriteStringUART1("Autonomni mod: STOP\r\n");
            }
        }

        if(!autonomno) continue;  
        float dist_lijevi = meri_lijevi_cm();
        float dist_napred  = meri_napred_cm();
       
        char buffer[64];
        sprintf(buffer, "L: %.2f cm | N: %.2f cm\r\n", dist_lijevi, dist_napred);
        WriteStringUART1(buffer);

        switch(stanje)
        {
            case STANJE_NAPRED:
                WriteStringUART1("Vozim napred\r\n");
                next = stanje;
                motori_naprijed();
                if (dist_napred < 12) { next = STANJE_STOP; }
                if(dist_lijevi > 22) { next = STANJE_STOP; }
                break;

              case STANJE_STOP:
                WriteStringUART1("Stajem\r\n");
                next = stanje;
                motori_stop();
                __delay_ms(1000);
                if( dist_lijevi > 22) { next = SKRENI_LEVO; }
                else { next = SKRENI_DESNO; }
                break;

            case SKRENI_LEVO:
                WriteStringUART1("Skrecem levo\r\n");
                next = stanje;
                skreni_levo();
                __delay_ms(1100); //bilo 1000
                motori_naprijed();
                __delay_ms(700);
                motori_stop();
                __delay_ms(1000);
                korekcija_pravca();
                next = STANJE_NAPRED;
                break;

            case SKRENI_DESNO:
                WriteStringUART1("Skrecem desno\r\n");
                next = stanje;
                skreni_desno();
                __delay_ms(900); //bilo 1100
                motori_stop();
                __delay_ms(1200); //bilo 1000
                korekcija_pravca();
                korekcija_pravca();
                next = STANJE_NAPRED;
                break;

            default:
                next = stanje;
                stanje = STANJE_STOP;
                break;
        }
        stanje = next;
    }
    return (EXIT_SUCCESS);
}