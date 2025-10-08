/*
 * pid.h
 *
 *  Created on: 6 lis 2024
 *      Author: asmnop
 */

#ifndef PID_H_
#define PID_H_


#include "universal_lib.h"


typedef struct
{
	float k_p;				//	Wzmocnienie członu proporcjonalnego,
	float k_i;				//	Wzmocnienie członu całkowego,
	float k_d;				//	Wzmocnienie członu różniczkowego,
	int16_t error_sum;		//	Suma wartości sygnałów błędu regulacji, suma uchybów,
	int16_t error_prev;		//	Wartość błędu regulacji z poprzedniego wyliczenia,
	int16_t anti_windup;
	uint8_t flag;			//	Informacja o zajściu przerwania,
	volatile uint16_t tmp;
}PID_t;


void PID_init(PID_t *PID, const float k_p, const float k_i, const float k_d, const uint16_t anti_windup);
void PID_reset(PID_t *PID);
int16_t PID_compute(PID_t *PID, const int16_t set_point, const int16_t process_variable);

void PID_set_flag(PID_t *PID);
void PID_reset_flag(PID_t *PID);
uint8_t PID_get_flag(PID_t *PID);



#endif /* PID_H_ */







