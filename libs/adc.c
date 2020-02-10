#include <xc.h>
#include "adc.h"


void ADC_Init(void) {
    AD1CON1bits.AD12B = 1;  // Convertisseur sur 12 bits
    AD1CON3bits.ADCS = 5;   // Diviseur d'horloge de l'adc
    AD1CON1bits.ASAM = 1;   // auto sample active
    AD1CSSLbits.CSS0 = 1;   // l'entree de l'ADC est AN0
    AD1PCFGLbits.PCFG0 = 0; // AN0 en mode analogique
    AD1CON1bits.ADON = 1;   // l'ADC est actif
}



void adcConversionStart(void) {
    AD1CON1bits.SAMP = 0;
}


uint16_t ADC_conversionFinished(void) {
    return (AD1CON1bits.DONE);
}


int16_t ADC_read(void) {
    if (AD1CON1bits.DONE) {
        return ADC1BUF0;
    } else {
        return (-32768);
    }
}
