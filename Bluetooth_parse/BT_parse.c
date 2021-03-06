/*
 * BT_parse.c
 *
 *  Created on: 24 lut 2017
 *      Author: Mateusz Salamon
 */

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <stdlib.h>
#include <string.h>

#include "../UART_IRQ/mkuart.h"
#include "BT_parse.h"
#include "../LCD_buf/lcd44780.h"
#include "../Termostat/termostat.h"
#include "../PH_CONTROL/ph_control.h"
#include "../SETTINGS/settings.h"

extern TSettings settings;
extern uint8_t cel, cel_fract_bits;	//zmienne temperaturowe



#define AT_CNT 	10	// ilo� polece� AT

//----------- tablica z poleceniami AT i wska�nikami funkcji do ich obs�ugi --------------------
const TATCMD polecenia_at[AT_CNT] PROGMEM =
{
	// { at_cmd } , { wska�nik do funkcji obs�ugi at },
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

		if ( strpbrk(pBuf, "=?"))
		{
			// obs�uga polece� AT we/wy + parametry

			if ( strpbrk(pBuf, "?"))
			{
				// zapytania do uk�adu w postaci: AT+CMD?

				cmd_wsk = strtok_r(pBuf, "?", &reszta);
				len = strlen(cmd_wsk);
				for(i=0;i<AT_CNT;i++)
				{
					if ( len && 0 == strncasecmp_P(cmd_wsk, polecenia_at[i].polecenie_at, len) )
					{
						if( pgm_read_word(polecenia_at[i].polecenie_at) ) { // <--- UWAGA! w tek�cie ksi��ki zabrak�o pgm_read_word()
							_at_srv = (void *)pgm_read_word( &polecenia_at[i].at_service );
							if( _at_srv)
							{
								if( _at_srv( 0, reszta ) < 0 ) uart_puts("ERROR\r\n");
							}
						}
						uart_puts("\r\n");
						break;
					}
				}

			}
			else
			{
				// ustawienia uk�adu w postaci: AT+CMD=parametry

				cmd_wsk = strtok_r(pBuf, "=", &reszta);
				len = strlen(cmd_wsk);
				for(i=0;i<AT_CNT;i++)
				{
					if ( len && 0 == strncasecmp_P(cmd_wsk, polecenia_at[i].polecenie_at, len) )
					{
						if( pgm_read_word(polecenia_at[i].polecenie_at) )
						{ // <--- UWAGA! w tek�cie ksi��ki zabrak�o pgm_read_word()
							_at_srv = (void *)pgm_read_word( &polecenia_at[i].at_service );
							if( _at_srv && ! _at_srv( 1, reszta ) ) uart_puts("OK\r\n");
							else uart_puts("ERROR\r\n");
						}
						break;
					}
				}
			}
		}
		else
		{
			// obs�uga polece� AT bez parametr�w

			if( 0 == pBuf[0] ) uart_puts("\r\n");	// reakcja na znak CR lub CRLF z terminala
			else
			{
				for(i=0;i<AT_CNT;i++)
				{
					if ( 0 == strncasecmp_P(pBuf, polecenia_at[i].polecenie_at, strlen(pBuf)) )
					{
						if( pgm_read_word(polecenia_at[i].polecenie_at) )
						{
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
	// AT
	uart_puts("OK\r\n");
	return 0;
}

int8_t ati_service(uint8_t inout, char * params)
{
	//	ATI
	uart_puts("Mateusz Salamon pH Controller\r\n");
	return 0;
}

int8_t at_inf_service(uint8_t inout, char * params)
{
	//	AT+INF
	//	First actual parameters, thes settings
	//	actual: temperature, pH, states(Thermostat outs, pH outs), CO2 ppm
	//	settings: KH, ther, therm hist, therm crit, ph, ph hist, ph crit
	//
	//	NO info for: menu time, calibration.
	//
	//	Message structure:
	//	+INF:xx.x,xx.xx,x,x,xxx,xx,xx.x,xx.x,xx.x,xx.x,xx.x,xx.x\r
	char tmp[5];

	uart_puts("+INF:");
	uart_puts(itoa(cel,tmp,10));
	uart_putc('.');
	uart_puts(itoa(cel_fract_bits,tmp,10));
	uart_putc(',');
	uart_puts(itoa(pH_cel_val,tmp,10));
	uart_putc('.');
	uart_puts(itoa(pH_fracts_val,tmp,10));
	uart_putc(',');
	uart_puts(itoa(termostat_state,tmp,10));
	uart_putc(',');
	uart_puts(itoa(pH_state,tmp,10));
	uart_putc(',');
	uart_puts(itoa(co2_ppm_int,tmp,10));
	uart_putc(',');
	uart_puts(itoa(kH_val,tmp,10));
	uart_putc(',');
	uart_puts(itoa(termostat_cel,tmp,10));
	uart_putc('.');
	uart_puts(itoa(termostat_fract,tmp,10));
	uart_putc(',');
	uart_puts(itoa(termostat_hist_cel,tmp,10));
	uart_putc('.');
	uart_puts(itoa(termostat_hist_fract,tmp,10));
	uart_putc(',');
	uart_puts(itoa(termostat_kryt_cel,tmp,10));
	uart_putc('.');
	uart_puts(itoa(termostat_kryt_fract,tmp,10));
	uart_putc(',');
	uart_puts(itoa(pH_cel,tmp,10));
	uart_putc('.');
	uart_puts(itoa(pH_fract,tmp,10));
	uart_putc(',');
	uart_puts(itoa(pH_hist_cel,tmp,10));
	uart_putc('.');
	uart_puts(itoa(pH_hist_fract,tmp,10));
	uart_putc(',');
	uart_puts(itoa(pH_kryt_cel,tmp,10));
	uart_putc('.');
	uart_puts(itoa(pH_kryt_fract,tmp,10));
	uart_putc('\r');

	return 0;
}

int8_t at_kh_service(uint8_t inout, char * params)
{
	//	AT+KH
	if( 1 == inout ) {
		if(!strlen(params)) return -1;
		int8_t tmp = atoi(params);
		if(!(tmp <= 50 &&  tmp >= 0)) return -1;
		kH_val = atoi(params);
		eeprom_update_byte(&settings.kH, kH_val);
	}
	else if( 2 == inout )
	{
		uart_puts("AT+KH = (0-50)\r\n");
	}
	else if(!inout)
	{
		uart_puts(itoa(kH_val,NULL,10));
	}

	return 0;
}

int8_t at_tmp_service(uint8_t inout, char * params)
{
	uint8_t cel,fracts;
	char * wsk;
	//	AT+TMP
	if( 1 == inout ) {
		if(!strlen(params)) return -1;

		wsk = strtok(params, ".");
		if(!strlen(wsk)) return -1;
		cel = atoi(wsk);
		if(!(cel <= 50 &&  cel >= 0)) return -1;

		wsk = strtok(NULL, ",");
		if(!strlen(wsk)) return -1;
		fracts = atoi(wsk);
		if(!(fracts <= 9 &&  fracts >= 0)) return -1;

		termostat_cel = cel;
		termostat_fract = fracts;
		eeprom_update_byte(&settings.termostat_cel, termostat_cel);
		eeprom_update_byte(&settings.termostat_fract, termostat_fract);
	}
	else if( 2 == inout )
	{
		uart_puts("AT+TMP = (0-50).(0-9)\r\n");
	}
	else if(!inout)
	{
		uart_puts(itoa(termostat_cel,NULL,10));
		uart_putc('.');
		uart_puts(itoa(termostat_fract,NULL,10));
	}
	return 0;
}

int8_t at_tmh_service( uint8_t inout, char * params )
{
	uint8_t cel,fracts;
	char * wsk;
	//	AT+TMH
	if( 1 == inout ) {
		if(!strlen(params)) return -1;

		wsk = strtok(params, ".");
		if(!strlen(wsk)) return -1;
		cel = atoi(wsk);
		if(!(cel <= 50 &&  cel >= 0)) return -1;

		wsk = strtok(NULL, ",");
		if(!strlen(wsk)) return -1;
		fracts = atoi(wsk);
		if(!(fracts <= 9 &&  fracts >= 0)) return -1;

		termostat_hist_cel = cel;
		termostat_hist_fract = fracts;
		eeprom_update_byte(&settings.termostat_hist_cel, termostat_hist_cel);
		eeprom_update_byte(&settings.termostat_hist_fract, termostat_hist_fract);
	}
	else if( 2 == inout )
	{
		uart_puts("AT+TMH = (0-50).(0-9)\r\n");
	}
	else if(!inout)
	{
		uart_puts(itoa(termostat_hist_cel,NULL,10));
		uart_putc('.');
		uart_puts(itoa(termostat_hist_fract,NULL,10));
	}
	return 0;
}

int8_t at_tmk_service(uint8_t inout, char * params)
{
	uint8_t cel,fracts;
	char * wsk;
	//	AT+TMK
	if( 1 == inout ) {
		if(!strlen(params)) return -1;

		wsk = strtok(params, ".");
		if(!strlen(wsk)) return -1;
		cel = atoi(wsk);
		if(!(cel <= 50 &&  cel >= 0)) return -1;

		wsk = strtok(NULL, ",");
		if(!strlen(wsk)) return -1;
		fracts = atoi(wsk);
		if(!(fracts <= 9 &&  fracts >= 0)) return -1;

		termostat_kryt_cel = cel;
		termostat_kryt_fract = fracts;
		eeprom_update_byte(&settings.termostat_kryt_cel, termostat_kryt_cel);
		eeprom_update_byte(&settings.termostat_kryt_fract, termostat_kryt_fract);
	}
	else if( 2 == inout )
	{
		uart_puts("AT+TMK = (0-50).(0-9)\r\n");
	}
	else if(!inout)
	{
		uart_puts(itoa(termostat_kryt_cel,NULL,10));
		uart_putc('.');
		uart_puts(itoa(termostat_kryt_fract,NULL,10));
	}
	return 0;
}

int8_t at_php_service(uint8_t inout, char * params)
{
	uint8_t cel,fracts;
	char * wsk;
	//	AT+PH
	if( 1 == inout ) {
		if(!strlen(params)) return -1;

		wsk = strtok(params, ".");
		if(!strlen(wsk)) return -1;
		cel = atoi(wsk);
		if(!(cel <= 14 &&  cel >= 0)) return -1;

		wsk = strtok(NULL, ",");
		if(!strlen(wsk)) return -1;
		fracts = atoi(wsk);
		if(!(fracts <= 99 &&  fracts >= 0)) return -1;

		pH_cel = cel;
		pH_fract = fracts;
		eeprom_update_byte(&settings.pH_cel, pH_cel);
		eeprom_update_byte(&settings.pH_fract, pH_fract);
	}
	else if( 2 == inout )
	{
		uart_puts("AT+PH = (0-14).(0-99)\r\n");
	}
	else if(!inout)
	{
		uart_puts(itoa(pH_cel,NULL,10));
		uart_putc('.');
		uart_puts(itoa(pH_fract,NULL,10));
	}
	return 0;
}

int8_t at_phh_service(uint8_t inout, char * params)
{
	uint8_t cel,fracts;
	char * wsk;
	//	AT+PHH
	if( 1 == inout ) {
		if(!strlen(params)) return -1;

		wsk = strtok(params, ".");
		if(!strlen(wsk)) return -1;
		cel = atoi(wsk);
		if(!(cel <= 14 &&  cel >= 0)) return -1;

		wsk = strtok(NULL, ",");
		if(!strlen(wsk)) return -1;
		fracts = atoi(wsk);
		if(!(fracts <= 99 &&  fracts >= 0)) return -1;

		pH_hist_cel = cel;
		pH_hist_fract = fracts;
		eeprom_update_byte(&settings.pH_hist_cel, pH_hist_cel);
		eeprom_update_byte(&settings.pH_hist_fract, pH_hist_fract);
	}
	else if( 2 == inout )
	{
		uart_puts("AT+PHH = (0-14).(0-99)\r\n");
	}
	else if(!inout)
	{
		uart_puts(itoa(pH_hist_cel,NULL,10));
		uart_putc('.');
		uart_puts(itoa(pH_hist_fract,NULL,10));
	}
	return 0;
}

int8_t at_phk_service( uint8_t inout, char * params )
{
	uint8_t cel,fracts;
	char * wsk;
	//	AT+PHK
	if( 1 == inout ) {
		if(!strlen(params)) return -1;

		wsk = strtok(params, ".");
		if(!strlen(wsk)) return -1;
		cel = atoi(wsk);
		if(!(cel <= 14 &&  cel >= 0)) return -1;

		wsk = strtok(NULL, ",");
		if(!strlen(wsk)) return -1;
		fracts = atoi(wsk);
		if(!(fracts <= 99 &&  fracts >= 0)) return -1;

		pH_kryt_cel = cel;
		pH_kryt_fract = fracts;
		eeprom_update_byte(&settings.pH_kryt_cel, pH_kryt_cel);
		eeprom_update_byte(&settings.pH_kryt_fract, pH_kryt_fract);
	}
	else if( 2 == inout )
	{
		uart_puts("AT+PHK = (0-14).(0-99)\r\n");
	}
	else if(!inout)
	{
		uart_puts(itoa(pH_kryt_cel,NULL,10));
		uart_putc('.');
		uart_puts(itoa(pH_kryt_fract,NULL,10));
	}
	return 0;
	return 0;
}
