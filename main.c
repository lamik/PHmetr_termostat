/*
 * main.c
 *
 *  Created on: 4 sty 2017
 *      Author: Mateusz Salamon
 */
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "LCD_buf/lcd44780.h"
#include "MK_ENCODER/mk_encoder.h"
#include "DS18X20/ds18x20.h"

//#define LED_PIN (1<<PB5)
//#define LED_PORT PORTB
//#define LED_DDR DDRB
//
//#define LED_TOGGLE LED_PORT ^= LED_PIN

void clear()
{
	enc_left = 0 ;
	enc_right = 0;
	//lcd_str("DOOOOOOOPA");
}

int main(void)
{
//	LED_DDR |= LED_PIN;

	lcd_init();

	sei();

	lcd_str("  pH regulator");
	lcd_locate(1,0);
	lcd_str("   Termostat");
	ds18x20_cnt = search_sensors();
	DS18X20_start_meas(DS18X20_POWER_EXTERN, NULL);
	_delay_ms(1000);

	encoder_init(1);
	register_sw_callback(clear);
	int i=0;

		while(1)
		{
			DS18X20_read_meas(gSensorIDs[0],&subzero,&cel,&cel_fract_bits);
			DS18X20_start_meas(DS18X20_POWER_EXTERN, NULL);

			ENCODER_EVENT();
			lcd_cls();
			lcd_int(i++);
			lcd_locate(1,0);
			lcd_int(cel);
			lcd_char('.');
			lcd_int(cel_fract_bits);

//			LED_TOGGLE;
			_delay_ms(50);
		}
}



