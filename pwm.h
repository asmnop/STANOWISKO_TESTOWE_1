/*
 * pwm.h
 *
 *  Created on: 2 wrz 2024
 *      Author: mm
 */

#ifndef PWM_H_
#define PWM_H_

#include "universal_lib.h"
#include "..\..\LIB\Inc\gpio.h"

typedef struct
{
	TIM_TypeDef *TIMx;			//	Wybór jednostki używanego timera,
	uint32_t periphs;			//	Nazwa peryferium
	uint32_t clock_source;		//	Wybór źrdła zegara: INTERNAL, EXT_MODE1, EXT_MODE2,
	uint32_t counter_mode;		//	Kierunek zliczania licznika: UP, DOWN, CENTER_UP, CENTER_DOWN, CENTER_UP_DOWN,
	uint32_t frequency;			//	Częstotliwość sygnału PWM w [Hz],
	uint32_t number_of_steps;	//	Liczba kroków wypełnienia sygnału PWM, najlepiej 100,

	uint32_t channel[4];		//	Numer kanału,
	//uint32_t preskaler;
	//uint32_t auto_reload;
}TIM_t_OLD;


typedef struct
{
	TIM_TypeDef *TIMx;		//	Wybór jednostki używanego timera,
	uint32_t clock_source;	//	Wybór źrdła zegara: INTERNAL, EXT_MODE1, EXT_MODE2,
	uint32_t counter_mode;	//	Kierunek zliczania licznika: UP, DOWN, CENTER_UP, CENTER_DOWN, CENTER_UP_DOWN,
	uint32_t NEW_FREQ;		//	Nowa częstotliwość pochodząca z zegara 'SYS_CORE_CLOCK',
	uint32_t frequency;		//	Częstotliwość generowania przerwania UPPDATE w [Hz],
}TIM_t;

typedef struct
{
	LINE_t *line;
	TIM_t *timer;
	uint32_t channel;
	uint32_t work_mode;
	volatile uint32_t duty;
	//uint32_t *duty_cycle;

}CHANNEL_t;



void TIM_basic_init_parameters(TIM_TypeDef *TIMx, uint32_t CounterMode, uint32_t Prescaler, uint32_t AutoReload);

void LED_LINE_disableIT(void);
void LED_LINE_TOGGLE_init(void);
void TIM_enable_IT(void);
void TIM_disable_IT(void);
void TIM_enable_channels(void);
void TIM_disable_channels(void);
void TIM_set_compare(uint16_t *data);
void TIM_OutputCompareMode(TIM_TypeDef *TIMx, uint32_t Channel, uint16_t duty);


void TIM_set_output_compare_mode(TIM_TypeDef *TIMx, uint32_t Channel, uint32_t Mode);
void TIM_set_output_compare_mode_all(uint32_t Mode);

void TIM_force_level(uint8_t *level);
void TIM_basic_timer(void);

void TIM_base_init_for_PWM(TIM_t_OLD *pTIMx);
void TIM_select_mode(TIM_t_OLD *pTIMx, uint32_t channel, uint32_t mode, uint16_t duty);

void TIM_basic_init(TIM_t *TIM);
void TIM_basic_init_parameters(TIM_TypeDef *TIMx, uint32_t counter_mode, uint32_t preskaler, uint32_t frequency);
void TIM_mode_init(CHANNEL_t *pTIM);
void TIM_mode_init_parameters(TIM_TypeDef *TIMx, uint32_t work_mode, uint32_t channel, uint16_t duty);
void TIM_start_counting(TIM_TypeDef *TIMx);

void TIM_set_basic_parameters(TIM_t *pTIMx, const uint32_t preskaler, const uint32_t frequency);

void TIM_one_pulse_mode(void);
void TIM_one_pulse_mode_3(void);

void TIM_enable_clock(TIM_TypeDef *TIMx);
void TIM_freeze_clock(TIM_TypeDef *TIMx);

#endif /* PWM_H_ */









