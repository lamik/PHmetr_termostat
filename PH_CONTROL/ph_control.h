/*
 * ph_control.h
 *
 *  Created on: 15 sty 2017
 *      Author: Mateusz Salamon
 */

#ifndef PH_CONTROL_PH_CONTROL_H_
#define PH_CONTROL_PH_CONTROL_H_

uint8_t kH_val;
uint8_t pH_cel_val;
uint8_t pH_fracts_val; //actual values

uint8_t pH_cel;
uint8_t pH_fract; //actual values
uint8_t pH_hist_cel, pH_hist_fract;
uint8_t pH_kryt_cel, pH_kryt_fract;

void Ph_controler_init();

#endif /* PH_CONTROL_PH_CONTROL_H_ */
