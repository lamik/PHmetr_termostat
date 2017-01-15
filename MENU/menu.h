/*
 * menu.h
 *
 *  Created on: 14 sty 2017
 *      Author: Mateusz Salamon
 */

#ifndef MENU_MENU_H_
#define MENU_MENU_H_

void(*menu_actual)(void);
volatile uint16_t Timer_menu;
volatile uint8_t Timer_blink_option;


void menu_main();			//0
void menu_kH();				//1
void menu_setkH();			//2
void menu_pH();				//3
void menu_set_pH_cel();		//4
void menu_set_pH_fract();	//5
void menu_temp();			//6
void menu_set_temp_cel();	//7
void menu_set_temp_fract();	//8
void menu_calibrate_probe();//9
void menu_calibrate_probe_down();//10
void menu_calibrate_probe_up();//11
void menu_back_to_main();	//12
void menu_showing_time();	//13
void menu_set_showing_time();	//14

#endif /* MENU_MENU_H_ */
