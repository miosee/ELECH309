/*
 * Main file of voltmetre2.X
 * Author: mosee
 *
 * Impl�mentation du voltm�tre en utilisant le timer3. La fr�quence
 * d'�chantillonnage est la plus pr�cise possible pour le dsPIC, car
 * les conversions sont enti�rement g�r�es par le hardware.
 */

#include "xc.h"
#include "adc.h"


int main(void) {
    unsigned int sample, i;
   
    // Configuration de L'ADC pour utilisation en polling sur AN0
    adcInit(ADC_TIMER3_SAMPLING);
    PR3 = 772;                 // T=10ms=(PR1+1)/3.685MHz => PR1=36849

        /* Configuration du Peripheral Pin Select (PPS) pour connecter le signal
     * Rx de l'UART1 � RB6/RP6 et le signal Tx � RB7/RP7 */
	_U1RXR = 6;    // U1RX -> RP6
	_RP7R = 3;     // RP7 -> U1Tx

    // Configuration de l'UART1 avec un format de trame 8N1, � 57600 bits/s
    U1MODEbits.PDSEL = 0;       // 8 bits, pas de parit�
    U1MODEbits.STSEL = 0;       // 1 stop bit
    /* L'UART peut fonctionner � 2 vitesses diff�rentes. Le mode "high speed" 
     * est plus sensible au bruit et ne doit donc �tre utilis� que pour
     * des d�bits trop rapides pour le mode standard 
     * En mode standard, le d�bit est donn� par :
     * baud rate = FCY / (16*(U1BRG+1) 
     * => U1BRG = (3.685MHz / (16*57.6kHz)) - 1  =  2.998*/
    U1MODEbits.BRGH = 0;
    U1BRG = 3;
    
    U1MODEbits.UARTEN = 1;      // on active l'UART
    U1STAbits.UTXEN = 1;        // on active l'�mission


	while(1) {
        if (U1STAbits.URXDA) {      // si UART1 a re�u un octet
            sample = U1RXREG;           // On le lit
            if (sample == 's') {        // on v�rifie si c'est celui qu'on attendait
            	T3CONbits.TON = 1;      // on d�marre le timer3
                for (i=0; i<1000; i++) {            // on fait 1000 acquisitions
                    while (adcConversionDone()) {}  // en "pollant" l'ADC
                    sample = adcRead()/4;           // on ram�ne le r�sultat sur 8 bits
                    while (U1STAbits.UTXBF) {}      // on v�rifie si l'UART est dispo
                    U1TXREG = sample;               // on envoie l'�chantillon au PC
                }
            	T3CONbits.TON = 0;      // Apr�s les 100 acquisitions, on d�sactive le timer
            }
        }
	}
}
