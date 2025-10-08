/*
 * TM1637_phrases.h
 *
 *  Created on: 7 maj 2024
 *      Author: asmnop
 */

#ifndef INC_TM1637_PHRASES_H_
#define INC_TM1637_PHRASES_H_

#include "ASCII_table.h"

//	UNIVERSAL CHARACT SPRINGS:
#define LED_BLANK	0					//	Empty field on display,
const char LED_END[] = {255};			//	Value accepted as a end data to display,
const char LED_CLEAR[] = {0, 0, 0, 0};	//	Clear LED display,





//	############################################ CIRCLE ############################################
#define CIRCLE_0	SEG_A | SEG_D
#define CIRCLE_1	SEG_B | SEG_E
#define CIRCLE_2	SEG_C | SEG_F

//	$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
const char circle_0_arr[] = { CIRCLE_0, LED_BLANK, LED_BLANK, LED_BLANK };
const char circle_1_arr[] = { CIRCLE_1, LED_BLANK, LED_BLANK, LED_BLANK };
const char circle_2_arr[] = { CIRCLE_2, LED_BLANK, LED_BLANK, LED_BLANK };

const char circle_0000_arr[] = { CIRCLE_0, CIRCLE_0, CIRCLE_0, CIRCLE_0 };
const char circle_1111_arr[] = { CIRCLE_1, CIRCLE_1, CIRCLE_1, CIRCLE_1 };
const char circle_2222_arr[] = { CIRCLE_2, CIRCLE_2, CIRCLE_2, CIRCLE_2 };

const char* const circle_film[] =
{
	circle_0_arr,
	circle_1_arr,
	circle_2_arr,
	LED_END
};

const char* const circle_full_film[] =
{
	circle_0000_arr,
	circle_1111_arr,
	circle_2222_arr,
	LED_END
};


//	############################################## WAVE ############################################
#define COLUMN_0 	SEG_D									//	Short column,
#define COLUMN_1 	SEG_C | SEG_E | SEG_G					//	Medium column,
#define COLUMN_2 	SEG_A | SEG_B | SEG_C | SEG_E | SEG_F	//	High column,

//	$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
const char wave_1_arr[] = { COLUMN_0, COLUMN_0, COLUMN_0, COLUMN_0 };
const char wave_2_arr[] = { COLUMN_1, COLUMN_0, COLUMN_0, COLUMN_0 };
const char wave_3_arr[] = { COLUMN_2, COLUMN_1, COLUMN_0, COLUMN_0 };
const char wave_4_arr[] = { COLUMN_1, COLUMN_2, COLUMN_1, COLUMN_0 };
const char wave_5_arr[] = { COLUMN_0, COLUMN_1, COLUMN_2, COLUMN_1 };
const char wave_6_arr[] = { COLUMN_0, COLUMN_0, COLUMN_1, COLUMN_2 };
const char wave_7_arr[] = { COLUMN_0, COLUMN_0, COLUMN_0, COLUMN_1 };

//	                                         _                       _                      _                      _
//	             	->	 _          	->	| |  _   	->	     _  | |  _    	->	    _  | |  _	->	       _  | |	->	             _	->
//	_   _   _   _		| |  _   _   _		| | | |  _  _		| | | | | |  _		_  | | | | | |		_   _ | | | |		 _   _   _  | |		_   _   _   _

const char* const wave_film[] =
{
	LED_CLEAR,
	wave_1_arr,
	wave_2_arr,
	wave_3_arr,
	wave_4_arr,
	wave_5_arr,
	wave_6_arr,
	wave_7_arr,
	wave_1_arr,
	LED_CLEAR,
	LED_END
};


//	$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
const char level_1_arr[] = { COLUMN_1, COLUMN_1, COLUMN_1, COLUMN_1 };
const char level_2_arr[] = { COLUMN_2, COLUMN_2, COLUMN_2, COLUMN_2 };

//	                                         _   _   _   _
//	             	->	 _   _   _   _  ->	| | | | | | | |	->	 _   _   _   _ 	->
//	_   _   _   _		| | | | | | | |		| | | | | | | |		| | | | | | | |		_   _   _   _

const char* const wave_horizontal_film[] =
{
	LED_CLEAR,
	wave_1_arr,
	level_1_arr,
	level_2_arr,
	level_1_arr,
	wave_1_arr,
	LED_CLEAR,
	LED_END
};


//	################################################ SNAKE #########################################
const char snake_1_arr[] = { SEG_A, LED_BLANK, LED_BLANK, LED_BLANK };
const char snake_2_arr[] = { LED_BLANK, SEG_A, LED_BLANK, LED_BLANK };
const char snake_3_arr[] = { LED_BLANK, LED_BLANK, SEG_A, LED_BLANK };
const char snake_4_arr[] = { LED_BLANK, LED_BLANK, LED_BLANK, SEG_A };
const char snake_5_arr[] = { LED_BLANK, LED_BLANK, LED_BLANK, SEG_B };
const char snake_6_arr[] = { LED_BLANK, LED_BLANK, LED_BLANK, SEG_C };
const char snake_7_arr[] = { LED_BLANK, LED_BLANK, LED_BLANK, SEG_D };
const char snake_8_arr[] = { LED_BLANK, LED_BLANK, SEG_D, LED_BLANK };
const char snake_9_arr[] = { LED_BLANK, SEG_D, LED_BLANK, LED_BLANK };
const char snake_10_arr[] = { SEG_D, LED_BLANK, LED_BLANK, LED_BLANK };
const char snake_11_arr[] = { SEG_E, LED_BLANK, LED_BLANK, LED_BLANK };
const char snake_12_arr[] = { SEG_F, LED_BLANK, LED_BLANK, LED_BLANK };

const char* const snake_film[] =
{
	snake_1_arr,
	snake_2_arr,
	snake_3_arr,
	snake_4_arr,
	snake_5_arr,
	snake_6_arr,
	snake_7_arr,
	snake_8_arr,
	snake_9_arr,
	snake_10_arr,
	snake_11_arr,
	snake_12_arr,
	LED_END
};


//	################################################ FACE ##############################################################
#define EYE_SMALL 	SEG_A | SEG_B | SEG_F | SEG_G			//	Short column,
#define SMILE_LEFT	SEG_D | SEG_E | SEG_F
#define SMILE_RIGHT	SEG_B | SEG_C | SEG_D
#define SMILE_DOWN	SEG_D

const char face_1_arr[] = { EYE_SMALL, LED_BLANK, LED_BLANK, EYE_SMALL };
const char face_2_arr[] = { SMILE_LEFT, SMILE_DOWN, SMILE_DOWN, SMILE_RIGHT };





#endif /* INC_TM1637_PHRASES_H_ */
