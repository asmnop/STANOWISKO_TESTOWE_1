/*
 * RC5.h
 *
 *  Created on: 7 lut 2025
 *      Author: asmnop
 */

#ifndef RC5_H_
#define RC5_H_


#include "universal_lib.h"
#include "../../LIB/Inc/terminal.h"


//	ADRESY:
#define TV1				0x00	//	0,
#define TV2				0x01	//	1,
#define TELETEXT		0x02	//	2.
#define VIDEO			0x03	//	3,
#define LV1				0x04	//	4,
#define VCR1			0x05	//	5,
#define VCR2			0x06	//	6,
#define EXPERIMENTAL	0x07	//	7,
#define SAT1			0x08	//	8,
#define CAMERA			0x09	//	9,
#define SAT2			0x0A	//	10,
								//	11,
#define CDV				0x0C	//	12,
#define CAMCORDER		0x0D	//	13,
								//	14,
								//	15,
#define PRE_AMP			0x10	//	16,
#define TUNER			0x11	//	17,
#define RECORDER1		0x12	//	18,
#define PRE_AMP2		0x13	//	19,
#define CD_PLAYER		0x14	//	20,
#define PHONE			0x15	//	21,
#define SATA			0x16	//	22,
#define RECORDER2		0x17	//	23,
								//	24,
								//	25,
#define CDR				0x1A	//	26,
								//	27,
								//	28,
#define LIGHTING		0x1D	//	29,
#define LIGHTING_		0x1E	//	30,
								//	31

#define TUNER			0x11
#define TAPE			0x12
#define CD				0x14
#define AUX				0x15


//	KOMENDY:
#define VOL_UP			16
#define VOL_DOWN		17
#define PROGRAM_UP		32
#define PROGRAM_DOWN	33
#define PAUSE			48
#define FAST_REWIND		50
#define FAST_FORWARD	52
#define PLAY			53
#define STOP			54


//	PILOT NINTENDO
#define NINTENDO		0x00

#define NIN_UP			0x01
#define NIN_RIGHT		0x02
#define NIN_DOWN		0x03
#define NIN_LEFT		0x04
#define NIN_SELECT		0x05
#define NIN_START		0x06
#define NIN_B			0x07
#define NIN_A			0x08




typedef struct
{
	uint8_t toggle;		//	Ostatnio odebrany bit 'toggle',
	uint8_t address;	//	Ostatnio odebrany adres,
	uint8_t command;	//	Ostatnio odebrana komenda,
	uint8_t flag;		//	Flaga informująca o odebranej prawidłowo nowej ramce danych,
	uint16_t RC5_data_frame[10];	//	Bufor na odebrane ramki danych,
	uint8_t buf;
}RC5_t;



void RC5_init(void);
void RC5_init_2(void);
void RC5_init_3(void);

void RC5_new_data(RC5_t *pRC5, const uint16_t data_frame);
uint16_t RC5_get_data(const RC5_t *pRC5);

void RC5_set_flag_new_data(RC5_t *pRC5);
void RC5_reset_flag_new_data(RC5_t *pRC5);
uint8_t RC5_get_flag_new_data(const RC5_t *pRC5);

void RC5_control_panel(RC5_t *pRC5, void (*foo_1)(void), void (*foo_2)(void), void (*foo_3)(void), void (*foo_4)(void));

void RC5_terminal_info(TERMINAL_t *pTERMINAL, RC5_t *pRC5, const uint8_t line, const uint8_t row_1, const uint8_t row_2);
void RC5_terminal_info_2(TERMINAL_t *pTERMINAL, RC5_t *pRC5, const uint8_t line, const uint8_t row_1, const uint8_t row_2);

#endif /* RC5_H_ */







