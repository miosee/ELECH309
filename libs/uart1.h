/*!
 * @file uart.h
 * @author Kevin De Cuyper
 * @brief Header file pour la configuration des UART
 */

#ifndef UART_H
#define	UART_H


typedef enum {
	UART_8N1_NO_HANDSHAKE	= 0x00,
	UART_8E1_NO_HANDSHAKE	= 0x02,
	UART_8O1_NO_HANDSHAKE	= 0x04,
	UART_9N1_NO_HANDSHAKE	= 0x06,
	UART_8N2_NO_HANDSHAKE	= 0x01,
	UART_8E2_NO_HANDSHAKE	= 0x03,
	UART_8O2_NO_HANDSHAKE	= 0x05,
	UART_9N2_NO_HANDSHAKE	= 0x07,
	UART_8N1_HARD_HANDSHAKE	= 0x200,
	UART_8E1_HARD_HANDSHAKE = 0x202,
	UART_8O1_HARD_HANDSHAKE = 0x204,
	UART_9N1_HARD_HANDSHAKE = 0x206,
	UART_8N2_HARD_HANDSHAKE = 0x201,
	UART_8E2_HARD_HANDSHAKE = 0x203,
	UART_8O2_HARD_HANDSHAKE = 0x205,
	UART_9N2_HARD_HANDSHAKE = 0x207
} uartModeType;

void uart1Config(float baudrate, uartModeType uartMode);
void uart1SendChar(unsigned char data);
unsigned char uart1GetChar(void);
int uart1RxDataAvailable(void);
int uart1TxReady(void);
void uart1TxIsrEnable(void);
void uart1TxIsrDisable(void);
void uart1RxIsrEnable(void);
void uart1RxIsrDisable(void);

/*
void __attribute__((interrupt, auto_psv)) _U1TXInterrupt(void) {
    IFS0bits.U1TXIF = 0;
}

void __attribute__((interrupt, auto_psv)) _U1RXInterrupt(void) {
    IFS0bits.U1RXIF = 0;
}
*/

#endif	/* UART_H */

