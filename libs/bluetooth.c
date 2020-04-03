//////////////////////////////////////////////////////////////////////////////////////////
//																						//
//	bluetooth.c, cree le 13/01/2013 par M. Osee											//
//																						//
//	Cette librairie de fonction interace le dsPIC avec le module Bluetooth AMB2300		//
//	Elle est ecrite pour la famille dsPIC33FJxxMCx02/04									//
//	Elle utilise le module UART1 et le TIMER3											//
//																						//
//	V1.0 : creation																		//
//																						//
//////////////////////////////////////////////////////////////////////////////////////////



#include "xc.h"
#include <libpic30.h>       // Contient la declaration de __delay32()
#include "uart1.h"
#include "bluetooth.h"


// Codes utilises dans les messages de statuts du LMX9830 (circuit principal du AMB2300)
#define LMX9830_STX						(0x02)
#define LMX9830_IND						(0x69)
#define LMX9830_READY					(0x25)
#define LMX9830_LINK_ESTABLISHED		(0x0C)
#define LMX9830_ETX						(0x03)
#define LMX9830_SPP_TRANSPARENT_MODE	(0x11)
#define LMX9830_SPP_LINK_RELEASED		(0x0E)


#define BT_RX_DATA_SIZE		256
#define BT_TX_DATA_SIZE		256
#define BT_TIME_OUT			5000	// en ms
// LMX9830 frame detection, int timer ticks (Prescaler = 1:8)
// PR3 = timeout/(8*Tcy) - 1 = 2ms/200ns - 1 = 10E3 - 1
#define TMR3_TIMEOUT    9999

char btRxData[BT_RX_DATA_SIZE];
int btRxDataNb;
int btRxDataStartPtr;
int btRxDataEndPtr;

char btTxData[BT_TX_DATA_SIZE];
int btTxDataNb;
int btTxDataStartPtr;
int btTxDataEndPtr;

int btLinkStatus;
#define BT_NO_LINK          0
#define BT_AMB2300_READY    1
#define BT_HOST_CONNECTED   2

int btIsrFrameState = 0;
char btIsrChecksum = 0;
int btIsrLength = 0;
int btIsrDataNb = 0;



// Donne la place encore disponible dans le buffer d'emission, en octets
// 2 cas possibles :
//	- Aucun hote n'est connecte : la fonction retourne BT_HOST_NOT_CONNECTED_ERR
//	- Sinon, la fonction retourne le nombre d'octets qui peuvent encore etre ecrit dans le buffer d'emission
int16_t btGetTxBufferSpace(void) {
	if (btLinkStatus == BT_HOST_CONNECTED) {
		return(btTxDataNb);
	} else {
		return(BT_HOST_NOT_CONNECTED_ERR);
	}	
}


// Ecrit des donnees dans le buffer d'emission
// Le 1er parametre est un pointeur vers les donnees a envoyer, considerees comme un vecteur de int8_t
// le 2eme parametre est le nombre d'octets a ecrire
// 3 cas possibles :
//	1) Aucun hote n'est connecte : la fonction ne fait rien et retourne BT_HOST_NOT_CONNECTED_ERR
//	2) un hote est connecte, mais il n'y a pas assez de place dans le buffer d'emission :
//     la fonction ne fait rien et retourne BT_TX_BUFFER_FULL
//	3) sinon (cas nominal) : la fonction copie les nb octets pointes par data 
//     dans le buffer d'emission et retourne BT_NO_ERR.
int16_t btSendData(int8_t* data, int nb) {
	int16_t i;
	
	if (btLinkStatus != BT_HOST_CONNECTED) {            // cas 1
		return(BT_HOST_NOT_CONNECTED_ERR);
	}
    else if (btTxDataNb < nb) {                         // cas 2
		return(BT_TX_BUFFER_FULL);
	}
    else {                                              // cas 3
		btTxDataNb -= nb;                               // btTxDataNb est mis a jour
		for (i=0; i<nb; i++) {
			btTxData[btTxDataEndPtr] = data[i];			// les donnees sont ecrites dans le buffer d'emission
			btTxDataEndPtr++;                           // le pointeur vers la derniere donnee est mis a jour
			if (btTxDataEndPtr >= BT_TX_DATA_SIZE) {    // en tenant compte que nous implementons un buffer
				btTxDataEndPtr = 0;                     // cyclique
			}
		}
		if ( U1STAbits.UTXBF == 0 ) {                   // si le module UART n'est pas occupe a transmettre
			IFS0bits.U1TXIF = 1;                        // on declenche l'envoi en activant le flag d'interruption
		}
		return(BT_NO_ERR);
	}
}


// Donne le nombre d'octets encore a traiter dans le buffer de rzception
// 2 cas possibles :
//	- Aucun hote n'est connecte : la fonction retourne BT_HOST_NOT_CONNECTED_ERR
//	- Sinon, la fonction retourne le nombre d'octets disponibles dans le buffer de reception
int16_t btGetRxBufferSpace(void) {
	if (btLinkStatus == BT_HOST_CONNECTED) {
		return(btRxDataNb);
	}
    else {
		return(BT_HOST_NOT_CONNECTED_ERR);
	}
}


// Lit des donnees dans le buffer de reception
// Le 1er parametre est un pointeur vers la variable a remplir avec les donnees (consideree comme un vecteur de int8_t)
// le 2eme parametre est le nombre de char a lire
// 3 cas possibles :
//	1) Aucun hote n'est connecte : la fonction ne fait rien et retourne BT_HOST_NOT_CONNECTED_ERR
//	2) un hote est connecte, mais il n'y a pas assez d'octets a lire dans le buffer de reception :
//     la fonction ne fait rien et retourne BT_RX_BUFFER_EMPTY
//	3) sinon (cas nominal) : la fonction copie nb octets du le buffer de reception
//     dans la variable pointee par data et retourne BT_NO_ERR.
int16_t btGetData(int8_t *data, int nb) {
	int16_t i;

	if (btLinkStatus != BT_HOST_CONNECTED) {        // cas 1
		return(BT_HOST_NOT_CONNECTED_ERR);
	}
    else if (btRxDataNb < nb) {                     // cas 2
		return(BT_RX_BUFFER_EMPTY);
	}
    else {                                          // cas 3
		btRxDataNb -= nb;                               // btRxDataNb est mis a jour
		for (i=0; i<nb; i++) {
			data[i] = btRxData[btRxDataStartPtr];       // la plus ancienne donnee du buffer de reception est lue
			btRxDataStartPtr++;                         // Le pointeur vers la plus ancienne donnee est mis a jour
			if (btRxDataStartPtr == BT_RX_DATA_SIZE) {  // en tenant compte que nous implementons un buffer cyclique
				btRxDataStartPtr = 0;
			}
		}
		return(BT_NO_ERR);
	}
}


// Configure l'UART et le PPS pour pouvoir communiquer avec le module AMB2300
int16_t btInit(void) {
	int16_t frameState;
	int16_t err;
	int8_t data;
	int16_t length;
	int8_t checksum;

	
	_U1RXR = 15;    // U1RX -> RP15
	_U1CTSR = 16;   // U1CTS -> RP16
	_RP14R = 3;     // RP14 -> U1Tx
	_RP3R = 4;      // RP3 -> U1RTS

	btRxDataNb = 0;
	btRxDataStartPtr = 0;
	btRxDataEndPtr = 0;
	btTxDataNb =  BT_TX_DATA_SIZE;
	btTxDataStartPtr = 0;
	btTxDataEndPtr = 0;
	btLinkStatus = BT_NO_LINK;
	
	// Configure uart1 receive and transmit interrupt
    uart1Config(115200, UART_8N1_NO_HANDSHAKE);
    uart1RxIsrEnable();
    uart1TxIsrEnable();

	// Configures timer3 to generate a timeout interrupt for LMX9830 frame detection
	T3CONbits.TON = 0;      // stop Timer3
	TMR3 = 0;               // clear timer3
	PR3 = TMR3_TIMEOUT;     // Set timeout value
	T3CONbits.TCS = 0;      // Timer3 uses internal clock (Fcy = Fosc/2 = 40MHz)
	T3CONbits.TCKPS = 1;    // prescaler = 1:8
	T3CONbits.TGATE = 0;    // we don't use external enable signal
	_T3IF = 0;              // clear the interrupt flag
	_T3IE = 0;              // disable the interrupt
	
	LAT_AMB2300_RESET = 0;
	TRIS_AMB2300_RESET = 0;
	__delay32(1000);
	LAT_AMB2300_RESET = 1;
	__delay32(500000);
	
	frameState = 0;
	err = BT_NO_ERR;
	checksum = 0;
    //T3CONbits.TON = 1;		// start Timer3 as timeout counter
	// The AMB2300 should send a valid start-up frame. Otherwise, Timer3 will
	// overflow, and the function will end waiting for the frame
	while ((_T3IF == 0) && (btLinkStatus == BT_NO_LINK) && (err == BT_NO_ERR)) {
		// if there is data available in the input buffer
		if (btRxDataNb > 0) {
			data = btRxData[btRxDataStartPtr];
			// we read one data, so btRxDataNb msut be decremented
			btRxDataNb--;
			// start pointer must now point to the "new" oldest byte, that is the
			// next one ...
			btRxDataStartPtr++;
			// ... unless the end of the vector is reached. If so, the oldest byte
			// is the first of the vector (it should'nt happen here)
			if (btRxDataStartPtr == BT_RX_DATA_SIZE)   btRxDataStartPtr = 0;

			switch (frameState) {
				case 0:
					// the data must match the start-up frame
					if (data == LMX9830_STX) {
						frameState = 1;
						TMR3 = 0;
					} else {
						err = BT_HARDWARE_ERR;	
					}
				break;
				case 1:
					// the data must match the start-up frame
					if (data == LMX9830_IND) {
						frameState = 2;
						TMR3 = 0;
						checksum += data;
					} else {
						err = BT_HARDWARE_ERR;	
					}
				break;
				case 2:
					// the data must match the start-up frame
					if (data == LMX9830_READY) {
						frameState = 3;
						TMR3 = 0;
						checksum += data;
					} else {
						err = BT_HARDWARE_ERR;	
					}
				break;
				case 3:
					length = data;
					checksum += data;
					TMR3 = 0;
					frameState = 4;
				break;
				case 4:
					length += (((int)data) << 8);
					checksum += data;
					if (length > 0) {
						frameState = 5;
						TMR3 = 0;
					} else {
						err = BT_HARDWARE_ERR;
					}		
				break;
				case 5:
					if (data == checksum){
						frameState = 6;
						TMR3 = 0;
					} else {
						err = BT_HARDWARE_ERR;
					}
				break;
				case 6:
					length--;
					TMR3 = 0;
					if (length <= 0) frameState = 7;
				break;
				case 7:
					if (data == LMX9830_ETX) {
						btLinkStatus =  BT_AMB2300_READY;
						TMR3 = 0;
					} else {
						err = BT_HARDWARE_ERR;
					}	
				break;
			}
		}
	}
	T3CONbits.TON = 0;
	TMR3 = 0;
	if (_T3IF == 1){
		_T3IF = 0;		// clear the interrupt flag
		err = BT_HARDWARE_ERR;
	}
	_T3IE = 1;		// enable the interrupt
	return(err);
}



//	INTERRUPTS
//////////////////////////////////////////////////////////////////////////////

/* This is TIMER3 ISR */ 
void __attribute__((__interrupt__, no_auto_psv)) _T3Interrupt(void) {
	// interrupt flag is reset
	_T3IF = 0;
	// Timer3 is stopped
	T3CONbits.TON = 0;
	// Timer3 is reset
	TMR3 = 0;
	// btRxDataNb is updated
	btRxDataNb += (btIsrFrameState + 1);
	btIsrDataNb = 0;
	// frameState is reset
	btIsrFrameState = 0;
	btIsrChecksum = 0;
}


/* This is UART1 transmit ISR */ 
void __attribute__((__interrupt__, no_auto_psv)) _U1TXInterrupt(void) {   
	// reset the interrupt flag
	IFS0bits.U1TXIF = 0;
	// if there are bytes to send in our output buffer and the UART Tx buffer
	// is not full, we copy the next byte to send in the Tx buffer.
	while ( ( btTxDataNb <  BT_TX_DATA_SIZE ) && ( U1STAbits.UTXBF == 0 ) ) {
		// first byte of the output buffer is written in the UART Tx buffer
		U1TXREG = btTxData[btTxDataStartPtr];
		// the start pointer must now point to the next byte to transmit,
		// that is the next in the vector ...
		btTxDataStartPtr++;
		// ... unless the end of the vector is reached. If so, the next byte
		// to transmit is at the beginning of the vector (even if the vector
		// is empty: the next byte will be written there)
		if (btTxDataStartPtr >= BT_TX_DATA_SIZE) {
			btTxDataStartPtr = 0;
		}
		// data counter is decremented
		btTxDataNb++;
	}
}


/* This is UART1 receive ISR */ 
void __attribute__((__interrupt__, no_auto_psv)) _U1RXInterrupt(void) {
	int8_t data;
	
    IFS0bits.U1RXIF = 0;
	while (U1STAbits.URXDA) {
		// First, if there was a framing error, we discard the byte.
		// It should never happen
		if (U1STAbits.FERR == 1) {
            data = U1RXREG;
		}
		else if (btRxDataNb < BT_RX_DATA_SIZE) {    // if input buffer is not full
			// new byte is written in the buffer
			data = U1RXREG;
			btRxData[btRxDataEndPtr] = data;
			// the end pointer must now point to where the next byte received
			// has to be put, that is after this one in the vector ...
			btRxDataEndPtr++;
			// ... unless the end of the vector is reached. If so, we put the
			// new byte at the beginning of the vector (as buffer is not full,
			// its first byte has to be empty)
			if (btRxDataEndPtr == BT_RX_DATA_SIZE) {
				btRxDataEndPtr = 0;
			}
	
			if (btLinkStatus == BT_NO_LINK) {
				btRxDataNb++;
			}
			else if (btLinkStatus == BT_AMB2300_READY) {
				btIsrDataNb++;
				switch (btIsrFrameState) {
					case 0:
						// the data must match the host connection frame
						if (data == LMX9830_STX) {
							btIsrFrameState = 1;
							TMR3 = 0;
							T3CONbits.TON = 1;
						}
                        else {
							_T3IF = 1;	// call Timer3 ISR
						}
					break;
					case 1:
						// the data must match the host connection frame
						if (data == LMX9830_IND) {
							btIsrFrameState = 2;
							TMR3 = 0;
							btIsrChecksum += data;
						}
                        else {
							_T3IF = 1;	// call Timer3 ISR
						}
					break;
					case 2:
						// the data must match the host connection frame
						if (data == LMX9830_LINK_ESTABLISHED) {
							btIsrFrameState = 3;
							TMR3 = 0;
							btIsrChecksum += data;
						}
                        else {
							_T3IF = 1;	// call Timer3 ISR
						}
					break;
					case 3:
						btIsrLength = data;
						btIsrChecksum += data;
						TMR3 = 0;
						btIsrFrameState = 4;
					break;
					case 4:
						btIsrLength += (((int)data) << 8);
						btIsrChecksum += data;
						if (btIsrLength > 0) {
							btIsrFrameState = 5;
							TMR3 = 0;
						}
                        else {
							_T3IF = 1;	// call Timer3 ISR
						}		
					break;
					case 5:
						if (data == btIsrChecksum){
							btIsrFrameState = 6;
							TMR3 = 0;
						}
                        else {
							_T3IF = 1;	// call Timer3 ISR
						}
					break;
					case 6:
						btIsrLength--;
						TMR3 = 0;
						if (btIsrLength <= 0) {
                            btIsrFrameState = 7;
                        }
					break;
					case 7:
						if (data == LMX9830_ETX) {
							btLinkStatus =  BT_HOST_CONNECTED;
							T3CONbits.TON = 0;		// stop Timer3
							TMR3 = 0;
							btIsrFrameState = 0;
							btIsrChecksum = 0;
							btRxDataStartPtr += btIsrDataNb;
							btIsrDataNb = 0;
						}
                        else {
							_T3IF = 1;	// call Timer3 ISR
						}	
					break;
				}
				
			}
			else if (btLinkStatus == BT_HOST_CONNECTED)	{   // if host PC is connected
				btIsrDataNb++;
				// Verify if we are receiving a LMX9830 frame, indicating the
				// deconnection of the remote device (SPP_TRANSPARENT_MODE)
				switch (btIsrFrameState) {
					case 0:
						// the data must match the host connection frame
						if (data == LMX9830_STX) {
							btIsrFrameState = 1;
							TMR3 = 0;
							T3CONbits.TON = 1;
						}
                        else {
							_T3IF = 1;	// call Timer3 ISR
						}
					break;
					case 1:
						// the data must match the host connection frame
						if (data == LMX9830_IND) {
							btIsrFrameState = 2;
							TMR3 = 0;
							btIsrChecksum += data;
						}
                        else {
							_T3IF = 1;	// call Timer3 ISR
						}
					break;
					case 2:
						// the data must match the host connection frame
						if (data == LMX9830_SPP_TRANSPARENT_MODE) {
							btIsrFrameState = 3;
							TMR3 = 0;
							btIsrChecksum += data;
						}
                        else {
							_T3IF = 1;	// call Timer3 ISR
						}
					break;
					case 3:
						btIsrLength = data;
						btIsrChecksum += data;
						TMR3 = 0;
						btIsrFrameState = 4;
					break;
					case 4:
						btIsrLength += (((int)data) << 8);
						btIsrChecksum += data;
						if (btIsrLength > 0) {
							btIsrFrameState = 5;
							TMR3 = 0;
						}
                        else {
							_T3IF = 1;	// call Timer3 ISR
						}		
					break;
					case 5:
						if (data == btIsrChecksum){
							btIsrFrameState = 6;
							TMR3 = 0;
						}
                        else {
							_T3IF = 1;	// call Timer3 ISR
						}
					break;
					case 6:
						btIsrLength--;
						TMR3 = 0;
						if (btIsrLength <= 0) btIsrFrameState = 7;
					break;
					case 7:
						if (data == LMX9830_ETX) {
							TMR3 = 0;
							btIsrFrameState = 8;
							btIsrChecksum = 0;
						}
                        else {
							_T3IF = 1;	// call Timer3 ISR
						}	
					break;
					
					case 8:
						// the data must match the host connection frame
						if (data == LMX9830_STX) {
							btIsrFrameState = 9;
							TMR3 = 0;
						}
                        else {
							_T3IF = 1;	// call Timer3 ISR
						}
					break;
					case 9:
						// the data must match the host connection frame
						if (data == LMX9830_IND) {
							btIsrFrameState = 10;
							TMR3 = 0;
							btIsrChecksum += data;
						}
                        else {
							_T3IF = 1;	// call Timer3 ISR
						}
					break;
					case 10:
						// the data must match the host connection frame
						if (data == LMX9830_SPP_LINK_RELEASED) {
							btIsrFrameState = 11;
							TMR3 = 0;
							btIsrChecksum += data;
						}
                        else {
							_T3IF = 1;	// call Timer3 ISR
						}
					break;
					case 11:
						btIsrLength = data;
						btIsrChecksum += data;
						TMR3 = 0;
						btIsrFrameState = 12;
					break;
					case 12:
						btIsrLength += (((int)data) << 8);
						btIsrChecksum += data;
						if (btIsrLength > 0) {
							btIsrFrameState = 13;
							TMR3 = 0;
						}
                        else {
							_T3IF = 1;	// call Timer3 ISR
						}		
					break;
					case 13:
						if (data == btIsrChecksum){
							btIsrFrameState = 14;
							TMR3 = 0;
						}
                        else {
							_T3IF = 1;	// call Timer3 ISR
						}
					break;
					case 14:
						btIsrLength--;
						TMR3 = 0;
						if (btIsrLength <= 0) {
                            btIsrFrameState = 15;
                        }
					break;
					case 15:
						if (data == LMX9830_ETX) {
							btLinkStatus =  BT_AMB2300_READY;
							T3CONbits.TON = 0;		// stop Timer3
							TMR3 = 0;
							btIsrFrameState = 0;
							btIsrChecksum = 0;
							btRxDataStartPtr += btIsrDataNb;
							btIsrDataNb = 0;
						}
                        else {
							_T3IF = 1;	// call Timer3 ISR
						}	
					break;
				}
			}	
		}	
	}
}
