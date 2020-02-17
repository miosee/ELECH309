#include <xc.h>


void ADC_pollingInit(void) {
    AD1CON1bits.AD12B = 1;  // Convertisseur sur 12 bits
    AD1CON3bits.ADCS = 5;   // Diviseur d'horloge de l'adc
    AD1CON1bits.ASAM = 1;   // auto sample active
    AD1CSSLbits.CSS0 = 1;   // l'entree de l'ADC est AN0
    AD1PCFGLbits.PCFG0 = 0; // AN0 en mode analogique
    AD1CON1bits.SSRC = 0;   // Conversions doivent etre lancees en software
    AD1CON1bits.ADON = 1;   // l'ADC est actif
}

// ADC activ� par le debordement du TIMER3 (en hardware))
void ADC_timer3Init(void) {
    AD1CON1bits.AD12B = 1;  // Convertisseur sur 10 bits
    AD1CON3bits.ADCS = 5;   // Diviseur d'horloge de l'adc
    AD1CON1bits.ASAM = 1;   // auto sample active
    AD1CSSLbits.CSS0 = 1;   // l'entree de l'ADC est AN0
    AD1PCFGLbits.PCFG0 = 0; // AN0 en mode analogique
    AD1CON1bits.SSRC = 2;   // Concersions lancees par TIMER3
    AD1CON1bits.ADON = 1;   // l'ADC est actif
}


void adcConversionStart(void) {
    AD1CON1bits.SAMP = 0;
}


int ADC_isConversionFinished(void) {
    return (AD1CON1bits.DONE);
}


int ADC_read(void) {
    if (AD1CON1bits.DONE) {
        return ADC1BUF0;
    } else {
        return (-32768);
    }
}
