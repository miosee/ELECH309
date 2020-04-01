/*
 * File:   main.c
 * Author: M. Osée
 *
 * V1.0		09/10/2013
 */


#include "xc.h"
#include "init.h"
#include "../fskDetector/FskDetector.h"


#define LED         LATBbits.LATB5
#define LED_TRIS    TRISBbits.TRISB5


int16_t main(void) {
    int16_t message;
    
//	int16_t trame[13] = {0, 0,0, 1,0,1,0,1,0,1,0, 0, 0};    // 0 00 10101010 0 0 : trame correcte, parité 0
//	int16_t trame[13] = {0, 0,0, 1,0,1,0,1,0,0,0, 1, 0};    // 0 00 10101000 1 0 : trame correcte, parité 1
	int16_t trame[13] = {0, 0,0, 0,1,0,1,0,1,0,1, 1, 0};    // 0 00 01010101 1 0 : trame avec erreur de parité (parité 0)
//	int16_t trame[13] = {0, 0,1, 0,0,0,0,1,1,1,1, 0, 0};    // 0 01 00001111 0 0 : trame avec erreur de parité (parité 1)
//	int16_t trame[13] = {1, 0,1, 1,1,1,1,0,0,0,0, 1, 0};    // 1 01 11110000 1 0 : trame avec erreur de start bit
//	int16_t trame[13] = {0, 0,1, 0,0,0,0,1,1,1,1, 0, 0};    // 0 10 00110100 0 1 : trame avec erreur de stop bit

    int16_t bitNb, sampNb;		// index de boucle
	int16_t detLow, detHigh;	// sorties des détecteurs

    oscillatorInit();

    LED = 0;
    LED_TRIS = 0;       // configure la patte de la LED en sortie

    // on envoie les 13 bits de la trame de poids faible en commencant par le MSB
    for (bitNb=0; bitNb<13; bitNb++) {
		if (trame[bitNb] == 1) {       // si le bit vaut 1, on aura detLow=0 et detHigh=1
			detLow = 0;
			detHigh = 1;
		} else {
			detLow = 1;
			detHigh = 0;
		}
		for(sampNb=0; sampNb<OSR; sampNb++){
			message = fskDetector(detLow, detHigh);
        }
        if (message) {
            LED = 1;
        }
    }
    while(1);
}
