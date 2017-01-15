/*
 * termostat.h
 *
 *  Created on: 15 sty 2017
 *      Author: Mateusz Salamon
 */

#ifndef TERMOSTAT_TERMOSTAT_H_
#define TERMOSTAT_TERMOSTAT_H_
#include <avr/io.h>
#include "../DS18X20/ds18x20.h"

#define HEATER 			(1<<PC1)
#define HEATER_PORT		PORTC
#define HEATER_DDR		DDRC

#define COOLER 			(1<<PC2)
#define COOLER_PORT		PORTC
#define COOLER_DDR		DDRC

#define HEATER_ON		HEATER_PORT &= ~HEATER
#define HEATER_OFF		HEATER_PORT |= HEATER
#define COOLER_ON		COOLER_PORT &= ~COOLER
#define COOLER_OFF		COOLER_PORT |= COOLER

enum term_state {IDLE, HEATING, COOLING};

uint8_t termostat_state;


uint8_t termostat_cel, termostat_fract;
uint8_t termostat_hist_cel, termostat_hist_fract;
uint8_t termostat_kryt_cel, termostat_kryt_fract;

void Termostat_init();

void Termostat_get_temperature();
void Termostat();

#endif /* TERMOSTAT_TERMOSTAT_H_ */
