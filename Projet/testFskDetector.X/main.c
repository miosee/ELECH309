/*
 * Main file of testFskDetector.X
 * Author: mosee
 */

#include "xc.h"
#include "configuration.h"
#include <stdio.h>

#include "FskDetector.h"


int getFrame(int start, int cmd, int param, int parity, int stop);
int sendFrame(int frame);
void printFrame(int frame);
void printMessage(int data);
void sendSilence(void);


int main(void) {
    int frame, decodedMsg;

    frcPll40MHzConfig();
   
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
     * => U1BRG = (40MHz / (16*57.6kHz)) - 1  =  42.4*/
    U1MODEbits.BRGH = 0;
    U1BRG = 42;
    
    U1MODEbits.UARTEN = 1;      // on active l'UART
    U1STAbits.UTXEN = 1;        // on active l'�mission


    frame = getFrame(0, 0, 0x55, 0, 1);
    printf("\nTrame avec une parite paire   : ");
    printFrame(frame);
    decodedMsg = sendFrame(frame);
    if (decodedMsg != 0) {
       printMessage(decodedMsg);
    }
    sendSilence();
    
    frame = getFrame(1, 0, 0x03, 1, 1);
    printf("\nErreur de start bit           : ");
    printFrame(frame);
    decodedMsg = sendFrame(frame);
    if (decodedMsg != 0) {
       printMessage(decodedMsg);
    }
    sendSilence();
    
    frame = getFrame(0, 1, 0xF0, 1, 1);
    printf("\nTrame avec une parite impaire : ");
    printFrame(frame);
    decodedMsg = sendFrame(frame);
    if (decodedMsg != 0) {
       printMessage(decodedMsg);
    }
    sendSilence();
    
    frame = getFrame(0, 0, 0xAA, 1, 1);
    printf("\nErreur de parite paire        : ");
    printFrame(frame);
    decodedMsg = sendFrame(frame);
    if (decodedMsg != 0) {
       printMessage(decodedMsg);
    }
    sendSilence();

    frame = getFrame(0, 3, 0xAA, 0, 1);
    printf("\nTrame avec une parite paire   : ");
    printFrame(frame);
    decodedMsg = sendFrame(frame);
    if (decodedMsg != 0) {
       printMessage(decodedMsg);
    }
    sendSilence();
    
    frame = getFrame(0, 3, 0x0F, 1, 1);
    printf("\nErreur de parite impaire      : ");
    printFrame(frame);
    decodedMsg = sendFrame(frame);
    if (decodedMsg != 0) {
       printMessage(decodedMsg);
    }
    sendSilence();

    frame = getFrame(0, 2, 0xCC, 1, 1);
    printf("\nTrame avec une parite impaire : ");
    printFrame(frame);
    decodedMsg = sendFrame(frame);
    if (decodedMsg != 0) {
       printMessage(decodedMsg);
    }
    
    frame = getFrame(0, 0, 0x5A, 0, 0);
    printf("\nErreur de stop bit            : ");
    printFrame(frame);
    decodedMsg = sendFrame(frame);
    if (decodedMsg != 0) {
       printMessage(decodedMsg);
    }
    sendSilence();
    
    frame = getFrame(0, 1, 0x33, 1, 1);
    printf("\nTrame avec une parite impaire : ");
    printFrame(frame);
    decodedMsg = sendFrame(frame);
    if (decodedMsg != 0) {
       printMessage(decodedMsg);
    }

	while(1) {
	}
    
    return(1);
}


int getFrame(int start, int cmd, int param, int parity, int stop) {
    int frame;
    
    frame = (start << 12);
    frame += (cmd << 10);
    frame += (param << 2);
    frame += (parity << 1) + stop;
    
    return (frame);
}

int sendFrame(int frame) {
    int bitCount, sampleCount;
    int detLow, detHigh;
    int decodedFrame;
    
    for (bitCount=12; bitCount>=0; bitCount--) {
        if ( frame & (1 << bitCount) ) {
            detLow = 0;
            detHigh = 1;
        } else {
            detLow = 1;
            detHigh = 0;
        }
        for(sampleCount=0; sampleCount<OSR; sampleCount++){
            decodedFrame = fskDetector(detLow, detHigh);
        }
    }
    return (decodedFrame);
}

void sendSilence(void) {
    int sampleCount;
    
    printf("Envoi d'un bit de silence\n");
    for(sampleCount=0; sampleCount<OSR; sampleCount++){
        fskDetector(0, 0);
    }
}

void printFrame(int frame) {
	int i;

	printf("%i|", (frame&(1<<12))>>12);		// le START BIT est le 'b12' de la trame
	for (i=11; i>9; i--) {
		printf("%i", (frame&(1<<i))>>i);		// ensuite, viennent les 2 bits de l'ordre
	}
	printf("|");
	for (i=9; i>1; i--) {
		printf("%i", (frame&(1<<i))>>i);		// puis les 8 bits du param?tre,
	}
	printf("|");
	printf("%i|", (frame&2)>>1);				// le bit de parit?
	printf("%i\n", (frame&1));				// et le STOP BIT
}

void printMessage(int data) {
	int i;

    printf("Message d�cod�                :   ");
	for (i=9; i>7; i--) {					// l'ordre se trouve dans les bits b9-b8 de data
		printf("%i", (data&(1<<i))>>i);		// on utilise un masque pour r?cup?rer le bit qui nous int?resse
	}										// et on le shifte ? droite pour qu'il deviennent b0
	printf("|");
	for (i=7; i>=0; i--) {
		printf("%i", (data&(1<<i))>>i);		// On fait de m?me pour les 8 bits du param?tre
	}
	printf("\n");
}
