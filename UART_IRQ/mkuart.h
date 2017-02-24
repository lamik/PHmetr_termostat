/*
 * mkuart.h
 *
 *  Created on: 2010-09-04
 *       Autor: Miros³aw Kardaœ
 */

#ifndef MKUART_H_
#define MKUART_H_


#define UART_BAUD 9600		// tu definiujemy interesuj¹c¹ nas prêdkoœæ
#define __UBRR F_CPU/16/UART_BAUD-1  // obliczamy UBRR dla U2X=0

// definicje na potrzeby RS485
//#define UART_DE_PORT PORTD
#define UART_DE_DIR DDRD
#define UART_DE_BIT (1<<PD2)

#define UART_DE_ODBIERANIE  UART_DE_PORT &= ~UART_DE_BIT
#define UART_DE_NADAWANIE  UART_DE_PORT |= UART_DE_BIT


#define UART_RX_BUF_SIZE 32 // definiujemy bufor o rozmiarze 32 bajtów
// definiujemy maskê dla naszego bufora
#define UART_RX_BUF_MASK ( UART_RX_BUF_SIZE - 1)

#define UART_TX_BUF_SIZE 2 // definiujemy bufor o rozmiarze 16 bajtów
// definiujemy maskê dla naszego bufora
#define UART_TX_BUF_MASK ( UART_TX_BUF_SIZE - 1)


extern volatile uint8_t ascii_line;


// deklaracje funkcji publicznych

void USART_Init( uint16_t baud );

int uart_getc(void);
void uart_putc( char data );
void uart_puts(char *s);
void uart_putint(int value, int radix);

char * uart_get_str(char * buf);

void UART_RX_STR_EVENT(char * rbuf);
void register_uart_str_rx_event_callback(void (*callback)(char * pBuf));

#endif /* MKUART_H_ */
