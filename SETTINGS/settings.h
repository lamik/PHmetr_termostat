/*
 * settings.h
 *
 *  Created on: 15 sty 2017
 *      Author: Mateusz Salamon
 */

#ifndef SETTINGS_SETTINGS_H_
#define SETTINGS_SETTINGS_H_

typedef struct Settings{
	uint16_t menu_time;
	uint8_t kH;
	uint8_t termostat_cel;
	uint8_t termostat_fract;
	uint8_t termostat_hist_cel;
	uint8_t termostat_hist_fract;
	uint8_t termostat_kryt_cel;
	uint8_t termostat_kryt_fract;

}TSettings;



#endif /* SETTINGS_SETTINGS_H_ */
