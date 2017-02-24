/*
 * BT_parse.c
 *
 *  Created on: 24 lut 2017
 *      Author: Mateusz Salamon
 */

#include <avr/io.h>
#include "../LCD_buf/lcd44780.h"

void parse_uart_data( char * pBuf ) {

	// wywietlamy odebrane polecenia AT na LCD
	lcd_locate(0,0);
	lcd_locate(1,0);
	lcd_str("                ");
	lcd_locate(1,0);
	lcd_str("[");
	lcd_str( pBuf );
	lcd_str("]");

}
