/*
 * settings.c
 *
 *  Created on: 15 sty 2017
 *      Author: Mateusz Salamon
 */
#include <avr/io.h>
#include <avr/eeprom.h>

#include "settings.h"

TSettings settings EEMEM = { //user values
	.menu_time = 600, //6 second menu
	.kH = 5,
	.termostat_cel = 24,
	.termostat_fract = 0,
	.termostat_hist_cel = 0,
	.termostat_hist_fract = 3,
	.termostat_kryt_cel = 29,
	.termostat_kryt_fract = 0,
	.pH_cel = 6,
	.pH_fract = 8,
	.pH_hist_cel = 0,
	.pH_hist_fract = 2,
	.pH_kryt_cel = 6,
	.pH_kryt_fract = 2,
};

