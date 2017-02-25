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
#include <avr/eeprom.h>

#include "LCD_buf/lcd44780.h"
#include "ENCODER/encoder.h"
#include "Termostat/termostat.h"
#include "MENU/menu.h"
#include "PH_CONTROL/ph_control.h"
#include "SETTINGS/settings.h"

#define LED_PIN (1<<PB5)
#define LED_PORT PORTB
#define LED_DDR DDRB
//
#define LED_TOGGLE LED_PORT ^= LED_PIN
#define LED_ON	LED_PORT &= ~LED_PIN
#define LED_OFF LED_PORT |= LED_PIN

volatile uint8_t Timer_1s;
extern TSettings settings;
extern uint16_t menu_time;

const uint8_t celsius[] PROGMEM = {7,5,7,32,32,32,32,32};
//Polish chars
const uint8_t l_pol[] PROGMEM = {12,4,6,4,12,4,14,32};
const uint8_t s_pol[] PROGMEM = {2,4,14,16,14,1,30,32};
const uint8_t c_pol[] PROGMEM = {2,4,14,16,16,17,14,32};
const uint8_t o_pol[] PROGMEM = {2,4,14,17,17,17,14,32};

int main(void)
{
	LED_DDR |= LED_PIN;
	LED_OFF;
	lcd_init();
	lcd_defchar_P(0,celsius);
	lcd_defchar_P(1,l_pol);
	lcd_defchar_P(2,s_pol);
	lcd_defchar_P(3,c_pol);
	lcd_defchar_P(4,o_pol);

	menu_time = eeprom_read_word(&settings.menu_time);

	sei();
	lcd_str("  Kontroler pH");
	lcd_locate(1,0);
	lcd_str("   Termostat");
	menu_actual = menu_main;
	Termostat_init();
	Ph_controler_init();
	_delay_ms(1000);

	/*START TIMER0 for time base*/
	TCCR0A |= (1<<WGM01); //CTC
	TCCR0B |= (1<<CS02)|(1<<CS00); //prescaler 1024
	OCR0A = 155; //for 10ms period
	TIMSK0 |= (1<<OCIE0A); //Timer0 start

	encoder_init(1);

	Timer_menu = menu_time;
		while(1)
		{
			Termostat_get_temperature();
			Termostat();

			Ph_controler_get_pH();
			Ph_controler_control_pH();

			menu_actual();

			LCD_EVENT();

			if((!Timer_menu) && (menu_actual != menu_main))
			{
				menu_actual = menu_main;
				Timer_menu = menu_time;
			}
			if(Timer_menu)
			{
				LED_ON; //LCD backlight on.
			}
			else LED_OFF;

			/*TODO Watchdog*/
		}
}

ISR(TIMER0_COMPA_vect)
{
	//10ms interrupt for time base
	uint16_t n;
//	Timer 1 sec
	n = Timer_1s;
	if (n) Timer_1s = --n;
	n = Timer_menu;
	if (n) Timer_menu = --n;
	n = Timer_blink_option;
	if (n) Timer_blink_option = --n;
	n = Timer_pH;
	if (n) Timer_pH = --n;
}


