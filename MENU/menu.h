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


void menu_main();					//0
void menu_kH();						//1
void menu_setkH();					//2

void menu_pH();						//3
void menu_set_pH_cel();				//4
void menu_set_pH_fract();			//5

void menu_set_pH_hist();			//6
void menu_set_pH_hist_cel();		//7
void menu_set_pH_hist_fract();		//8

void menu_set_pH_kryt();			//9
void menu_set_pH_kryt_cel();		//10
void menu_set_pH_kryt_fract();		//11

void menu_temp();					//12
void menu_set_temp_cel();			//13
void menu_set_temp_fract();			//14

void menu_temp_hist();				//15
void menu_set_temp_hist_cel();		//16
void menu_set_temp_hist_fract();	//17

void menu_temp_kryt();				//18
void menu_set_temp_kryt_cel();		//19
void menu_set_temp_kryt_fract();	//20

void menu_calibrate_probe();		//21
void menu_calibrate_probe_sure_no();//22
void menu_calibrate_probe_sure_yes();//23
void menu_calibrate_probe_down();	//24
void menu_calibrate_probe_up();		//25

void menu_showing_time();			//26
void menu_set_showing_time();		//27

void menu_back_to_main();			//28



#endif /* MENU_MENU_H_ */
