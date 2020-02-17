#define FCY 40000000ul
#include <xc.h>
#include <libpic30.h>
#include "morse.h"

#define DOT  1
#define DASH 3

#define MORSE_LED       LATBbits.LATB6
#define MORSE_LED_TRIS  TRISBbits.TRISB6


int8_t morseCode[26][4] = { {DOT, DASH, 0, 0}, {DASH, DOT, DOT, DOT},           // a,b
                            {DASH, DOT, DASH, DOT}, {DOT, 0, 0, 0},             // c,d
                            {DOT, DOT, DASH, DOT}, {DASH, DASH, DOT, 0},        // e,f
                            {DOT, DOT, DOT, DOT}, {DOT, DOT, 0, 0},             // g,h
                            {DOT, DOT, 0, 0},{DOT, DASH, DASH, DASH},           // i,j
                            {DASH, DOT, DASH, 0}, {DOT, DASH, DOT, DOT},        // k,l
                            {DASH, DASH, 0, 0}, {DASH, DOT, 0, 0},              // m,n
                            {DASH, DASH, DASH, 0}, {DOT, DASH, DASH, DOT},      // o,p
                            {DASH, DASH, DOT, DASH}, {DOT, DASH, DOT, 0},       // q,r
                            {DOT, DOT, DOT, 0}, {DASH, 0, 0, 0},                // s,t
                            {DOT, DOT, DASH, 0}, {DOT, DOT, DOT, DASH},         // u,v
                            {DOT, DASH, DASH, 0}, {DASH, DOT, DOT, DASH},       // w,x
                            {DASH, DOT, DASH, DASH}, {DASH, DASH, DOT, DOT} };  // y,z


void morseInit(void) {
    MORSE_LED_TRIS = 0;
}


void morseCharToLed(uint8_t letter) {
    uint16_t i = 0, tmp;
    
    letter -= 'a';
    while( (i < 4) && (morseCode[letter][i] != 0) ) {
        MORSE_LED = 1;
        tmp = DOT_DURATION*morseCode[letter][i];
          __delay_ms(DOT_DURATION*morseCode[letter][i]);
        MORSE_LED = 0;
        __delay_ms(DOT_DURATION);
        i++;
    }
}


void morseStrToLed(uint8_t *str) {
    uint16_t i = 0;
    
    while( (i < 20) && (str[i] != '\0') ) {
        if ( (str[i] >= 'a') && (str[i] <= 'z') ) {
            morseCharToLed(str[i]);
        }
        else if (str[i] == ' ') {
            __delay_ms(DOT_DURATION*4);
        }
        __delay_ms(DOT_DURATION*3);
        i++;
    }
}
