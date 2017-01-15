/*
 * menu.c
 *
 *  Created on: 14 sty 2017
 *      Author: Mateusz Salamon
 */
#include <avr/io.h>
#include <avr/interrupt.h>

#include "menu.h"
#include "../LCD_buf/lcd44780.h"
#include "../ENCODER/encoder.h"
#include "../Termostat/termostat.h"

extern uint8_t cel, cel_fract_bits;	//zmienne temperaturowe
uint16_t menu_time = 300;

void menu_main()
{
	cli();
	lcd_cls();
	lcd_int(cel);
	lcd_char('.');
	lcd_int(cel_fract_bits);
	lcd_char(0x00);
	lcd_str("C    ");
	lcd_int(7);
	lcd_char('.');
	lcd_int(21);
	lcd_str("pH");
	lcd_locate(1,0);
	if(termostat_state == IDLE) lcd_str("Bezczynny");
	if(termostat_state == HEATING) lcd_str("Grzanie");
	if(termostat_state == COOLING) {lcd_str("Ch");lcd_char(0x01);lcd_str("odzenie");}
	sei();

	if(enc_left_flag)
	{
		Timer_menu = menu_time;
		enc_left_flag = 0;
	}
	if(enc_right_flag)
	{
		Timer_menu = menu_time;
		enc_right_flag =0;
	}
	if(enc_sw_flag)
	{
		menu_actual = menu_kH;
		while(!SW_PRESS); //until release button
		Timer_menu = menu_time;
		enc_sw_flag = 0;
	}

}

void menu_kH()
{

	cli();
	lcd_cls();
	lcd_str("   Warto");
	lcd_char(0x02);
	lcd_char(0x03);
	lcd_str(" KH");
	sei();

	if(enc_left_flag)
	{

		Timer_menu = menu_time;
		enc_left_flag = 0;
	}
	if(enc_right_flag)
	{

		Timer_menu = menu_time;
		enc_right_flag =0;
	}
	if(enc_sw_flag)
	{
		menu_actual = menu_main;

		while(!SW_PRESS); //until release button
		Timer_menu = menu_time;
		enc_sw_flag = 0;
	}
}

void menu_setkH()
{

}

void menu_pH()
{

}

void menu_set_pH_cel()
{

}

void menu_set_pH_fract()
{

}

void menu_temp()
{

}

void menu_set_temp_cel()
{

}

void menu_set_temp_fract()
{

}

void menu_calibrate_probe()
{

}

void menu_calibrate_probe_down()
{

}

void menu_calibrate_probe_up()
{

}

void menu_back_to_main()
{

}
