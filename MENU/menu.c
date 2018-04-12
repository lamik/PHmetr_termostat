/*
 * menu.c
 *
 *  Created on: 14 sty 2017
 *      Author: Mateusz Salamon
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <stdlib.h>
#include <math.h>

#include "menu.h"
#include "../LCD_buf/lcd44780.h"
#include "../ENCODER/encoder.h"
#include "../Termostat/termostat.h"
#include "../PH_CONTROL/ph_control.h"
#include "../SETTINGS/settings.h"

#define BLINKING 20;

extern uint8_t cel, cel_fract_bits;	//zmienne temperaturowe
uint16_t menu_time = 500;
uint8_t blynk;
extern TSettings settings;

void menu_main()
{

/*****Temp******/
	lcd_cls();
	lcd_int(cel);
	lcd_char('.');
	lcd_int(cel_fract_bits);
	lcd_char(0x00);
	lcd_str("C   ");
/****PH*****/
//lcd_locate(0,7);
//	lcd_int(pH_all);
	if(pH_cel_val < 14)
	{
	if(pH_cel_val < 10) lcd_char(' ');;
	lcd_int(pH_cel_val);
	lcd_char('.');
	if(pH_fracts_val < 10) lcd_char('0');
	lcd_int(pH_fracts_val);
	}
	else
	{
		lcd_str("!!!!!");
	}
	lcd_str("pH");
//	lcd_locate(1,9);
//	lcd_int(pH_ADC_mid);

/***CO2***/
	float co2_ppm = 3 * kH_val;
	co2_ppm *= pow(10,7-pH_float);
	lcd_locate(1,4);
	uint16_t co2_ppm_tmp = co2_ppm;
	if(co2_ppm_tmp < 200)
	{
	if(co2_ppm_tmp < 99) lcd_str(" ");
	if(co2_ppm_tmp < 9) lcd_str(" ");
	lcd_int(co2_ppm_tmp);
	co2_ppm_int = co2_ppm_tmp;
	lcd_char('.');
	co2_ppm *= 10;
	lcd_int( (int)co2_ppm% 10);
	}
	else
	{
		lcd_str("!!!!!");
		co2_ppm_int = 200;
	}
	lcd_str("ppm");

/***STATES***/

	lcd_locate(1,0);
	if(termostat_state == IDLE) lcd_str("---");
	if(termostat_state == HEATING) lcd_str("Grz");
	if(termostat_state == COOLING) {lcd_str("Ch");lcd_char(0x01);}

	lcd_locate(1,13);
	if(pH_state == IDLE_PH) lcd_str("---");
	if(pH_state == CO2) lcd_str("CO2");
	if(pH_state == O2) lcd_str(" O2");


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

	lcd_cls();
	lcd_str("   Warto");
	lcd_char(0x02);
	lcd_char(0x03);
	lcd_str(" KH");
	if(kH_val > 9) lcd_locate(1,6);
	else lcd_locate(1,7);
	lcd_int(kH_val);


	if(enc_left_flag)
	{
		menu_actual = menu_back_to_main;
		Timer_menu = menu_time;
		enc_left_flag = 0;
	}
	if(enc_right_flag)
	{
		menu_actual = menu_pH;
		Timer_menu = menu_time;
		enc_right_flag =0;
	}
	if(enc_sw_flag)
	{
		menu_actual = menu_setkH;
		while(!SW_PRESS); //until release button
		Timer_menu = menu_time;
		enc_sw_flag = 0;
	}
}

void menu_setkH()
{
	if(!Timer_blink_option)
	{
		if(blynk==0) blynk = 1;
		else blynk = 0;
		Timer_blink_option = BLINKING;
	}

	lcd_cls();
	lcd_str("   Warto");
	lcd_char(0x02);
	lcd_char(0x03);
	lcd_str(" KH");
	if(kH_val > 9) lcd_locate(1,6);
	else lcd_locate(1,7);
	if(blynk)
		lcd_int(kH_val);


	if(enc_left_flag)
	{
		if(kH_val > 0)
		{
			kH_val--;
			eeprom_update_byte(&settings.kH, kH_val);
		}
		Timer_menu = menu_time;
		enc_left_flag = 0;
	}
	if(enc_right_flag)
	{
		if(kH_val < 50)
		{
			kH_val++;
			eeprom_update_byte(&settings.kH, kH_val);
		}
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

void menu_pH()
{

	lcd_cls();
	lcd_str("  Ustalone pH");
	if(pH_cel>9) lcd_locate(1,5);
	else lcd_locate(1,6);
	lcd_int(pH_cel);
	lcd_char('.');
	if(pH_fract < 10) lcd_char('0');
	lcd_int(pH_fract);


	if(enc_left_flag)
	{
		menu_actual = menu_kH;
		Timer_menu = menu_time;
		enc_left_flag = 0;
	}
	if(enc_right_flag)
	{
		menu_actual = menu_set_pH_hist;
		Timer_menu = menu_time;
		enc_right_flag =0;
	}
	if(enc_sw_flag)
	{
		menu_actual = menu_set_pH_cel;
		while(!SW_PRESS); //until release button
		Timer_menu = menu_time;
		enc_sw_flag = 0;
	}
}

void menu_set_pH_cel()
{
	if(!Timer_blink_option)
	{
		if(blynk==0) blynk = 1;
		else blynk = 0;
		Timer_blink_option = BLINKING;
	}

	lcd_cls();
	lcd_str("  Ustalone pH");
	if(pH_cel>9) lcd_locate(1,5);
	else lcd_locate(1,6);
	if(blynk)
		lcd_int(pH_cel);
	else
		{
			if(pH_cel>9) lcd_str("  ");
			else lcd_char(' ');
		}
	lcd_char('.');
	lcd_int(pH_fract);


	if(enc_left_flag)
	{
		if(pH_cel > 0)
		{
			pH_cel--;
			eeprom_update_byte(&settings.pH_cel, pH_cel);
		}
		Timer_menu = menu_time;
		enc_left_flag = 0;
	}
	if(enc_right_flag)
	{
		if(pH_cel < 14)
		{
			pH_cel++;
			eeprom_update_byte(&settings.pH_cel, pH_cel);
		}
	Timer_menu = menu_time;
		enc_right_flag =0;
	}
	if(enc_sw_flag)
	{
		menu_actual = menu_set_pH_fract;
		while(!SW_PRESS); //until release button
		Timer_menu = menu_time;
		enc_sw_flag = 0;
	}
}

void menu_set_pH_fract()
{
	if(!Timer_blink_option)
	{
		if(blynk==0) blynk = 1;
		else blynk = 0;
		Timer_blink_option = BLINKING;
	}

	lcd_cls();
	lcd_str("  Ustalone pH");
	if(pH_cel>9) lcd_locate(1,5);
	else lcd_locate(1,6);
	lcd_int(pH_cel);
	lcd_char('.');
	if(blynk)
	{
		if(pH_fract < 10) lcd_char('0');
		lcd_int(pH_fract);
	}
	else lcd_str("  ");


	if(enc_left_flag)
	{
		if(pH_fract > 0)
		{
			pH_fract--;
			eeprom_update_byte(&settings.pH_fract, pH_fract);
		}
		Timer_menu = menu_time;
		enc_left_flag = 0;
	}
	if(enc_right_flag)
	{
		if(pH_fract < 99)
		{
			pH_fract++;
			eeprom_update_byte(&settings.pH_fract, pH_fract);
		}
		Timer_menu = menu_time;
		enc_right_flag =0;
	}
	if(enc_sw_flag)
	{
		menu_actual = menu_pH;
		while(!SW_PRESS); //until release button
		Timer_menu = menu_time;
		enc_sw_flag = 0;
	}
}

void menu_set_pH_hist()
{

	lcd_cls();
	lcd_str("  Histereza pH");
	if(pH_hist_cel>9) lcd_locate(1,5);
	else lcd_locate(1,6);
	lcd_int(pH_hist_cel);
	lcd_char('.');
	if(pH_hist_fract < 10) lcd_char('0');
	lcd_int(pH_hist_fract);


	if(enc_left_flag)
	{
		menu_actual = menu_pH;
		Timer_menu = menu_time;
		enc_left_flag = 0;
	}
	if(enc_right_flag)
	{
		menu_actual = menu_set_pH_kryt;
		Timer_menu = menu_time;
		enc_right_flag =0;
	}
	if(enc_sw_flag)
	{
		menu_actual = menu_set_pH_hist_cel;
		while(!SW_PRESS); //until release button
		Timer_menu = menu_time;
		enc_sw_flag = 0;
	}
}

void menu_set_pH_hist_cel()
{
	if(!Timer_blink_option)
	{
		if(blynk==0) blynk = 1;
		else blynk = 0;
		Timer_blink_option = BLINKING;
	}

	lcd_cls();
	lcd_str("  Histereza pH");
	if(pH_hist_cel>9) lcd_locate(1,5);
	else lcd_locate(1,6);
	if(blynk)
		lcd_int(pH_hist_cel);
	else
		{
			if(pH_hist_cel>9) lcd_str("  ");
			else lcd_char(' ');
		}
	lcd_char('.');
	lcd_int(pH_hist_fract);


	if(enc_left_flag)
	{
		if(pH_hist_cel > 0)
		{
			pH_hist_cel--;
			eeprom_update_byte(&settings.pH_hist_cel, pH_hist_cel);
		}
		Timer_menu = menu_time;
		enc_left_flag = 0;
	}
	if(enc_right_flag)
	{
		if(pH_hist_cel < 14)
		{
			pH_hist_cel++;
			eeprom_update_byte(&settings.pH_hist_cel, pH_hist_cel);
		}
		Timer_menu = menu_time;
		enc_right_flag =0;
	}
	if(enc_sw_flag)
	{
		menu_actual = menu_set_pH_hist_fract;
		while(!SW_PRESS); //until release button
		Timer_menu = menu_time;
		enc_sw_flag = 0;
	}
}

void menu_set_pH_hist_fract()
{
	if(!Timer_blink_option)
	{
		if(blynk==0) blynk = 1;
		else blynk = 0;
		Timer_blink_option = BLINKING;
	}

	lcd_cls();
	lcd_str("  Histereza pH");
	if(pH_hist_cel>9) lcd_locate(1,5);
	else lcd_locate(1,6);
	lcd_int(pH_hist_cel);
	lcd_char('.');
	if(blynk)
	{
		if(pH_hist_fract < 10) lcd_char('0');
		lcd_int(pH_hist_fract);
	}
	else lcd_str("  ");


	if(enc_left_flag)
	{
		if(pH_hist_fract > 0)
		{
			pH_hist_fract--;
			eeprom_update_byte(&settings.pH_hist_fract, pH_hist_fract);
		}
		Timer_menu = menu_time;
		enc_left_flag = 0;
	}
	if(enc_right_flag)
	{
		if(pH_hist_fract < 99)
		{
			pH_hist_fract++;
			eeprom_update_byte(&settings.pH_hist_fract, pH_hist_fract);
		}
		Timer_menu = menu_time;
		enc_right_flag =0;
	}
	if(enc_sw_flag)
	{
		menu_actual = menu_set_pH_hist;
		while(!SW_PRESS); //until release button
		Timer_menu = menu_time;
		enc_sw_flag = 0;
	}
}

void menu_set_pH_kryt()
{

	lcd_cls();
	lcd_str("  Krytyczne pH");
	if(pH_kryt_cel>9) lcd_locate(1,5);
	else lcd_locate(1,6);
	lcd_int(pH_kryt_cel);
	lcd_char('.');
	if(pH_kryt_fract < 10) lcd_char('0');
	lcd_int(pH_kryt_fract);


	if(enc_left_flag)
	{
		menu_actual = menu_set_pH_hist;
		Timer_menu = menu_time;
		enc_left_flag = 0;
	}
	if(enc_right_flag)
	{
		menu_actual = menu_temp;
		Timer_menu = menu_time;
		enc_right_flag =0;
	}
	if(enc_sw_flag)
	{
		menu_actual = menu_set_pH_kryt_cel;
		while(!SW_PRESS); //until release button
		Timer_menu = menu_time;
		enc_sw_flag = 0;
	}
}

void menu_set_pH_kryt_cel()
{
	if(!Timer_blink_option)
	{
		if(blynk==0) blynk = 1;
		else blynk = 0;
		Timer_blink_option = BLINKING;
	}

	lcd_cls();
	lcd_str("  Krytyczne pH");
	if(pH_kryt_cel>9) lcd_locate(1,5);
	else lcd_locate(1,6);
	if(blynk)
		lcd_int(pH_kryt_cel);
	else
		{
			if(pH_kryt_cel>9) lcd_str("  ");
			else lcd_char(' ');
		}
	lcd_char('.');
	lcd_int(pH_kryt_fract);


	if(enc_left_flag)
	{
		if(pH_kryt_cel > 0)
			{
			pH_kryt_cel--;
			eeprom_update_byte(&settings.pH_kryt_cel, pH_kryt_cel);
			}
		Timer_menu = menu_time;
		enc_left_flag = 0;
	}
	if(enc_right_flag)
	{
		if((pH_kryt_cel < 14))
			{
			pH_kryt_cel++;

			if((pH_kryt_cel*10 + pH_kryt_fract) > (pH_cel*10 + pH_fract))
			{
				pH_kryt_cel =  pH_cel;
				pH_kryt_fract = pH_fract;
			}
			eeprom_update_byte(&settings.pH_kryt_cel, pH_kryt_cel);
			}
		Timer_menu = menu_time;
		enc_right_flag =0;
	}
	if(enc_sw_flag)
	{
		menu_actual = menu_set_pH_kryt_fract;
		while(!SW_PRESS); //until release button
		Timer_menu = menu_time;
		enc_sw_flag = 0;
	}
}

void menu_set_pH_kryt_fract()
{
	if(!Timer_blink_option)
	{
		if(blynk==0) blynk = 1;
		else blynk = 0;
		Timer_blink_option = BLINKING;
	}

	lcd_cls();
	lcd_str("  Krytyczne pH");
	if(pH_kryt_cel>9) lcd_locate(1,5);
	else lcd_locate(1,6);
	lcd_int(pH_kryt_cel);
	lcd_char('.');
	if(blynk)
	{
		if(pH_kryt_fract < 10) lcd_char('0');
		lcd_int(pH_kryt_fract);
	}
	else lcd_str("  ");


	if(enc_left_flag)
	{
		if(pH_kryt_fract > 0)
			{
				pH_kryt_fract--;
				eeprom_update_byte(&settings.pH_kryt_fract, pH_kryt_fract);
			}
		Timer_menu = menu_time;
		enc_left_flag = 0;
	}
	if(enc_right_flag)
	{
		if(pH_kryt_fract < 9)
			{
				pH_kryt_fract++;
				if((pH_kryt_cel*10 + pH_kryt_fract) > (pH_cel*10 + pH_fract))
				{
					pH_kryt_cel =  pH_cel;
					pH_kryt_fract = pH_fract;
				}
				eeprom_update_byte(&settings.pH_kryt_fract, pH_kryt_fract);
			}
		Timer_menu = menu_time;
		enc_right_flag =0;
	}
	if(enc_sw_flag)
	{
		menu_actual = menu_set_pH_kryt;
		while(!SW_PRESS); //until release button
		Timer_menu = menu_time;
		enc_sw_flag = 0;
	}
}

void menu_temp()
{

	lcd_cls();
	lcd_str("Ust. temperatura");
	if(termostat_cel>9) lcd_locate(1,5);
	else lcd_locate(1,6);
	lcd_int(termostat_cel);
	lcd_char('.');
	lcd_int(termostat_fract);
	lcd_char(0x00);
	lcd_char('C');


	if(enc_left_flag)
	{
		menu_actual = menu_set_pH_kryt;
		Timer_menu = menu_time;
		enc_left_flag = 0;
	}
	if(enc_right_flag)
	{
		menu_actual = menu_temp_hist;
		Timer_menu = menu_time;
		enc_right_flag =0;
	}
	if(enc_sw_flag)
	{
		menu_actual = menu_set_temp_cel;
		while(!SW_PRESS); //until release button
		Timer_menu = menu_time;
		enc_sw_flag = 0;
	}
}

void menu_set_temp_cel()
{
	if(!Timer_blink_option)
	{
		if(blynk==0) blynk = 1;
		else blynk = 0;
		Timer_blink_option = BLINKING;
	}

	lcd_cls();
	lcd_str("Ust. temperatura");
	if(termostat_cel>9) lcd_locate(1,5);
	else lcd_locate(1,6);
	if(blynk)
		lcd_int(termostat_cel);
	else
		{
			if(termostat_cel>9) lcd_str("  ");
			else lcd_char(' ');
		}
	lcd_char('.');
	lcd_int(termostat_fract);
	lcd_char(0x00);
	lcd_char('C');


	if(enc_left_flag)
	{
		if(termostat_cel > 0)
			{
			termostat_cel--;
			eeprom_update_byte(&settings.termostat_cel, termostat_cel);
			}
		Timer_menu = menu_time;
		enc_left_flag = 0;
	}
	if(enc_right_flag)
	{
		if((termostat_cel < 50))
			{
			termostat_cel++;
			eeprom_update_byte(&settings.termostat_cel, termostat_cel);
			}
		Timer_menu = menu_time;
		enc_right_flag =0;
	}
	if(enc_sw_flag)
	{
		menu_actual = menu_set_temp_fract;
		while(!SW_PRESS); //until release button
		Timer_menu = menu_time;
		enc_sw_flag = 0;
	}
}

void menu_set_temp_fract()
{
	if(!Timer_blink_option)
	{
		if(blynk==0) blynk = 1;
		else blynk = 0;
		Timer_blink_option = BLINKING;
	}

	lcd_cls();
	lcd_str("Ust. temperatura");
	if(termostat_cel>9) lcd_locate(1,5);
	else lcd_locate(1,6);
	lcd_int(termostat_cel);
	lcd_char('.');
	if(blynk)
	lcd_int(termostat_fract);
	else lcd_char(' ');
	lcd_char(0x00);
	lcd_char('C');


	if(enc_left_flag)
	{
		if(termostat_fract > 1)
			{
			termostat_fract--;
			eeprom_update_byte(&settings.termostat_fract, termostat_fract);
			}
		Timer_menu = menu_time;
		enc_left_flag = 0;
	}
	if(enc_right_flag)
	{
		if(termostat_fract < 9)
			{
			termostat_fract++;
			eeprom_update_byte(&settings.termostat_fract, termostat_fract);
			}
		Timer_menu = menu_time;
		enc_right_flag =0;
	}
	if(enc_sw_flag)
	{
		menu_actual = menu_temp;
		while(!SW_PRESS); //until release button
		Timer_menu = menu_time;
		enc_sw_flag = 0;
	}
}

void menu_temp_hist()
{

	lcd_cls();
	lcd_str("Histereza temp.");
	if(termostat_hist_cel>9) lcd_locate(1,5);
	else lcd_locate(1,6);
	lcd_int(termostat_hist_cel);
	lcd_char('.');
	lcd_int(termostat_hist_fract);
	lcd_char(0x00);
	lcd_char('C');


	if(enc_left_flag)
	{
		menu_actual = menu_temp;
		Timer_menu = menu_time;
		enc_left_flag = 0;
	}
	if(enc_right_flag)
	{
		menu_actual = menu_temp_kryt;
		Timer_menu = menu_time;
		enc_right_flag =0;
	}
	if(enc_sw_flag)
	{
		menu_actual = menu_set_temp_hist_cel;
		while(!SW_PRESS); //until release button
		Timer_menu = menu_time;
		enc_sw_flag = 0;
	}
}

void menu_set_temp_hist_cel()
{
	if(!Timer_blink_option)
	{
		if(blynk==0) blynk = 1;
		else blynk = 0;
		Timer_blink_option = BLINKING;
	}

	lcd_cls();
	lcd_str("Histereza temp.");
	if(termostat_hist_cel>9) lcd_locate(1,5);
	else lcd_locate(1,6);
	if(blynk)
		lcd_int(termostat_hist_cel);
	else
		{
			if(termostat_hist_cel>9) lcd_str("  ");
			else lcd_char(' ');
		}
	lcd_char('.');
	lcd_int(termostat_hist_fract);
	lcd_char(0x00);
	lcd_char('C');


	if(enc_left_flag)
	{
		if(termostat_hist_cel > 0)
			{
			termostat_hist_cel--;
			eeprom_update_byte(&settings.termostat_hist_cel, termostat_hist_cel);
			}
		Timer_menu = menu_time;
		enc_left_flag = 0;
	}
	if(enc_right_flag)
	{
		if((termostat_hist_cel < 50))
			{
			termostat_hist_cel++;
			eeprom_update_byte(&settings.termostat_hist_cel, termostat_hist_cel);
			}
		Timer_menu = menu_time;
		enc_right_flag =0;
	}
	if(enc_sw_flag)
	{
		menu_actual = menu_set_temp_hist_fract;
		while(!SW_PRESS); //until release button
		Timer_menu = menu_time;
		enc_sw_flag = 0;
	}
}

void menu_set_temp_hist_fract()
{
	if(!Timer_blink_option)
	{
		if(blynk==0) blynk = 1;
		else blynk = 0;
		Timer_blink_option = BLINKING;
	}

	lcd_cls();
	lcd_str("Histereza temp.");
	if(termostat_hist_cel>9) lcd_locate(1,5);
	else lcd_locate(1,6);
	lcd_int(termostat_hist_cel);
	lcd_char('.');
	if(blynk)
	lcd_int(termostat_hist_fract);
	else lcd_char(' ');
	lcd_char(0x00);
	lcd_char('C');


	if(enc_left_flag)
	{
		if(termostat_hist_fract > 1)
			{
			termostat_hist_fract--;
			eeprom_update_byte(&settings.termostat_hist_fract, termostat_hist_fract);
			}
		Timer_menu = menu_time;
		enc_left_flag = 0;
	}
	if(enc_right_flag)
	{
		if(termostat_hist_fract < 9)
			{
			termostat_hist_fract++;
			eeprom_update_byte(&settings.termostat_hist_fract, termostat_hist_fract);
			}
		Timer_menu = menu_time;
		enc_right_flag =0;
	}
	if(enc_sw_flag)
	{
		menu_actual = menu_temp_hist;
		while(!SW_PRESS); //until release button
		Timer_menu = menu_time;
		enc_sw_flag = 0;
	}
}

void menu_temp_kryt()
{

	lcd_cls();
	lcd_str("  Kryt. temp.");
	if(termostat_kryt_cel>9) lcd_locate(1,5);
	else lcd_locate(1,6);
	lcd_int(termostat_kryt_cel);
	lcd_char('.');
	lcd_int(termostat_kryt_fract);
	lcd_char(0x00);
	lcd_char('C');


	if(enc_left_flag)
	{
		menu_actual = menu_temp_hist;
		Timer_menu = menu_time;
		enc_left_flag = 0;
	}
	if(enc_right_flag)
	{
		menu_actual = menu_calibrate_probe;
		Timer_menu = menu_time;
		enc_right_flag =0;
	}
	if(enc_sw_flag)
	{
		menu_actual = menu_set_temp_kryt_cel;
		while(!SW_PRESS); //until release button
		Timer_menu = menu_time;
		enc_sw_flag = 0;
	}
}

void menu_set_temp_kryt_cel()
{
	if(!Timer_blink_option)
	{
		if(blynk==0) blynk = 1;
		else blynk = 0;
		Timer_blink_option = BLINKING;
	}

	lcd_cls();
	lcd_str("  Kryt. temp.");
	if(termostat_kryt_cel>9) lcd_locate(1,5);
	else lcd_locate(1,6);
	if(blynk)
		lcd_int(termostat_kryt_cel);
	else
		{
			if(termostat_kryt_cel>9) lcd_str("  ");
			else lcd_char(' ');
		}
	lcd_char('.');
	lcd_int(termostat_kryt_fract);
	lcd_char(0x00);
	lcd_char('C');


	if(enc_left_flag)
	{
		if(termostat_kryt_cel > 0)
			{
			termostat_kryt_cel--;
			if((termostat_kryt_cel*10 + termostat_kryt_fract) < (termostat_cel*10 + termostat_fract))
			{
				termostat_kryt_cel =  termostat_cel;
				termostat_kryt_fract = termostat_fract;
			}
			eeprom_update_byte(&settings.termostat_kryt_cel, termostat_kryt_cel);
			}
		Timer_menu = menu_time;
		enc_left_flag = 0;
	}
	if(enc_right_flag)
	{
		if((termostat_kryt_cel < 50))
			{
			termostat_kryt_cel++;
			eeprom_update_byte(&settings.termostat_kryt_cel, termostat_kryt_cel);
			}
		Timer_menu = menu_time;
		enc_right_flag =0;
	}
	if(enc_sw_flag)
	{
		menu_actual = menu_set_temp_kryt_fract;
		while(!SW_PRESS); //until release button
		Timer_menu = menu_time;
		enc_sw_flag = 0;
	}
}

void menu_set_temp_kryt_fract()
{
	if(!Timer_blink_option)
	{
		if(blynk==0) blynk = 1;
		else blynk = 0;
		Timer_blink_option = BLINKING;
	}

	lcd_cls();
	lcd_str("  Kryt. temp.");
	if(termostat_kryt_cel>9) lcd_locate(1,5);
	else lcd_locate(1,6);
	lcd_int(termostat_kryt_cel);
	lcd_char('.');
	if(blynk)
	lcd_int(termostat_kryt_fract);
	else lcd_char(' ');
	lcd_char(0x00);
	lcd_char('C');


	if(enc_left_flag)
	{
		if(termostat_kryt_fract > 0)
			{
			termostat_kryt_fract--;
			if((termostat_kryt_cel*10 + termostat_kryt_fract) < (termostat_cel*10 + pH_fract))
			{
				termostat_kryt_cel =  termostat_cel;
				termostat_kryt_fract = termostat_fract;
			}
				eeprom_update_byte(&settings.termostat_kryt_fract, termostat_kryt_fract);
			}
		Timer_menu = menu_time;
		enc_left_flag = 0;
	}
	if(enc_right_flag)
	{
		if(termostat_kryt_fract < 9)
			{
				termostat_kryt_fract++;
				eeprom_update_byte(&settings.termostat_kryt_fract, termostat_kryt_fract);
			}
		Timer_menu = menu_time;
		enc_right_flag =0;
	}
	if(enc_sw_flag)
	{
		menu_actual = menu_temp_kryt;
		while(!SW_PRESS); //until release button
		Timer_menu = menu_time;
		enc_sw_flag = 0;
	}
}

void menu_calibrate_probe()
{

	lcd_cls();
	lcd_str(" ==Kalibracja==");
	lcd_locate(1,0);
	lcd_str(" ===sondy pH===");


	if(enc_left_flag)
	{
		menu_actual = menu_temp_kryt;
		Timer_menu = menu_time;
		enc_left_flag = 0;
	}
	if(enc_right_flag)
	{
		menu_actual = menu_showing_time;
		Timer_menu = menu_time;
		enc_right_flag =0;
	}
	if(enc_sw_flag)
	{
		menu_actual = menu_calibrate_probe_sure_no;
		while(!SW_PRESS); //until release button
		Timer_menu = menu_time;
		enc_sw_flag = 0;
	}
}

void menu_calibrate_probe_sure_no()
{
	if(!Timer_blink_option)
	{
		if(blynk==0) blynk = 1;
		else blynk = 0;
		Timer_blink_option = BLINKING;
	}

	lcd_cls();
	lcd_str(" Jeste");
	lcd_char(0x02);
	lcd_str(" pewien?");
	lcd_locate(1,6);
	if(blynk)
		lcd_str("Nie");


	if(enc_left_flag)
	{
		menu_actual = menu_calibrate_probe_sure_yes;
		Timer_menu = menu_time;
		enc_left_flag = 0;
	}
	if(enc_right_flag)
	{
		menu_actual = menu_calibrate_probe_sure_yes;
		Timer_menu = menu_time;
		enc_right_flag =0;
	}
	if(enc_sw_flag)
	{
		menu_actual = menu_calibrate_probe;
		while(!SW_PRESS); //until release button
		Timer_menu = menu_time;
		enc_sw_flag = 0;
	}
}

void menu_calibrate_probe_sure_yes()
{
	if(!Timer_blink_option)
	{
		if(blynk==0) blynk = 1;
		else blynk = 0;
		Timer_blink_option = BLINKING;
	}

	lcd_cls();
	lcd_str(" Jeste");
	lcd_char(0x02);
	lcd_str(" pewien?");
	lcd_locate(1,3);
	if(blynk)
		lcd_str("!!!TAK!!!");


	if(enc_left_flag)
	{
		menu_actual = menu_calibrate_probe_sure_no;
		Timer_menu = menu_time;
		enc_left_flag = 0;
	}
	if(enc_right_flag)
	{
		menu_actual = menu_calibrate_probe_sure_no;
		Timer_menu = menu_time;
		enc_right_flag =0;
	}
	if(enc_sw_flag)
	{
		menu_actual = menu_calibrate_probe_down;
		while(!SW_PRESS); //until release button
		Timer_menu = menu_time;
		enc_sw_flag = 0;
	}
}

void menu_calibrate_probe_down()
{

	lcd_cls();
	lcd_str("  Odczyn 4 pH");
	lcd_locate(1,6);
	if(pH_ADC_mid < 99) lcd_str(" ");
	if(pH_ADC_mid < 9) lcd_str(" ");
	lcd_int(pH_ADC_mid);



	Timer_menu = menu_time;

	if(enc_left_flag)
	{
		enc_left_flag = 0;
	}
	if(enc_right_flag)
	{
		enc_right_flag =0;
	}
	if(enc_sw_flag)
	{
		menu_actual = menu_calibrate_probe_up;

		/*WRITE DOWN ADC VALUE TO VARIABLE*/
		pH_calib_4pH = pH_ADC_mid;
		eeprom_update_word(&settings.pH_calib_4pH, pH_calib_4pH);

		while(!SW_PRESS); //until release button
		Timer_menu = menu_time;
		enc_sw_flag = 0;
	}
}

void menu_calibrate_probe_up()
{

	lcd_cls();
	lcd_str("  Odczyn 7 pH");
	lcd_locate(1,6);
	if(pH_ADC_mid < 99) lcd_str(" ");
	if(pH_ADC_mid < 9) lcd_str(" ");
	lcd_int(pH_ADC_mid);


	Timer_menu = menu_time;

	if(enc_left_flag)
	{
		enc_left_flag = 0;
	}
	if(enc_right_flag)
	{
		enc_right_flag =0;
	}
	if(enc_sw_flag)
	{
		menu_actual = menu_calibrate_probe;

		/*WRITE UP ADC VALUE TO VARIABLE*/
		pH_calib_7pH = pH_ADC_mid;
		eeprom_update_word(&settings.pH_calib_7pH, pH_calib_7pH);

		while(!SW_PRESS); //until release button
		Timer_menu = menu_time;
		enc_sw_flag = 0;
	}
}

void menu_showing_time()
{

	lcd_cls();
	lcd_str(" Menu i pod");
	lcd_char(0x02);
	lcd_str("w.");
	if(menu_time>900) lcd_locate(1,5);
	else lcd_locate(1,6);
	lcd_int(menu_time/100);
	lcd_str(" sec");


	if(enc_left_flag)
	{
		menu_actual = menu_calibrate_probe;
		Timer_menu = menu_time;
		enc_left_flag = 0;
	}
	if(enc_right_flag)
	{
		menu_actual = menu_back_to_main;
		Timer_menu = menu_time;
		enc_right_flag =0;
	}
	if(enc_sw_flag)
	{
		menu_actual = menu_set_showing_time;
		while(!SW_PRESS); //until release button
		Timer_menu = menu_time;
		enc_sw_flag = 0;
	}
}

void menu_set_showing_time()
{
	if(!Timer_blink_option)
	{
		if(blynk==0) blynk = 1;
		else blynk = 0;
		Timer_blink_option = BLINKING;
	}

	lcd_cls();
	lcd_str("   Czas menu");
	if(menu_time>900) lcd_locate(1,5);
	else lcd_locate(1,6);
	if(blynk)
		lcd_int(menu_time/100);
	else
	{
		if(menu_time>900) lcd_str("  ");
		else lcd_char(' ');
	}
	lcd_str(" sec");


	if(enc_left_flag)
	{
		if(menu_time > 400)
		{
			menu_time -= 100;
			eeprom_update_word(&settings.menu_time, menu_time);
		}
		Timer_menu = menu_time;
		enc_left_flag = 0;
	}
	if(enc_right_flag)
	{
		if(menu_time < 5000)
		{
			menu_time += 100;
			eeprom_update_word(&settings.menu_time, menu_time);
		}
		Timer_menu = menu_time;
		enc_right_flag =0;
	}
	if(enc_sw_flag)
	{
		menu_actual = menu_showing_time;
		while(!SW_PRESS); //until release button
		Timer_menu = menu_time;
		enc_sw_flag = 0;
	}
}


void menu_back_to_main()
{

	lcd_cls();
	lcd_str("Powr");
	lcd_char(0x04);
	lcd_str("t do ekranu");
	lcd_locate(1,4);
	lcd_char('g');
	lcd_char(0x01);
	lcd_char(0x04);
	lcd_str("wnego");


	if(enc_left_flag)
	{
		menu_actual = menu_showing_time;
		Timer_menu = menu_time;
		enc_left_flag = 0;
	}
	if(enc_right_flag)
	{
		menu_actual = menu_kH;
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
