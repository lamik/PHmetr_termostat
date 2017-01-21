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
	uint8_t pH_cel;
	uint8_t pH_fract;
	uint8_t pH_hist_cel;
	uint8_t pH_hist_fract;
	uint8_t pH_kryt_cel;
	uint8_t pH_kryt_fract;
	uint16_t pH_calib_4pH;
	uint16_t pH_calib_7pH;

}TSettings;



#endif /* SETTINGS_SETTINGS_H_ */
