/*
 * menu.c
 *
 *  Created on: 14 sty 2017
 *      Author: Mateusz Salamon
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>

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
	cli();
	lcd_cls();
	lcd_int(cel);
	lcd_char('.');
	lcd_int(cel_fract_bits);
	lcd_char(0x00);
	lcd_str("C    ");
/*TODO*/
	lcd_int(7);
	lcd_char('.');
	lcd_int(21);
/******/
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
	if(kH_val > 9) lcd_locate(1,6);
	else lcd_locate(1,7);
	lcd_int(kH_val);
	sei();

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
	cli();
	lcd_cls();
	lcd_str("   Warto");
	lcd_char(0x02);
	lcd_char(0x03);
	lcd_str(" KH");
	if(kH_val > 9) lcd_locate(1,6);
	else lcd_locate(1,7);
	if(blynk)
		lcd_int(kH_val);
	sei();

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
	cli();
	lcd_cls();
	lcd_str("  Ustalone pH");
	if(pH_cel>9) lcd_locate(1,5);
	else lcd_locate(1,6);
	lcd_int(pH_cel);
	lcd_char('.');
	lcd_int(pH_fract);
	sei();

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
	cli();
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
	sei();

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
	cli();
	lcd_cls();
	lcd_str("  Ustalone pH");
	if(pH_cel>9) lcd_locate(1,5);
	else lcd_locate(1,6);
	lcd_int(pH_cel);
	lcd_char('.');
	if(blynk)
	lcd_int(pH_fract);
	else lcd_char(' ');
	sei();

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
		if(pH_fract < 9)
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
	cli();
	lcd_cls();
	lcd_str("  Histereza pH");
	if(pH_hist_cel>9) lcd_locate(1,5);
	else lcd_locate(1,6);
	lcd_int(pH_hist_cel);
	lcd_char('.');
	lcd_int(pH_hist_fract);
	sei();

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
	cli();
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
	sei();

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
	cli();
	lcd_cls();
	lcd_str("  Histereza pH");
	if(pH_hist_cel>9) lcd_locate(1,5);
	else lcd_locate(1,6);
	lcd_int(pH_hist_cel);
	lcd_char('.');
	if(blynk)
	lcd_int(pH_hist_fract);
	else lcd_char(' ');
	sei();

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
		if(pH_hist_fract < 9)
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
	cli();
	lcd_cls();
	lcd_str("  Krytyczne pH");
	if(pH_kryt_cel>9) lcd_locate(1,5);
	else lcd_locate(1,6);
	lcd_int(pH_kryt_cel);
	lcd_char('.');
	lcd_int(pH_kryt_fract);
	sei();

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
	cli();
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
	sei();

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
	cli();
	lcd_cls();
	lcd_str("  Krytyczne pH");
	if(pH_kryt_cel>9) lcd_locate(1,5);
	else lcd_locate(1,6);
	lcd_int(pH_kryt_cel);
	lcd_char('.');
	if(blynk)
	lcd_int(pH_kryt_fract);
	else lcd_char(' ');
	sei();

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
	cli();
	lcd_cls();
	lcd_str("Ust. temperatura");
	if(termostat_cel>9) lcd_locate(1,5);
	else lcd_locate(1,6);
	lcd_int(termostat_cel);
	lcd_char('.');
	lcd_int(termostat_fract);
	lcd_char(0x00);
	lcd_char('C');
	sei();

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
	cli();
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
	sei();

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
	cli();
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
	sei();

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
	cli();
	lcd_cls();
	lcd_str("Histereza temp.");
	if(termostat_hist_cel>9) lcd_locate(1,5);
	else lcd_locate(1,6);
	lcd_int(termostat_hist_cel);
	lcd_char('.');
	lcd_int(termostat_hist_fract);
	lcd_char(0x00);
	lcd_char('C');
	sei();

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
	cli();
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
	sei();

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
	cli();
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
	sei();

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
	cli();
	lcd_cls();
	lcd_str("  Kryt. temp.");
	if(termostat_kryt_cel>9) lcd_locate(1,5);
	else lcd_locate(1,6);
	lcd_int(termostat_kryt_cel);
	lcd_char('.');
	lcd_int(termostat_kryt_fract);
	lcd_char(0x00);
	lcd_char('C');
	sei();

	if(enc_left_flag)
	{
		menu_actual = menu_temp_hist;
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
	cli();
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
	sei();

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
	cli();
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
	sei();

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

}

void menu_calibrate_probe_down()
{

}

void menu_calibrate_probe_up()
{

}

void menu_showing_time()
{

}

void menu_set_showing_time()
{

}


void menu_back_to_main()
{
	cli();
	lcd_cls();
	lcd_str("Powr");
	lcd_char(0x04);
	lcd_str("t do ekranu");
	lcd_locate(1,4);
	lcd_char('g');
	lcd_char(0x01);
	lcd_char(0x04);
	lcd_str("wnego");
	sei();

	if(enc_left_flag)
	{

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
