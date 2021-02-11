/*
 * Main file of gpio4.X
 * Author: mosee
 *
 * A LED is connected to RB15 and a push-button is connected to RB14.
 * When the button is pressed, the LED is turned on for 5s.
 * If the button is pressed again during this 5s perido, this period is reset.
 * For eample, if the button is pressed again after 2s, the LED will stay on
 * during 7s. 
 */

/* We will use a software timer: a variable is incremented at a given period
 * (1ms in our case). It allows us to split a long delay in smaller ones.
 * The main loop is executed once between each small delay, allowing it to
 * react to events (the button pressing).
 * Here, the execution time of the main loop is neglected. */

#define FCY 3685000     // µcycle frequency. Needed for __delay_ms
#include "libpic30.h"   // Contains __delay_ms definition
#include "xc.h"         // Contains register variable definitions

#define ON_TIME 5000    // LED on-state duration, in ms


int main(void) {
    int timer;  // Time elapsed since the last button pressed, in ms
    
    // Setup
    TRISBbits.TRISB15 = 0;  // Configure RB15 as a digital output pin
    TRISBbits.TRISB14 = 1;  // Configure RB14 as a digital input pin
    timer = ON_TIME;

    // Main (infinite) loop
    while(1) {
        /* Button handling:
         * If the button is pressed, the LED is switch on
         * and the timer is reset */
        if (PORTBbits.RB14) {
            LATBbits.LATB15 = 1;
            timer = 0;
        }
        
        /* Timer handling:
         * If the timer has not reached its goal, it's incremented
         * and we wait for 1 period */
        if (timer < ON_TIME) {
            timer++;
            __delay_ms(1);
        } else {
            LATBbits.LATB15 = 0;
        }
    }
    
    return 0;
}

