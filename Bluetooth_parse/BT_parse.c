/*
 * BT_parse.c
 *
 *  Created on: 24 lut 2017
 *      Author: Mateusz Salamon
 */

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <string.h>

#include "../UART_IRQ/mkuart.h"
#include "BT_parse.h"
#include "../LCD_buf/lcd44780.h"


#define AT_CNT 	10	// iloæ poleceñ AT

//----------- tablica z poleceniami AT i wskaŸnikami funkcji do ich obs³ugi --------------------
const TATCMD polecenia_at[AT_CNT] PROGMEM =
{
	// { at_cmd } , { wskaŸnik do funkcji obs³ugi at },
		{"AT", 		at_service},
		{"ATI", 	ati_service},

		{"AT+INF", 	at_inf_service}, //request for all parameters

		{"AT+KH", 	at_kh_service}, //KH setting

		{"AT+TMP", 	at_tmp_service}, //temperature setting
		{"AT+TMH", 	at_tmh_service}, //temperature histerese setting
		{"AT+TMK", 	at_tmk_service}, //critical temperature setting

		{"AT+PH", 	at_php_service}, //pH setting
		{"AT+PHH", 	at_phh_service}, //pH histerese setting
		{"AT+PHK", 	at_phk_service}, //critical pH setting
};

void parse_uart_data( char * pBuf )
{
	int8_t (*_at_srv)(uint8_t inout, char * data);

		char * cmd_wsk;
		char * reszta;
		uint8_t i=0, len;

		if ( strpbrk(pBuf, "=?"))	{
			// obs³uga poleceñ AT we/wy + parametry

			if ( strpbrk(pBuf, "?"))	{
				// zapytania do uk³adu w postaci: AT+CMD?

				cmd_wsk = strtok_r(pBuf, "?", &reszta);
				len = strlen(cmd_wsk);
				for(i=0;i<AT_CNT;i++) {
					if ( len && 0 == strncasecmp_P(cmd_wsk, polecenia_at[i].polecenie_at, len) ) {
						if( pgm_read_word(polecenia_at[i].polecenie_at) ) { // <--- UWAGA! w tekœcie ksi¹¿ki zabrak³o pgm_read_word()
							_at_srv = (void *)pgm_read_word( &polecenia_at[i].at_service );
							if( _at_srv) {
								if( _at_srv( 0, reszta ) < 0 ) uart_puts("ERROR\r\n");
							}
						}
						uart_puts("\r\n");
						break;
					}
				}

			} else {
				// ustawienia uk³adu w postaci: AT+CMD=parametry

				cmd_wsk = strtok_r(pBuf, "=", &reszta);
				len = strlen(cmd_wsk);
				for(i=0;i<AT_CNT;i++) {
					if ( len && 0 == strncasecmp_P(cmd_wsk, polecenia_at[i].polecenie_at, len) ) {
						if( pgm_read_word(polecenia_at[i].polecenie_at) ) { // <--- UWAGA! w tekœcie ksi¹¿ki zabrak³o pgm_read_word()
							_at_srv = (void *)pgm_read_word( &polecenia_at[i].at_service );
							if( _at_srv && ! _at_srv( 1, reszta ) ) uart_puts("OK\r\n");
							else uart_puts("ERROR\r\n");
						}
						break;
					}
				}
			}

		} else {
			// obs³uga poleceñ AT bez parametrów

			if( 0 == pBuf[0] ) uart_puts("\r\n");	// reakcja na znak CR lub CRLF z terminala
			else {
				for(i=0;i<AT_CNT;i++) {
					if ( 0 == strncasecmp_P(pBuf, polecenia_at[i].polecenie_at, strlen(pBuf)) ) {
						if( pgm_read_word(polecenia_at[i].polecenie_at) ) { // <--- UWAGA! w tekœcie ksi¹¿ki zabrak³o pgm_read_word()
							_at_srv = (void *)pgm_read_word( &polecenia_at[i].at_service );
							if( _at_srv) _at_srv(2,0);
						}
						break;
					}
				}
			}
		}

		if( AT_CNT == i ) uart_puts("ERROR\r\n");
}

int8_t at_service(uint8_t inout, char * params)
{
	uart_puts("OK\r\n");
	return 0;
}

int8_t ati_service(uint8_t inout, char * params)
{
	uart_puts("pH Controller\r\n");
	return 0;
}

int8_t at_inf_service(uint8_t inout, char * params)
{

	return 0;
}

int8_t at_kh_service(uint8_t inout, char * params)
{

	return 0;
}

int8_t at_tmp_service(uint8_t inout, char * params)
{

	return 0;
}

int8_t at_tmh_service( uint8_t inout, char * params )
{

	return 0;
}

int8_t at_tmk_service(uint8_t inout, char * params)
{

	return 0;
}

int8_t at_php_service(uint8_t inout, char * params)
{

	return 0;
}

int8_t at_phh_service(uint8_t inout, char * params)
{

	return 0;
}

int8_t at_phk_service( uint8_t inout, char * params )
{

	return 0;
}
