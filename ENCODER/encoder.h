/*
 * encoder.h
 *
 *  Created on: 14 sty 2017
 *      Author: Mateusz Salamon
 */

#ifndef ENCODER_ENCODER_H_
#define ENCODER_ENCODER_H_

/*
 * encoder.h
 *
 *  Created on: 14.01.2017
 *      Author: Mateusz Salamon
 */
// definicja pinów enkodera
#define ENC_PH_A 	(1<<PD3)
#define ENC_PH_B 	(1<<PD2)
#define ENC_PORT 	PORTD
#define ENC_PIN		PIND

// definicja pinu przycisku enkodera
#define ENC_SW		(1<<PD4)
#define ENC_SWPORT	PORTD
#define ENC_SWPIN	PIND

//-----------------------------------

// wybór timera 0 lub 2
#define TIMER		2
//-----------------------------------

#define PHASE_A         (ENC_PIN & ENC_PH_A)
#define PHASE_B         (ENC_PIN & ENC_PH_B)
#define SW_PRESS		(ENC_SWPIN & ENC_SW)

#if TIMER == 0
	#define ENCODER_vect TIMER0_COMP_vect
#endif

#if TIMER == 2
	#define ENCODER_vect TIMER2_COMPA_vect
#endif

volatile uint8_t enc_left_flag, enc_right_flag, enc_sw_flag;

void encoder_init( uint8_t on_off );



#endif /* ENCODER_ENCODER_H_ */
