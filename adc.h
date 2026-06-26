/* 
 * File:   adc.h
 * GP2Y0A21 IR senzor - AN0 (RB0)
 */

#ifndef ADC_H
#define ADC_H

#include <p30fxxxx.h>

void ADCinit(void);
unsigned int ADC_ReadAN0(void);
float ADC_to_cm(unsigned int adc_val);

#endif

