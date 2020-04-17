//////////////////////////////////////////////////////////////////////////////////////////
//																						//
//	bluetooth.h, créé le 13/01/2013 par M. Osée											//
//																						//
//	Cette librairie de fonction interace le dsPIC avec le module Bluetooth AMB2300		//
//	Elle est écrite pour la famille dsPIC33FJxxMCx02/04									//
//	Elle utilise le module UART1 et le TIMER3											//
//																						//
//	V1.0 : création																		//
//																						//
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef BLUETOOTH_H
#define	BLUETOOTH_H


	#define BT_NO_ERR						 0
	#define BT_HOST_NOT_CONNECTED_ERR		-1
	#define BT_TX_BUFFER_FULL				-2
	#define BT_RX_BUFFER_EMPTY				-3
	#define BT_HARDWARE_ERR					-4


	// defines wireless module reset pin
	#define LAT_AMB2300_RESET		_LATB2
	#define TRIS_AMB2300_RESET		_TRISB2


	// Configure l'UART et le PPS pour pouvoir communiquer avec le module AMB2300
	int16_t btInit(void);

	// Donne la place encore disponible dans le buffer d'émission, en octets
	// 2 cas possibles :
	//	- Aucun hôte n'est connecté : la fonction retourne RADIO_HOST_NOT_CONNECTED_ERR
	//	- Sinon, la fonction retourne le nombre d'octets qui peuvent encore être écrit dans le buffer d'émission
	int16_t btGetTxBufferSpace(void);

	// Ecrit des données dans le buffer d'émission
	// Le 1er paramètre est un pointeur vers les données à envoyer, considérées comme un vecteur de char
	// le 2ème paramètre est le nombre de char à écrire
	// 3 cas possibles :
	//	- Aucun hôte n'est connecté : la fonction ne fait rien et retourne RADIO_HOST_NOT_CONNECTED_ERR
	//	- un hôte est connecté, mais il n'y a pas assez de place dans le buffer d'émission : la fonction ne fait rien et retourne RADIO_TX_BUFFER_FULL
	//	- sinon (cas nominal) : la fonction copie les nb octets point16_tés par data dans le buffer d'émission et retourne RADIO_NO_ERR.
	int16_t btSendData(int8_t* data, int nb);

	// Donne le nombre d'octets encore à traiter dans le buffer de réception
	// 2 cas possibles :
	//	- Aucun hôte n'est connecté : la fonction retourne RADIO_HOST_NOT_CONNECTED_ERR
	//	- Sinon, la fonction retourne le nombre d'octets disponibles dans le buffer de réception
	int16_t btGetRxBufferSpace(void);

	// Lit des données dans le buffer de réception
	// Le 1er paramètre est un pointeur vers la variable à remplir avec les données (considérée comme un vecteur de char)
	// le 2ème paramètre est le nombre de char à lire
	// 3 cas possibles :
	//	- Aucun hôte n'est connecté : la fonction ne fait rien et retourne RADIO_HOST_NOT_CONNECTED_ERR
	//	- un hôte est connecté, mais il n'y a pas assez d'octets à lire dans le buffer de réception : la fonction ne fait rien
	//	  et retourne RADIO_RX_BUFFER_EMPTY
	//	- sinon (cas nominal) : la fonction copie nb octets du le buffer de réception dans la variable pointée par data et retourne RADIO_NO_ERR.
	int16_t btGetData(int8_t *data, int nb);


#endif
