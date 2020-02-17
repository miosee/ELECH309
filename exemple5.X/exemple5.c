/*
 * File:   exemple1.c
 * Author: mosee
 *
 * Created on 3 février 2020, 17:34
 */


#include <xc.h>
#include "init.h"
#include "../libs/dac.h"
#include "../libs/morse.h"


#define LENGTH  40  // Length of the sample vector (for the DAC)



int main(void) {
    uint16_t i;
    uint16_t data[LENGTH];
    uint8_t str[] = "sos";
    
    
    // Initialise the main oscillator to run at Fosc=80MHz => Fcy=40MHz
    oscillatorInit();
    
    // SYNTHETISER INITIALISATIONS
    //////////////////////////////
    // Create the sample vector to feed the DAC with a saw-tooth wave
    for (i=0; i<LENGTH; i++) {
        data[i] = 800*i;
    }
    // Set the DAC sampling frequency to 80MHz/256/8 = 39.06kHz
    DAC_init(8);

    // MORSE LED INITIALISATION
    ///////////////////////////
    // Set the push-button pin as an input
    TRISBbits.TRISB5 = 1;
    // Initialise the Morse LED pin as an output
    morseInit();

    i = 0;
    while(1) {
        /* if the DAC needs a new sample, we give ive it the next one in
         * the sample vector */
        if (IFS4bits.DAC1RIF) {
            IFS4bits.DAC1RIF = 0;
            DAC_write(data[i++]);
            if (i >= LENGTH) {
                i = 0;
            }
        }
        // if the button is pressed, we send the morse message
        if (PORTBbits.RB5) {
            morseStrToLed(str);
        }
    }
    
    return 0;
}
