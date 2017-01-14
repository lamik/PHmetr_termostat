/*
 * main.c
 *
 *  Created on: 4 sty 2017
 *      Author: Mateusz Salamon
 */
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "LCD_buf/lcd44780.h"
#include "ENCODER/encoder.h"
#include "DS18X20/ds18x20.h"
#include "MENU/menu.h"

#define LED_PIN (1<<PB5)
#define LED_PORT PORTB
#define LED_DDR DDRB
//
#define LED_TOGGLE LED_PORT ^= LED_PIN
volatile uint8_t Timer_1s;

const uint8_t celsius[] PROGMEM = {7,5,7,32,32,32,32,32};

int main(void)
{
	LED_DDR |= LED_PIN;
	lcd_init();
	lcd_defchar_P(0x80,celsius);

	sei();

	lcd_str("  pH regulator");
	lcd_locate(1,0);
	lcd_str("   Termostat");
	menu_actual = menu_main;
	ds18x20_cnt = search_sensors();
	DS18X20_start_meas(DS18X20_POWER_EXTERN, NULL);
	_delay_ms(1000);

	/*START TIMER0 for time base*/
	TCCR0A |= (1<<WGM01); //CTC
	TCCR0B |= (1<<CS02)|(1<<CS00); //prescaler 1024
	OCR0A = 155; //for 10ms period
	TIMSK0 |= (1<<OCIE0A); //Timer0 start

	encoder_init(1);

		while(1)
		{
			if(!Timer_1s)
			{
			DS18X20_read_meas(gSensorIDs[0],&subzero,&cel,&cel_fract_bits);
			DS18X20_start_meas(DS18X20_POWER_EXTERN, NULL);
			Timer_1s = 100;
			}

			menu_actual();

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


