#include <xc.h>

void DAC_init(uint16_t freqDiv) {
    DAC1CONbits.FORM = 0;               // Data Format is Unsigned
    ACLKCONbits.APSTSCLR = 6;           // FVCO = 80MHz => ACLK = 40MHz
    DAC1CONbits.DACFDIV = freqDiv - 1;  // ACLK = 40MHz => Fs = (40MHZ/256)/freqDiv
    DAC1DFLT = 0x8000;                  // Default value 

    DAC1STATbits.ROEN = 1;      // Right Channel DAC Output Enabled
    DAC1STATbits.RITYPE = 0;    // Right Channel Interrupt if FIFO is not Full
    DAC1CONbits.AMPON = 0;      // Amplifier Disabled During Sleep and Idle Modes
    IFS4bits.DAC1RIF = 0;       // Clear Right Channel Interrupt Flag
    // IEC4bits.DAC1RIE = 1;    // Right Channel Interrupt Enabled
   
    DAC1CONbits.DACEN = 1; /* DAC1 Module Enabled */
}


void DAC_write(uint16_t data) {
    DAC1RDAT = data;
}


/*
void __attribute__((interrupt, no_auto_psv))_DAC1RInterrupt(void) {
    IFS4bits.DAC1RIF = 0;
}
*/
