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
#include "ENCODER/encoder.h"
#include "DS18X20/ds18x20.h"

#define LED_PIN (1<<PB5)
#define LED_PORT PORTB
#define LED_DDR DDRB
//
#define LED_TOGGLE LED_PORT ^= LED_PIN
volatile uint8_t Timer_1s;
uint8_t enc_licznik;

void clear()
{
	enc_licznik = 0 ;
}

int main(void)
{
	LED_DDR |= LED_PIN;

	lcd_init();

	sei();

	lcd_str("  pH regulator");
	lcd_locate(1,0);
	lcd_str("   Termostat");
	ds18x20_cnt = search_sensors();
	DS18X20_start_meas(DS18X20_POWER_EXTERN, NULL);
	_delay_ms(1000);

	/*START TIMER0 for time base*/
	TCCR0A |= (1<<WGM01); //CTC
	TCCR0B |= (1<<CS02)|(1<<CS00); //prescaler 1024
	OCR0A = 155; //for 10ms period
	TIMSK0 |= (1<<OCIE0A); //Timer0 start

	encoder_init(1);
//	register_sw_callback(clear);

		while(1)
		{
			if(!Timer_1s)
			{
			DS18X20_read_meas(gSensorIDs[0],&subzero,&cel,&cel_fract_bits);
			DS18X20_start_meas(DS18X20_POWER_EXTERN, NULL);
			Timer_1s = 100;
			}
			if(enc_left_flag)
			{
				enc_licznik--;
				enc_left_flag = 0;
			}
			if(enc_right_flag)
			{
				enc_licznik++;
				enc_right_flag =0;
			}
			if(enc_sw_flag)
			{
				clear();
				enc_sw_flag = 0;
			}
//			ENCODER_EVENT();
			cli();
			lcd_cls();
			lcd_int(enc_licznik);
			lcd_locate(1,0);
			lcd_int(cel);
			lcd_char('.');
			lcd_int(cel_fract_bits);
			sei();

//			LED_TOGGLE;
//			_delay_ms(50);
		}
}

ISR(TIMER0_COMPA_vect)
{
	//10ms interrupt for time base
	uint16_t n;
//	Timer 1 sec
	n = Timer_1s;
	if (n) Timer_1s = --n;
}


