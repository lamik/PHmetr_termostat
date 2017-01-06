/*
 * mk_encoder.c
 *
 *  na podstawie biblioteki z:
 *  http://www.mikrocontroller.net/articles/Drehgeber
 *  Author: Peter Dannegger
 *
 *  Created on: 15-04-2013
 *      modification: Miros³aw Kardaœ
 */
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "mk_encoder.h"


volatile int8_t ed;

volatile int8_t enc_cnt;
volatile uint8_t enc_flag;


//const int8_t table[16] PROGMEM = {0,1,-1,0,-1,0,0,1,1,0,0,-1,0,-1,1,0};
const int8_t table[16] PROGMEM = {0,1,-1,0,-1,0,0,1,1,0,0,-1,0,-1,1,0};

// obs³uga enkodera
void (*enc_callback)( int8_t dir, int8_t cnt );

void register_enc_callback( void (*callback)( int8_t dir, int8_t cnt ) ) {
	enc_callback = callback;
}

// obs³uga klawisza
void (*sw_callback)( void );

void register_sw_callback( void (*callback)( void ) ) {
	sw_callback = callback;
}

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

	PIN(ENC_PORT)  &= ~(ENC_PH_A|ENC_PH_B);
	PORT(ENC_PORT) |= ENC_PH_A|ENC_PH_B;

	PIN(ENC_SWPORT) &= ~ENC_SW;
	PORT(ENC_SWPORT) |= ENC_SW;


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

void ENCODER_EVENT(  ) {
	uint8_t sreg;
	int8_t ecnt;
	static uint8_t key_lock;

	// obs³uga zdarzeia enkodera
//	if( enc_callback && enc_flag  ) {
		if( enc_flag  ) {
		sreg = SREG;
		cli();
		ecnt = ed>>2; ed &= 3;
		SREG = sreg;				// sei()
		if(ecnt>1) enc_left++;
		if(ecnt<-1) enc_right++;
//
//		if(!res) res=1;
//
//		if(ecnt) enc_callback( ecnt*-1, ecnt*res );

		enc_flag=0;
	}

	// obs³uga zdarzenia przycisku
	if( sw_callback ) {
		if( !key_lock && !(PIN(ENC_SWPORT) & ENC_SW) ) {
			key_lock=1;

			// reakcja na wciniêcie klawisza
			sw_callback();
		}
		else if( key_lock && (PIN(ENC_SWPORT) & ENC_SW ) ) {
			if( !++key_lock ) {
//				 ewentualnie reakcja na zwolnienie klawisza ;)
//				sw_callback();
			}
		}
	}

}



// Przerwanie timera co 1 ms
ISR( ENCODER_vect , ISR_NOBLOCK ) {
	enc_timer++;
    static int8_t last;
    static int8_t last_ed;

    last = (last << 2)  & 0x0F;
    if (PHASE_A) last |=2;
    if (PHASE_B) last |=1;
    ed += pgm_read_byte( &table[last] );

    if( last_ed != ed ) enc_flag=1;

    last_ed=ed;
}
