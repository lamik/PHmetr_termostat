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
}

