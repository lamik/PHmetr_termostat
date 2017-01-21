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

void Ph_controler_init()
{
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
		float pH_ADC_tmp = abs((3*pH_ADC_mid) - (7*pH_calib_4pH) + (4*pH_calib_7pH));

		pH_ADC_tmp /= tmp;
		pH_all = pH_ADC_tmp*100;

		Timer_pH = 25;
	}
}

