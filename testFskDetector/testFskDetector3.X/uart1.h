/*!
 * @file uart.h
 * @author Kevin De Cuyper
 * @brief Header file pour la configuration des UART
 */

#ifndef UART_H
#define	UART_H


// initialise l'uart1 en mode 8bits de données, pas de parité, 1 stop bit
// le paramètre baudrate indique à la fonction le baudrate désiré, en Hz
void uart1Init(float baudrate, uint32_t fcy);

// renvoie 1 si il y a une donnée qui attend d'être lue dans le buffer de réception, 0 sinon
int16_t uart1RxDataAvailable(void);

// renvoie 1 si le buffer d'émission n'est pas plein
int16_t uart1TxReady(void);

// envoie un char sur l'uart1. Cette fonction est bloquante. En effet, si le buffer d'émission est plein,
// elle attendra qu'il se vide pour envoyer son octet
void uart1SendChar(uint8_t data);

// reçoit un char par l'uart1. Cette fonction est bloquante. En effet, si il n'y a rien à lire dans le buffer,
// elle attendra qu'un octet soit recu par l'uart1 pour le lire
uint8_t uart1GetChar(void);

#endif	/* UART_H */

