/*
 * File:   adc.c
 * GP2Y0A21 IR senzor daljine - AN0 (RB0)
 */

#include <p30fxxxx.h>
#include "adc.h"

void ADCinit(void)
{
    TRISBbits.TRISB0 = 1;   // RB0 = ulaz (AN0)

    ADPCFG = 0xFFFE;        // samo AN0 analogni, ostalo digitalno

    ADCON1bits.ADSIDL = 0;
    ADCON1bits.FORM   = 0;  // integer (0-1023)
    ADCON1bits.SSRC   = 7;  // auto-convert
    ADCON1bits.ASAM   = 0;  // manuelni start
    ADCON1bits.SAMP   = 0;

    ADCON2bits.VCFG   = 0;  // Vref = AVdd/AVss
    ADCON2bits.CSCNA  = 0;  // bez skena
    ADCON2bits.SMPI   = 0;  // interrupt nakon svake konverzije
    ADCON2bits.BUFM   = 0;
    ADCON2bits.ALTS   = 0;

    ADCON3bits.SAMC   = 31;
    ADCON3bits.ADRC   = 1;  // interni RC takt
    ADCON3bits.ADCS   = 31;

    ADCHSbits.CH0NA   = 0;  // negativni = AVss
    ADCHSbits.CH0SA   = 0;  // pozitivni = AN0

    ADCSSL = 0x0000;

    IEC0bits.ADIE = 0;
    IFS0bits.ADIF = 0;

    ADCON1bits.ADON = 1;    // ukljuci ADC
}

unsigned int ADC_ReadAN0(void)
{
    ADCON1bits.SAMP = 1;        // pocni sampling
    ADCON1bits.SAMP = 0;        // zaustavi, pocni konverziju
    while(!ADCON1bits.DONE);    // cekaj kraj
    return ADCBUF0;
}

// GP2Y0A21: cm = 2076 / (adc - 11), vrijedi za 10-80cm
float ADC_to_cm(unsigned int adc_val)
{
    float cm;
    if(adc_val < 15) return -1.0;
    cm = 2076.0 / ((float)adc_val - 11.0);
    if(cm < 10.0 || cm > 40.0) return -1.0;
    return cm;
}
