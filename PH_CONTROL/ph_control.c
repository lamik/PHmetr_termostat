/*
 * ph_control.c
 *
 *  Created on: 15 sty 2017
 *      Author: Mateusz Salamon
 */
#include <avr/io.h>
#include <avr/eeprom.h>
#include <stdlib.h>
#include <math.h>

#include "ph_control.h"
#include "../SETTINGS/settings.h"


extern TSettings settings;
uint32_t pH_ADC_mid;
uint16_t pH_all;

void Ph_controler_init()
{
	CO2_VALVE_DDR |= CO2_VALVE;
	CO2_VALVE_OFF;
	O2_VALVE_DDR |= O2_VALVE;
	O2_VALVE_OFF;
	pH_state = IDLE_PH;

	//get all saved parameters
	kH_val = eeprom_read_byte(&(settings.kH));
	pH_cel = eeprom_read_byte(&(settings.pH_cel));
	pH_fract = eeprom_read_byte(&(settings.pH_fract));
	pH_hist_cel = eeprom_read_byte(&(settings.pH_hist_cel));
	pH_hist_fract = eeprom_read_byte(&(settings.pH_hist_fract));
	pH_kryt_cel = eeprom_read_byte(&(settings.pH_kryt_cel));
	pH_kryt_fract = eeprom_read_byte(&(settings.pH_kryt_fract));
	pH_calib_4pH = eeprom_read_word(&(settings.pH_calib_4pH));
	pH_calib_7pH = eeprom_read_word(&(settings.pH_calib_7pH));

	//Init ADC for measurement
	ADCSRA |= (1<<ADEN);  //ADC Enable
	ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); //prescaler se to 128 - 125kHz ADC clock
	ADMUX |= (1<<REFS0); //Vcc as ref voltage, channel ADC0
	ADCSRA |= (1<<ADSC);  //ADC Start Conversion, ADC is in Free Running Mode - always start new conversion when old is read

}

void Ph_controler_get_pH()
{
	if(!Timer_pH)
	{
		static uint16_t adc_mes[10];
		static uint8_t idx;
		uint8_t i;

		adc_mes[idx++] = ADC;
		if(idx>9) idx = 0;

		pH_ADC_mid = 0; //calculate average from 10 mes
		for(i = 0; i<10 ; i++) pH_ADC_mid += adc_mes[i];
		pH_ADC_mid /= 10;

		ADCSRA |= (1<<ADSC);

		float tmp = abs(pH_calib_7pH - pH_calib_4pH);
		pH_float = abs((3*pH_ADC_mid) - (7*pH_calib_4pH) + (4*pH_calib_7pH));

		pH_float /= tmp;

		pH_all = pH_float*100;

		pH_cel_val = pH_float;
		pH_fracts_val = pH_all % 100;

		Timer_pH = 25;
	}
}

void Ph_controler_control_pH()
{

	uint16_t goal_pH_tmp = pH_cel*100+pH_fract;

	uint16_t pH_down = goal_pH_tmp - (pH_hist_cel*100 + pH_hist_fract);
	uint16_t pH_up = goal_pH_tmp + (pH_hist_cel*100 + pH_hist_fract);
	uint16_t pH_kryt = pH_kryt_cel*100 + pH_kryt_fract;

	if(pH_state == IDLE_PH)
	{
		if(pH_all >= pH_up)
		{
			pH_state = CO2;
			CO2_VALVE_ON;
			O2_VALVE_OFF;
		}

		if(pH_all <= pH_kryt)
		{
			pH_state = O2;
			O2_VALVE_ON;
			CO2_VALVE_OFF;
		}
	}
	else if(pH_state == CO2)
	{
		if(pH_all <= pH_down)
		{
			pH_state = IDLE_PH;
			CO2_VALVE_OFF;
			O2_VALVE_OFF;
		}
	}
	else if(pH_state == O2)
	{
		if(pH_all >= pH_down)
		{
			pH_state = IDLE_PH;
			CO2_VALVE_OFF;
			O2_VALVE_OFF;
		}
	}
}

