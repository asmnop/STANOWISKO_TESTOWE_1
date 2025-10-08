/*
 * key.h
 *
 *  Created on: 23 maj 2024
 *      Author: asmnop
 */

#ifndef INC_KEY_H_
#define INC_KEY_H_


#include "universal_lib.h"

#include "..\..\LIB\Inc\gpio.h"


/*
typedef struct
{
	GPIO_TypeDef* port;
	uint16_t pin;
	uint8_t number;
}KEY_t;
*/

#define ON_KEY_UP		88	//	Reakcja na zwolnienie przycisku,
#define KEY_TIME_OFF	9999	//	Reakcja na upływ czasu, coś dzieje się jeśli puścimy przed 'time', coś dzieje się po upływie czasu 'time',
#define KEY_PULSER		66	//	Informacja o zajściu obrotu impulsatora,
#define KEY_CLEAR		44	//	Nakazanie wyczyszczenia zebranych danych na temat wciśniętego przycisku, potrzebne do obsługi przycisków w wywoływanej funkcji,






uint8_t key_pressed(LINE_t *pLINE, const uint16_t time, const uint16_t time_rep, void(*foo)(void), void(*foo_rep)(void));


#endif /* INC_KEY_H_ */









