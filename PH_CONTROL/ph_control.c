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
		pH_ADC_mid = ADC;
		ADCSRA |= (1<<ADSC);
		double tmp = abs(pH_calib_7pH - pH_calib_4pH);

//		pH_ADC = (3*ph_temp) - (7*pH_calib_4pH) + (4*pH_calib_7pH);

		long double pH_ADC_tmp = abs((3*pH_ADC_mid) - (7*pH_calib_4pH) + (4*pH_calib_7pH));

		pH_ADC_tmp /= tmp;

		pH_all = pH_ADC_tmp*100;

		Timer_pH = 100;
	}
}

