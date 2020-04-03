/*****************************************************************************
 * 
 * Demo of the AMB2300 library, created the 02/04/2020
 * 
 * The AMB2300 is the bluetooth-serial interface on the main board of the
 * ELECH309 robot.µ
 * This demo was written for this board. It sends a sine wave period through
 * the bluetooth connexion to the host (i.e. a PC).
 * 
 * It waits that a host sends 2 uint16_t parameters through the bluetooth
 * connexion.
 *      - fisrt, the number of samples to send
 *      - second, the amplitude of the sine wave
 * It answers by sending the sine period samples at 100Hz. the samples are
 * numbers.
 * A Python script (in the 'Python' folder) can be used for the host part.
 * 
 * V1.0 : creation
 * 
 *****************************************************************************/ 

#include "xc.h"
#include "math.h"
#include "../libs/init.h"
#include "../libs/bluetooth.h"

#define SAMPLE_NB_MAX   10000
#define AMPLITUDE_MAX   30000
#define PI              3.14159



int main(void) {
    uint16_t sampleNb, amplitude;
    uint16_t i;
    float sample;
    
    // Initialise the main oscillator to run at Fosc=80MHz => Fcy=40MHz
    oscillatorInit();
    // Initialise the bluetooth communication
    btInit();

    // we use TIMER2 as sampling clock
    T2CONbits.TCKPS = 1;    // prescaler 1:8 => 1ms <=> 5000 ticks
    PR2 = 4999;            // T = 1ms

    while(1) {
        /* Wait for the parameters from the PC :
         * the number of samples and the amplitudes of the sine wave.
         * We know that a uint16_t variable is 2 bytes long */
        while (btGetRxBufferSpace() < 4);
        /* When there is the right number of bytes in the buffer, we read the
         * parameters.
         * The first parameter is a pointer to the variable where the value
         * will be stored, cast as a pointer to a int8_t variable.
         * The function will see the variable as a int8_ vector.
         * The second parameter is the number of bytes to read.
         * We choose to use sizeof(). It gives the size of any variable type*/ 
        btGetData((int8_t*)&sampleNb, sizeof(uint16_t));
        if (sampleNb > SAMPLE_NB_MAX) {     // we check the value's validity
            sampleNb = SAMPLE_NB_MAX;
        }
        btGetData((int8_t*)&amplitude, sizeof(uint16_t));
        if (amplitude > AMPLITUDE_MAX) {
            amplitude = AMPLITUDE_MAX;
        }

        T2CONbits.TON = 1;
        for(i=0; i<sampleNb; i++) {
            while (IFS0bits.T2IF == 0);
            IFS0bits.T2IF = 0;
            sample = amplitude*sin(2*PI*i/sampleNb);
            /* send the sample.
             * The fisrt paramter is a pointer to the float
             * variable, cast as a pointer to a int8_t variable.  The function
             * will see the float variable as a int8_ vector.
             * The second parameter is the number of bytes to send. */ 
            btSendData((int8_t*)&sample, sizeof(sample));
        }
    }
    
    return 0;
}
