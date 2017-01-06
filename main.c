/*
 * main.c
 *
 *  Created on: 4 sty 2017
 *      Author: Mateusz Salamon
 */
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "LCD/lcd44780.h"
#include "MK_ENCODER/mk_encoder.h"

void clear()
{
	enc_left = 0 ;
	enc_right = 0;
	//lcd_str("DOOOOOOOPA");
}

int main(void)
{
	lcd_init();
	lcd_cls();
	lcd_str("gitara siema");
	_delay_ms(1000);

	encoder_init(1);

	register_sw_callback(clear);

	int i=0;
	sei();
		while(1)
		{
			ENCODER_EVENT();
			lcd_cls();
			lcd_int(enc_left);
			lcd_locate(0,3);
			lcd_int(enc_right);
			lcd_locate(0,6);
			lcd_int(enc_timer);
			lcd_locate(1,3);
			lcd_int(enc_right);
			lcd_int(i++);
		}
}
