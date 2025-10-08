/*
 * encoder.h
 *
 *  Created on: 2 lip 2024
 *      Author: asmnop
 */

#ifndef INC_ENCODER_H_
#define INC_ENCODER_H_


#include "universal_lib.h"

#include "..\..\LIB\Inc\gpio.h"


typedef struct
{
	LINE_t *ENCO_A;		//	Linia z sygnałem z kanału A enkodera,
	LINE_t *ENCO_B;		//	Linia z sygnałem z kanału B enkodera,
	TIM_TypeDef *TIMx;	//	Licznik obsługujący enkoder,
	uint16_t counts;	//	Zliczona ilość zmian stanu w czasie próbkowania,
	uint16_t CPR;		//	Counts Per Revolution, dostępna liczba zmian stanu na obu kanałach podczas jednego obrotu,
	uint8_t flag;
	uint16_t sample_time;	//	Czas próbkowania ilości kroków w jednostce czasu, [ms],
	uint16_t rps;			//	Liczba obrotów na sekundę,
	uint8_t last_direction_step;	//	Kierunek ostatniego obrotu,
	volatile int8_t UPDATE_counter;	//	Licznik zajść przerwania od UPDATE w czasie próbkowania,
	volatile uint16_t get_counts;
}ENCODER_t;


void ENCODER_init(ENCODER_t *pEncoder);
void ENCODER_force_update(ENCODER_t *pEncoder);
void ENCODER_set_IT(ENCODER_t *pEncoder, uint32_t channel, uint32_t edge_polarity);

uint16_t ENCODER_get_counts_in_sample_time(ENCODER_t *pEncoder);
uint8_t ENCO_get_start_position(ENCODER_t *pEncoder);
void ENCO_set_ENCODER_parameters(ENCODER_t *pEncoder, const uint32_t direction, const uint32_t counts);
uint16_t ENCO_get_rps(ENCODER_t *pEncoder);

uint8_t ENCODER_as_key(ENCODER_t *pEncoder, void(*foo)(void), void(*foo_2)(void));

void ENCO_set_steps(ENCODER_t *pEncoder, uint32_t steps);
void ENCO_reset_steps(ENCODER_t *pEncoder);
uint16_t ENCO_get_steps(ENCODER_t *pEncoder);

void ENCO_set_flag(ENCODER_t *pEncoder);
void ENCO_reset_flag(ENCODER_t *pEncoder);
uint8_t ENCO_get_flag(ENCODER_t *pEncoder);

void ENCO_set_UPDATE_counter(ENCODER_t *pEncoder, const int32_t UPDATE_counter);
int8_t ENCO_get_UPDATE_counter(ENCODER_t *pEncoder);
void ENCO_reset_UPDATE_counter(ENCODER_t *pEncoder);

void ENCODER_set_CPR(ENCODER_t *pEncoder, const uint16_t CPR);


#endif /* INC_ENCODER_H_ */











