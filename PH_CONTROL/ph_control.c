/*
 * ph_control.c
 *
 *  Created on: 15 sty 2017
 *      Author: Mateusz Salamon
 */
#include <avr/io.h>
#include <avr/eeprom.h>

#include "ph_control.h"
#include "../SETTINGS/settings.h"


extern TSettings settings;

void Ph_controler_init()
{
	kH_val = eeprom_read_byte(&(settings.kH));
	pH_cel = eeprom_read_byte(&(settings.pH_cel));
	pH_fract = eeprom_read_byte(&(settings.pH_fract));
	pH_hist_cel = eeprom_read_byte(&(settings.pH_hist_cel));
	pH_hist_fract = eeprom_read_byte(&(settings.pH_hist_fract));
	pH_kryt_cel = eeprom_read_byte(&(settings.pH_kryt_cel));
	pH_kryt_fract = eeprom_read_byte(&(settings.pH_kryt_fract));
}

