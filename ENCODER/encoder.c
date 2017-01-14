/*
 * encoder.c
 *
 *  Created on: 14.01.2017
 *      Author: Mateusz Salamon
 */

#include <avr/io.h>
#include <avr/interrupt.h>

#include "encoder.h"

volatile uint8_t encoder_A, encoder_B, encoder_A_prev=0;

void encoder_init( uint8_t on_off ) {

	if( !on_off ) {
#if TIMER == 0
		TCCR0 &= ~((1<<CS01)|(1<<CS00));        // stop timer0
		TIMSK &= ~(1<<OCIE0);					// disable interrupt
#endif

#if TIMER == 2
		TCCR2B &= ~(1<<CS22);			        // stop timer2
		TIMSK2 &= ~(1<<OCIE2A);					// disable interrupt
#endif
		return;
	}

	ENC_PIN  &= ~(ENC_PH_A|ENC_PH_B);
	ENC_PORT |= ENC_PH_A|ENC_PH_B;

	ENC_SWPIN &= ~ENC_SW;
	ENC_SWPORT |= ENC_SW;


#if TIMER == 0
	TCCR0 = (1<<WGM01);					// tryb CTC
	TCCR0 |= (1<<CS01)|(1<<CS00);         // prescaler 64
	OCR0 = (uint8_t)(F_CPU / 64 / 1000 );           // 1ms  --> poprawka do VIDEO
	TIMSK |= (1<<OCIE0);
#endif

#if TIMER == 2
	TCCR2A |= (1<<WGM21);				// tryb CTC
	TCCR2B |= (1<<CS22);			        // prescaler 64
	OCR2A = (uint8_t)(F_CPU / 64 / 1000 );           // 1ms --> poprawka do VIDEO
	TIMSK2 |= (1<<OCIE2A);
#endif

}




// Przerwanie timera co 1 ms
ISR( ENCODER_vect ) {

//	(*(volatile uint8_t *)((0x05) + 0x20)) ^= (1<<5); //debug LED

	encoder_A = (PHASE_A);
	encoder_B = (PHASE_B);

	if((!encoder_A) && (encoder_A_prev))
	{
		if(encoder_B)
		{
			enc_right_flag = 1;
		}
		else
		{
			enc_left_flag = 1;
		}
	}
	if(!SW_PRESS) enc_sw_flag = 1;

	encoder_A_prev = encoder_A;

}
