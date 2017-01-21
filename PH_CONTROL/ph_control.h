/*
 * ph_control.h
 *
 *  Created on: 15 sty 2017
 *      Author: Mateusz Salamon
 */

#ifndef PH_CONTROL_PH_CONTROL_H_
#define PH_CONTROL_PH_CONTROL_H_

enum pH_state {IDLE_PH, CO2, O2};
uint8_t pH_state;

volatile uint8_t Timer_pH;

uint8_t kH_val;
uint8_t pH_cel_val;
uint8_t pH_fracts_val; //actual values
uint16_t pH_all;
uint32_t pH_ADC_mid;

uint8_t pH_cel; //set values
uint8_t pH_fract;
uint8_t pH_hist_cel, pH_hist_fract;
uint8_t pH_kryt_cel, pH_kryt_fract;

uint16_t pH_calib_4pH, pH_calib_7pH;

void Ph_controler_init();
void Ph_controler_get_pH();
void Ph_controler_control_pH();

#endif /* PH_CONTROL_PH_CONTROL_H_ */
