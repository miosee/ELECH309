/* 
 * File:   morse.h
 * Author: mosee
 *
 * Created on 13 février 2020, 11:51
 */

#ifndef MORSE_H
#define	MORSE_H

#ifdef	__cplusplus
extern "C" {
#endif

    #define DOT_DURATION    150     // Duration of a dot, in ms

    
    // Initialise RB6 as an output.  A LED should be connected to this pin
    void morseInit(void);
    // Send the morse code of a lower case 
    void morseCharToLed(uint8_t letter);
    void morseStrToLed(uint8_t *str);


#ifdef	__cplusplus
}
#endif

#endif	/* MORSE_H */

