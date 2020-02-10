/*
 * File:   exemple1.c
 * Author: mosee
 *
 * Created on 3 février 2020, 17:34
 */


#include "xc.h"
#include "init.h"


int main(void) {
    oscillatorInit();
    TRISBbits.TRISB6 = 0;
    TRISBbits.TRISB5 = 1;
    
    
    while(1) {
        LATBbits.LATB6 = PORTBbits.RB5;
    }
    
    return 0;
}
