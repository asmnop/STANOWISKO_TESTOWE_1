/*
 * TM1637.h
 *
 *  Created on: Apr 25, 2024
 *      Author: asmnop
 */


#ifndef INC_TM1637_H_
#define INC_TM1637_H_


#include "universal_lib.h"

#include "..\..\LIB\Inc\gpio.h"


typedef struct
{
	LINE_t *line_CLK;
	LINE_t *line_DIO;
	uint8_t brightness;
}TM1637_t;


//	SETTINGS:
#define NUMBER_OF_DISPLAYS	4		//	Amount of displays to service,

//	DATA COMMAND SETTING:
#define NORMAL_AUTO_WRITE	0x40	//	Normal mode,	automatic address adding,	write data to display register,
#define TEST_AUTO_WRITE		0x48	//	Test mode,		automatic address adding,	write data to display register,
#define NORMAL_FIX_WRITE	0x44	//	Normal mode,	fix address,				write data to display register,
#define TEST_FIX_WRITE		0x4B	//	Test mode,		fix address,				write data to display register,

#define NORMAL_AUTO_READ	0x42	//	Normal mode,	automatic address adding,	read key scan data,
#define TEST_AUTO_READ		0x4A	//	Test mode,		automatic address adding,	read key scan data,
#define NORMAL_FIX_READ		0x46	//	Normal mode,	fix address,				read key scan data,
#define TEST_FIX_READ		0x4D	//	Test mode,		fix address,				read key scan data,


//	DISPLAY CONTROL:
#define	DISP_ON_1		0x88	//	Display on, brightness level: 1,
#define	DISP_ON_2		0x89	//	Display on, brightness level: 2,
#define	DISP_ON_3		0x8A	//	Display on, brightness level: 3,
#define	DISP_ON_4		0x8B	//	Display on, brightness level: 4,
#define	DISP_ON_5		0x8C	//	Display on, brightness level: 5,
#define	DISP_ON_6		0x8D	//	Display on, brightness level: 6,
#define	DISP_ON_7		0x8E	//	Display on, brightness level: 7,
#define	DISP_ON_8		0x8F	//	Display on, brightness level: 8,
#define	DISP_OFF		0x80	//	Display off,
#define	DISP_ON			0x88	//	Different name, Display on, brightness level: 1,


//	ADDRESS SETTING COMMAND:
#define	DISP_ADDR_1		0xC0	//	C0H,	data to first display,
#define	DISP_ADDR_2		0xC1	//	C1H,	data to second display,
#define	DISP_ADDR_3		0xC2	//	C2H,	data to third display,
#define	DISP_ADDR_4		0xC3	//	C3H,	data to forth display,
#define	DISP_ADDR_5		0xC4	//	C4H,	data to fifth display,
#define	DISP_ADDR_6		0xC5	//	C5H,	data to sixth display,
#define	START_ADDR		0xC0	//	C0H,	data to second display, different name,


extern const char* const wave_film[];
extern const char* const snake_film[];
extern const char face_1_arr[];
extern const char face_2_arr[];

extern const char* const circle_full_film[];

//void TM1637_init_2(TM1637_tt *ptr);
void TM1637_init(TM1637_t *ptr);
void TM1637_demo(TM1637_t *pDisplay_1, TM1637_t *pDisplay_2);
void TM1637_cls(TM1637_t *ptr);
void TM1637_text(TM1637_t *ptr, const char sign_arr[], const uint16_t duration);
void TM1637_symbol(TM1637_t *ptr, const char sign_arr[], const uint16_t duration);
void TM1637_film(TM1637_t *ptr, const char* const film_ptr[], const uint16_t duration);
void TM1637_number(TM1637_t *ptr, const uint16_t number, const uint8_t system, const uint16_t duration);
void TM1637_clock(TM1637_t *ptr, const uint8_t hours, const uint8_t minutes, const uint8_t dot);
void TM1637_clock_H_M_dot(TM1637_t *ptr, const uint8_t hours, const uint8_t minutes, const uint8_t dot);
void TM1637_RC5(TM1637_t *ptr, const uint8_t address, const uint8_t command, const uint8_t toggle);
uint8_t TM1637_get_brightness(TM1637_t *ptr);
void TM1637_set_brightness(TM1637_t *ptr, uint8_t up_down);
void TM1637_show_brightness(TM1637_t *ptr);

void TM1637_show_temperature(TM1637_t *pTM1637, int8_t temperature);




#endif /* INC_TM1637_H_ */





