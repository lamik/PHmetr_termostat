//-----------------------------------------------------------------------------------------------------------
// *** Obs�uga wy�wietlaczy alfanumerycznych zgodnych z HD44780 ***
//
// - Sterowanie: tryb 4-bitowy
// - Dowolne przypisanie ka�dego sygna�u steruj�cego do dowolnego pinu mikrokontrolera
// - Praca z pinem RW pod��czonym do GND lub do mikrokontrolera (sprawdzanie BusyFLAG - szybkie operacje LCD)
//
// Pliki 			: lcd44780.c , lcd44780.h
// Mikrokontrolery 	: Atmel AVR
// Kompilator 		: avr-gcc
// �r�d�o 			: http://www.atnel.pl
// Data 			: marzec 2010
// Autor 			: Miros�aw Karda�
//----------------------------------------------------------------------------------------------------------


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <util/delay.h>

#include "../LCD_buf/lcd44780.h"

volatile char lcd_buf[LCD_Y][LCD_X];
volatile uint8_t cur_x, cur_y;
volatile uint8_t lcd_event_flag = 1;

// makrodefinicje operacji na sygna�ach steruj�cych RS,RW oraz E

#define SET_RS 	PORT(LCD_RSPORT) |= (1<<LCD_RS)			// stan wysoki na linii RS
#define CLR_RS 	PORT(LCD_RSPORT) &= ~(1<<LCD_RS)		// stan niski na linii RS

#define SET_RW 	PORT(LCD_RWPORT) |= (1<<LCD_RW)			// stan wysoki na RW - odczyt z LCD
#define CLR_RW 	PORT(LCD_RWPORT) &= ~(1<<LCD_RW)		// stan niski na RW - zapis do LCD

#define SET_E 	PORT(LCD_EPORT) |= (1<<LCD_E)			// stan wysoki na linii E
#define CLR_E 	PORT(LCD_EPORT) &= ~(1<<LCD_E)			// stan niski na linii E

#if USE_RW == 1
uint8_t check_BF(void);			// deklaracja funkcji wewn�trznej
#endif

//********************* FUNKCJE WEWN�TRZNE *********************

//----------------------------------------------------------------------------------------
//
//		 Ustawienie wszystkich 4 linii danych jako WYj�cia
//
//----------------------------------------------------------------------------------------
static inline void data_dir_out(void)
{
	DDR(LCD_D7PORT)	|= (1<<LCD_D7);
	DDR(LCD_D6PORT)	|= (1<<LCD_D6);
	DDR(LCD_D5PORT)	|= (1<<LCD_D5);
	DDR(LCD_D4PORT)	|= (1<<LCD_D4);
}

//----------------------------------------------------------------------------------------
//
//		 Ustawienie wszystkich 4 linii danych jako WEj�cia
//
//----------------------------------------------------------------------------------------
#if USE_RW == 1
static inline void data_dir_in(void)
{
	DDR(LCD_D7PORT)	&= ~(1<<LCD_D7);
	DDR(LCD_D6PORT)	&= ~(1<<LCD_D6);
	DDR(LCD_D5PORT)	&= ~(1<<LCD_D5);
	DDR(LCD_D4PORT)	&= ~(1<<LCD_D4);
}
#endif
//----------------------------------------------------------------------------------------
//
//		 Wys�anie po��wki bajtu do LCD (D4..D7)
//
//----------------------------------------------------------------------------------------
static inline void lcd_sendHalf(uint8_t data)
{
	if (data&(1<<0)) PORT(LCD_D4PORT) |= (1<<LCD_D4); else PORT(LCD_D4PORT) &= ~(1<<LCD_D4);
	if (data&(1<<1)) PORT(LCD_D5PORT) |= (1<<LCD_D5); else PORT(LCD_D5PORT) &= ~(1<<LCD_D5);
	if (data&(1<<2)) PORT(LCD_D6PORT) |= (1<<LCD_D6); else PORT(LCD_D6PORT) &= ~(1<<LCD_D6);
	if (data&(1<<3)) PORT(LCD_D7PORT) |= (1<<LCD_D7); else PORT(LCD_D7PORT) &= ~(1<<LCD_D7);
}

#if USE_RW == 1
//----------------------------------------------------------------------------------------
//
//		 Odczyt po��wki bajtu z LCD (D4..D7)
//
//----------------------------------------------------------------------------------------
static inline uint8_t lcd_readHalf(void)
{
	uint8_t result=0;

	if(PIN(LCD_D4PORT)&(1<<LCD_D4)) result |= (1<<0);
	if(PIN(LCD_D5PORT)&(1<<LCD_D5)) result |= (1<<1);
	if(PIN(LCD_D6PORT)&(1<<LCD_D6)) result |= (1<<2);
	if(PIN(LCD_D7PORT)&(1<<LCD_D7)) result |= (1<<3);

	return result;
}
#endif

//----------------------------------------------------------------------------------------
//
//		 Zapis bajtu do wy�wietlacza LCD
//
//----------------------------------------------------------------------------------------
void _lcd_write_byte(unsigned char _data)
{
	// Ustawienie pin�w portu LCD D4..D7 jako wyj�cia
	data_dir_out();

#if USE_RW == 1
	CLR_RW;
#endif

	SET_E;
	lcd_sendHalf(_data >> 4);			// wys�anie starszej cz�ci bajtu danych D7..D4
	CLR_E;

	SET_E;
	lcd_sendHalf(_data);				// wys�anie m�odszej cz�ci bajtu danych D3..D0
	CLR_E;

#if USE_RW == 1
	while( (check_BF() & (1<<7)) );
#else
	_delay_us(120);
#endif

}


//----------------------------------------------------------------------------------------
//
//		 Zapis komendy do wy�wietlacza LCD
//
//----------------------------------------------------------------------------------------
void lcd_write_cmd(uint8_t cmd)
{
	CLR_RS;
	_lcd_write_byte(cmd);
}

//----------------------------------------------------------------------------------------
//
//		 Zapis danych do wy�wietlacza LCD
//
//----------------------------------------------------------------------------------------
void lcd_write_data(uint8_t data)
{
	SET_RS;
	_lcd_write_byte(data);
}




//**************************  FUNKCJE PRZEZNACZONE TAK�E DLA INNYCH MODU��W  ******************

#if USE_LCD_CHAR == 1
//----------------------------------------------------------------------------------------
//
//		 Wys�anie pojedynczego znaku do wy�wietlacza LCD w postaci argumentu
//
//		 8 w�asnych znak�w zdefiniowanych w CGRAM
//		 wysy�amy za pomoc� kod�w 0x80 do 0x87 zamiast 0x00 do 0x07
//
//----------------------------------------------------------------------------------------
void lcd_char(char c)
{
	lcd_buf[cur_y][cur_x] = c;

	if(cur_x < LCD_X-1) cur_x++;
	else
	{
		cur_x = 0;
		if(cur_y < LCD_Y-1) cur_y++;
	}
}
#endif

//----------------------------------------------------------------------------------------
//
//		 Wys�anie stringa do wy�wietlacza LCD z pami�ci RAM
//
//		 8 w�asnych znak�w zdefiniowanych w CGRAM
//		 wysy�amy za pomoc� kod�w 0x80 do 0x87 zamiast 0x00 do 0x07
//
//----------------------------------------------------------------------------------------
void lcd_str(char * str)
{
	register char znak;
	while ( (znak=*(str++)) )
		lcd_char(znak);
}

#if USE_LCD_STR_P == 1
//----------------------------------------------------------------------------------------
//
//		 Wys�anie stringa do wy�wietlacza LCD z pami�ci FLASH
//
//		 8 w�asnych znak�w zdefiniowanych w CGRAM
//		 wysy�amy za pomoc� kod�w 0x80 do 0x87 zamiast 0x00 do 0x07
//
//----------------------------------------------------------------------------------------
void lcd_str_P(char * str)
{
	register char znak;
	while ( (znak=pgm_read_byte(str++)) )
		lcd_char(znak);
}
#endif


#if USE_LCD_STR_E == 1
//----------------------------------------------------------------------------------------
//
//		 Wys�anie stringa do wy�wietlacza LCD z pami�ci EEPROM
//
//		 8 w�asnych znak�w zdefiniowanych w CGRAM
//		 wysy�amy za pomoc� kod�w 0x80 do 0x87 zamiast 0x00 do 0x07
//
//----------------------------------------------------------------------------------------
void lcd_str_E(char * str)
{
	register char znak;
	while(1)
	{
		znak=eeprom_read_byte( (uint8_t *)(str++) );
		if(!znak || znak==0xFF) break;
		else lcd_char(znak);
	}
}
#endif


#if USE_LCD_INT == 1
//----------------------------------------------------------------------------------------
//
//		 Wy�wietla liczb� dziesi�tn� na wy�wietlaczu LCD
//
//----------------------------------------------------------------------------------------
void lcd_int(int val)
{
	char bufor[17];
	lcd_str( itoa(val, bufor, 10) );
}
#endif

#if USE_LCD_HEX == 1
//----------------------------------------------------------------------------------------
//
//		 Wy�wietla liczb� szestnastkow� HEX na wy�wietlaczu LCD
//
//----------------------------------------------------------------------------------------
void lcd_hex(int val)
{
	char bufor[17];
	lcd_str( itoa(val, bufor, 16) );
}
#endif

#if USE_LCD_DEFCHAR == 1
//----------------------------------------------------------------------------------------
//
//		Definicja w�asnego znaku na LCD z pami�ci RAM
//
//		argumenty:
//		nr: 		- kod znaku w pami�ci CGRAM od 0x80 do 0x87
//		*def_znak:	- wska�nik do tablicy 7 bajt�w definiuj�cych znak
//
//----------------------------------------------------------------------------------------
void lcd_defchar(uint8_t nr, uint8_t *def_znak)
{
	register uint8_t i,c;
	lcd_write_cmd( 64+((nr&0x07)*8) );
	for(i=0;i<8;i++)
	{
		c = *(def_znak++);
		lcd_write_data(c);
	}
}
#endif

#if USE_LCD_DEFCHAR_P == 1
//----------------------------------------------------------------------------------------
//
//		Definicja w�asnego znaku na LCD z pami�ci FLASH
//
//		argumenty:
//		nr: 		- kod znaku w pami�ci CGRAM od 0x80 do 0x87
//		*def_znak:	- wska�nik do tablicy 7 bajt�w definiuj�cych znak
//
//----------------------------------------------------------------------------------------
void lcd_defchar_P(uint8_t nr, const uint8_t *def_znak)
{
	register uint8_t i,c;
	lcd_write_cmd( 64+((nr&0x07)*8) );
	for(i=0;i<8;i++)
	{
		c = pgm_read_byte(def_znak++);
		lcd_write_data(c);
	}
}
#endif

#if USE_LCD_DEFCHAR_E == 1
//----------------------------------------------------------------------------------------
//
//		Definicja w�asnego znaku na LCD z pami�ci EEPROM
//
//		argumenty:
//		nr: 		- kod znaku w pami�ci CGRAM od 0x80 do 0x87
//		*def_znak:	- wska�nik do tablicy 7 bajt�w definiuj�cych znak
//
//----------------------------------------------------------------------------------------
void lcd_defchar_E(uint8_t nr, uint8_t *def_znak)
{
	register uint8_t i,c;

	lcd_write_cmd( 64+((nr&0x07)*8) );
	for(i=0;i<8;i++)
	{
		c = eeprom_read_byte(def_znak++);
		lcd_write_data(c);
	}
}
#endif


#if USE_LCD_LOCATE == 1
//----------------------------------------------------------------------------------------
//
//		Ustawienie kursora w pozycji Y-wiersz, X-kolumna
//
// 		Y = od 0 do 3
// 		X = od 0 do n
//
//		funkcja dostosowuje automatycznie adresy DDRAM
//		w zale�no�ci od rodzaju wy�wietlacza (ile posiada wierszy)
//
//----------------------------------------------------------------------------------------
void lcd_locate(uint8_t y, uint8_t x)
{
	cur_x = x;
	cur_y = y;
}
#endif


//----------------------------------------------------------------------------------------
//
//		Kasowanie ekranu wy�wietlacza
//
//----------------------------------------------------------------------------------------
void lcd_cls(void)
{
	int i,j;
	for(i=0; i<LCD_X; i++)
	{
		for(j=0;j<LCD_Y; j++)
		{
			lcd_buf[j][i] = ' ';
		}
	}
	cur_x = cur_y = 0;
}


#if USE_LCD_CURSOR_HOME == 1
//----------------------------------------------------------------------------------------
//
//		Powr�t kursora na pocz�tek
//
//----------------------------------------------------------------------------------------
void lcd_home(void)
{
	cur_x = cur_y = 0;
}
#endif


//----------------------------------------------------------------------------------------
//
//		 ******* INICJALIZACJA WY�WIETLACZA LCD ********
//
//----------------------------------------------------------------------------------------
void lcd_init(void)
{
	// inicjowanie pin�w port�w ustalonych do pod��czenia z wy�wietlaczem LCD
	// ustawienie wszystkich jako wyj�cia
	data_dir_out();
	DDR(LCD_RSPORT) |= (1<<LCD_RS);
	DDR(LCD_EPORT) |= (1<<LCD_E);
	#if USE_RW == 1
		DDR(LCD_RWPORT) |= (1<<LCD_RW);
	#endif

	// wyzerowanie wszystkich linii steruj�cych
	PORT(LCD_RSPORT) &= ~(1<<LCD_RS);
	PORT(LCD_EPORT) &= ~(1<<LCD_E);
	#if USE_RW == 1
		PORT(LCD_RWPORT) &= ~(1<<LCD_RW);
	#endif

	_delay_ms(15);
	PORT(LCD_RSPORT) &= ~(1<<LCD_RS);

	// jeszcze nie mo�na u�ywa� Busy Flag
	lcd_sendHalf(LCDC_FUNC|LCDC_FUNC8B);
	_delay_ms(4.1);
	lcd_sendHalf(LCDC_FUNC|LCDC_FUNC8B);
	_delay_us(100);
	lcd_sendHalf(LCDC_FUNC|LCDC_FUNC4B);
	_delay_us(100);

	// ju� mo�na u�ywa� Busy Flag
	// tryb 4-bitowy, 2 wiersze, znak 5x7
	lcd_write_cmd( LCDC_FUNC|LCDC_FUNC4B|LCDC_FUNC2L|LCDC_FUNC5x7 );
	// wy��czenie kursora
	lcd_write_cmd( LCDC_ONOFF|LCDC_CURSOROFF );
	// w��czenie wy�wietlacza
	lcd_write_cmd( LCDC_ONOFF|LCDC_DISPLAYON );
	// przesuwanie kursora w prawo bez przesuwania zawarto�ci ekranu
	lcd_write_cmd( LCDC_ENTRY|LCDC_ENTRYR );

	// kasowanie ekranu
	lcd_cls();

	/*Timer1 50Hz for lcd*/
		TCCR1B |= (1<<WGM12)|(1<<CS12); // CTC 256-prescaler
		OCR1A = 1249; //for 50Hz
		TIMSK1 |= (1<<OCIE1A); //Timer start
}

void LCD_EVENT()
{
	if(lcd_event_flag)
	{
	uint8_t i,j,y;
		char znak;
	//	lcd_write_cmd( LCDC_CLS|LCDC_HOME );
		for(j=0; j<LCD_Y; j++)
		{
			switch(j)
			{
			case 0: y = LCD_LINE1; break;

			#if (LCD_Y>1)
				    case 1: y = LCD_LINE2; break; // adres 1 znaku 2 wiersza
			#endif
			#if (LCD_Y>2)
			    	case 2: y = LCD_LINE3; break; // adres 1 znaku 3 wiersza
			#endif
			#if (LCD_Y>3)
			    	case 3: y = LCD_LINE4; break; // adres 1 znaku 4 wiersza
			#endif
			}

			lcd_write_cmd(0x80 + y);

			for(i=0; i<LCD_X; i++)
			{
				//send to lcd
				znak = lcd_buf[j][i];
				lcd_write_data(znak);
			}
		}

		lcd_event_flag = 0;
	}
}

ISR(TIMER1_COMPA_vect)
{
//	(*(volatile uint8_t *)((0x05) + 0x20)) ^= (1<<5); //debug LED

	lcd_event_flag = 1;
}
