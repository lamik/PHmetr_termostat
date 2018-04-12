/*
 * BT_parse.h
 *
 *  Created on: 24 lut 2017
 *      Author: Mateusz Salamon
 */

#ifndef BLUETOOTH_PARSE_BT_PARSE_H_
#define BLUETOOTH_PARSE_BT_PARSE_H_

// definicja typu strukturalnego
typedef struct {
	char polecenie_at[8];
	int8_t (* at_service)(uint8_t inout, char * params);
} TATCMD;


// deklaracje zmiennych zewnêtrznych
extern const TATCMD polecenia_at[];

void parse_uart_data( char * pBuf );

int8_t at_service(uint8_t inout, char * params);
int8_t ati_service(uint8_t inout, char * params);
int8_t at_inf_service(uint8_t inout, char * params);
int8_t at_kh_service(uint8_t inout, char * params);
int8_t at_tmp_service(uint8_t inout, char * params);
int8_t at_tmh_service( uint8_t inout, char * params );
int8_t at_tmk_service(uint8_t inout, char * params);
int8_t at_php_service(uint8_t inout, char * params);
int8_t at_phh_service(uint8_t inout, char * params);
int8_t at_phk_service( uint8_t inout, char * params );

#endif /* BLUETOOTH_PARSE_BT_PARSE_H_ */
