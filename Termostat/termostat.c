/*
 * termostat.c
 *
 *  Created on: 15 sty 2017
 *      Author: Mateusz Salamon
 */
#include "termostat.h"

extern volatile uint8_t Timer_1s;
extern uint8_t subzero, cel, cel_fract_bits;	//zmienne temperaturowe



void Termostat_init()
{

	HEATER_DDR |= HEATER;
	HEATER_OFF;
	COOLER_DDR |= COOLER;
	COOLER_OFF;
	termostat_state = IDLE;

	ds18x20_cnt = search_sensors();
	DS18X20_start_meas(DS18X20_POWER_EXTERN, NULL);

	termostat_cel = 26;
	termostat_fract = 0;
	termostat_hist_cel = 1;
	termostat_hist_fract = 0;
	termostat_kryt_cel = 30;
	termostat_kryt_fract = 0;

}

void Termostat_get_temperature()
{
	if(!Timer_1s)
	{
		DS18X20_read_meas(gSensorIDs[0],&subzero,&cel,&cel_fract_bits);
		DS18X20_start_meas(DS18X20_POWER_EXTERN, NULL);
		Timer_1s = 100;
	}
}

void Termostat()
{
	uint16_t temp_tmp = cel*10 + cel_fract_bits;
	uint16_t goal_temp_tmp = termostat_cel*10+termostat_fract;

	uint16_t temp_down = goal_temp_tmp - (termostat_hist_cel*10 + termostat_hist_fract);
	uint16_t temp_up = goal_temp_tmp + (termostat_hist_cel*10 + termostat_hist_fract);
	uint16_t temp_kryt = termostat_kryt_cel*10 + termostat_kryt_fract;

	if(termostat_state == IDLE)
	{
		if(temp_tmp <= temp_down)
		{
			termostat_state = HEATING;
			HEATER_ON;
			COOLER_OFF;
		}

		if(temp_tmp >= temp_kryt)
		{
			termostat_state = COOLING;
			HEATER_OFF;
			COOLER_ON;
		}
	}
	else if(termostat_state == HEATING)
	{
		if(temp_tmp >= temp_up)
		{
			termostat_state = IDLE;
			HEATER_OFF;
			COOLER_OFF;
		}
	}
	else if(termostat_state == COOLING)
	{
		if(temp_tmp <= temp_up)
		{
			termostat_state = IDLE;
			HEATER_OFF;
			COOLER_OFF;
		}
	}
}


