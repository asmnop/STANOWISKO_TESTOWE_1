/*
 * SM_driver.c
 *
 *  Created on: 18 gru 2024
 *      Author: asmnop
 */


#include "../../LIB/Inc/SM_driver.h"
#include "../../LIB/Inc/pwm.h"
#include "../../LIB/Inc/universal_functions.h"
#include "../../LIB/Inc/notes.h"
#include "../../LIB/Inc/buzzer.h"

/*
 * PRZEMYŚLENIA:
 * -
 */

/*
 * STEROWANIE:
 * -wykonanie kroku - zbocze narastające na PUL, 0-->1
 *
 */

/*
 * MODUŁ LINIOWY:
 * -po włączeniu zasilania:
 * -silnik krokowy stoi w miejscu,
 * -współrzędna po osi X jest określona przez wartość zero,
 * -oś X jest zorientowana od krańcówki X1 do X2,
 * -położenie może przyjmować wartości dodatnie i ujemne,
 * -zanim wykonamy bazowanie to współrzędne określające bieżące położenie to współrzędne lokalne,
 * -sterowanie ręczne odbywa się za pomocą joysticka lub z terminala,
 * -po wykryciu krańcówki w trybie ręcznym zostaje zapamiętana pozycja współrzednej lokalnej
 * oraz ustawiana jest flaga wykrycia krańcówki,
 * -flaga wykrycia krańcówki zapobiega możliwości dalszego wykonania ruchu w zadanym kierunku,
 *
 *
 */


void SM_init_first(SM_t *pSM)
{
	//	-procedura inicjalizacyjna SM,

	GPIO_set_pin_as_output(pSM->dir_line); 			//	Ustawienie linii z kierunkiem obrotu,
	GPIO_set_pin_as_output(pSM->step_line->line);	//	Ustawienia linii z sygnałem PWM,

	if(pSM->microstepping_1 != NULL)
	{
		GPIO_set_pin_as_output(pSM->microstepping_1);
	}
	if(pSM->microstepping_2 != NULL)
	{
		GPIO_set_pin_as_output(pSM->microstepping_2);
	}
	if(pSM->microstepping_3 != NULL)
	{
		GPIO_set_pin_as_output(pSM->microstepping_3);
	}
	if(pSM->enable_line != NULL)
	{
		GPIO_set_pin_as_output(pSM->enable_line);
		pSM->work_mode = SM_STANDBY;
	}

	uint32_t auto_reload = __LL_TIM_CALC_ARR(SYS_CORE_CLOCK, 10 - 1, 100) + 1;

	////////////////////////////////////////////////////////////////////////////
	LL_APB1_GRP1_EnableClock   (LL_APB1_GRP1_PERIPH_TIM2);
	LL_TIM_EnableARRPreload    (TIM2);
	LL_TIM_SetClockSource      (TIM2, LL_TIM_CLOCKSOURCE_INTERNAL);
	LL_TIM_SetCounterMode      (TIM2, LL_TIM_COUNTERMODE_UP);
	LL_TIM_SetPrescaler        (TIM2, 10 - 1);
	LL_TIM_SetAutoReload       (TIM2, auto_reload - 1);
	LL_TIM_OC_SetMode          (TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_PWM2);
	LL_TIM_OC_SetPolarity      (TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_OCPOLARITY_HIGH);
	LL_TIM_OC_EnablePreload    (TIM2, LL_TIM_CHANNEL_CH1);
	LL_TIM_OC_SetCompareCH1    (TIM2, 0);
	LL_TIM_CC_EnableChannel    (TIM2, LL_TIM_CHANNEL_CH1);
	LL_TIM_EnableIT_CC1        (TIM2);
	LL_TIM_GenerateEvent_UPDATE(TIM2);
	LL_TIM_ClearFlag_UPDATE    (TIM2);
	//LL_TIM_EnableCounter       (TIM2);
}

void SM_init(SM_t *pSM)
{
	//	-procedura inicjalizacyjna SM,

	GPIO_set_pin_as_output(pSM->dir_line); 			//	Ustawienie linii z kierunkiem obrotu,
	GPIO_set_pin_as_output(pSM->step_line->line);	//	Ustawienia linii z sygnałem PWM,

	if(pSM->microstepping_1 != NULL)
	{
		GPIO_set_pin_as_output(pSM->microstepping_1);
	}
	if(pSM->microstepping_2 != NULL)
	{
		GPIO_set_pin_as_output(pSM->microstepping_2);
	}
	if(pSM->microstepping_3 != NULL)
	{
		GPIO_set_pin_as_output(pSM->microstepping_3);
	}
	if(pSM->enable_line != NULL)
	{
		GPIO_set_pin_as_output(pSM->enable_line);
		pSM->work_mode = SM_STANDBY;
	}

	////////////////////////////////////////////////////////////////////////////
	TIM_enable_clock(pSM->step_line->timer->TIMx);

	uint32_t prescal = __LL_TIM_CALC_PSC(SYS_CORE_CLOCK, pSM->step_line->timer->NEW_FREQ);

	//	Wartość preskalera, preskaler 16-bitowy!!! wartości od 1..65535:
	//LL_TIM_SetPrescaler(pSM->step_line->timer->TIMx, prescal);		//	TIM_CR1_DIR | TIM_CR1_CMS

	uint32_t auto_reload = __LL_TIM_CALC_ARR(SYS_CORE_CLOCK, prescal, 100) + 1;

	LL_TIM_EnableARRPreload    (pSM->step_line->timer->TIMx);
	LL_TIM_SetClockSource      (pSM->step_line->timer->TIMx, pSM->step_line->timer->clock_source);
	LL_TIM_SetCounterMode      (pSM->step_line->timer->TIMx, pSM->step_line->timer->counter_mode);
	LL_TIM_SetPrescaler        (pSM->step_line->timer->TIMx, prescal);
	LL_TIM_SetAutoReload       (pSM->step_line->timer->TIMx, auto_reload - 1);
	LL_TIM_OC_SetMode          (pSM->step_line->timer->TIMx, pSM->step_line->channel, pSM->step_line->work_mode);
	LL_TIM_OC_SetPolarity      (pSM->step_line->timer->TIMx, pSM->step_line->channel, LL_TIM_OCPOLARITY_HIGH);
	LL_TIM_OC_EnablePreload    (pSM->step_line->timer->TIMx, pSM->step_line->channel);
	LL_TIM_OC_SetCompareCH1    (pSM->step_line->timer->TIMx, 0);
	LL_TIM_CC_EnableChannel    (pSM->step_line->timer->TIMx, pSM->step_line->channel);
	LL_TIM_EnableIT_CC1        (pSM->step_line->timer->TIMx);
	LL_TIM_GenerateEvent_UPDATE(pSM->step_line->timer->TIMx);
	LL_TIM_ClearFlag_UPDATE    (pSM->step_line->timer->TIMx);
	//LL_TIM_EnableCounter       (pSM->step_line->timer->TIMx);
}


void SM_reset_counters(SM_t *pSM)
{
	//	-zresetowanie wszystkich liczników globalnych dotyczących wykonanych obrotów przez silnik krokowy,

	if((pSM->work_mode == SM_STOP) || (pSM->work_mode == SM_STANDBY))
	{
		pSM->global_step_counter = 0;
		pSM->CW_step_counter = 0;
		pSM->CCW_step_counter = 0;
	}
}

void SM_set_standby(SM_t *pSM)
{
	//	-wyłączenie zasilania do silnika SM, wejście do trybu wstrzymania,

	if(pSM->enable_line != NULL)
	{
		GPIO_reset_pin(pSM->enable_line);
	}

	LL_TIM_DisableCounter(pSM->step_line->timer->TIMx);
	LL_TIM_SetCounter(pSM->step_line->timer->TIMx, 0);
	SM_set_freq(pSM, 0);
	LL_TIM_GenerateEvent_UPDATE(pSM->step_line->timer->TIMx);
	LL_TIM_ClearFlag_UPDATE(pSM->step_line->timer->TIMx);

	pSM->steps_counter = 0;
	pSM->steps_to_make = 0;
	pSM->interval = 0;
	pSM->interval_counter= 0;
	pSM->freq_base = 0;
	pSM->time_base = 0;
	SM_reset_counters(pSM);

	pSM->work_mode = SM_STANDBY;

}

void SM_stop(SM_t *pSM)
{
	//	-zatrzymanie obrotów silnika krokowego,
	if(pSM->enable_line != NULL)
	{
		GPIO_set_pin(pSM->enable_line);
	}

	LL_TIM_DisableCounter(pSM->step_line->timer->TIMx);
	LL_TIM_SetCounter(pSM->step_line->timer->TIMx, 0);
	SM_set_freq(pSM, 0);
	LL_TIM_GenerateEvent_UPDATE(pSM->step_line->timer->TIMx);
	LL_TIM_ClearFlag_UPDATE(pSM->step_line->timer->TIMx);

	pSM->steps_counter = 0;
	pSM->steps_to_make = 0;
	pSM->interval = 0;
	pSM->interval_counter= 0;
	pSM->freq_base = 0;
	pSM->time_base = 0;

	pSM->work_mode = SM_STOP;
}

void SM_start(SM_t *pSM)
{
	//	Setting rotation parameters:
	SM_set_direction(pSM, pSM->param_table[0][0]);
	SM_set_freq(pSM, pSM->param_table[0][1]);
	pSM->steps_to_make = pSM->param_table[0][2];

	//	Make a work mode:
	if(pSM->steps_to_make != 0)
	{
		if(pSM->interval == 1)
		{
			pSM->work_mode = SM_STEP;
		}
		else
		{
			pSM->work_mode = SM_ACCEL;
		}
	}
	else if(pSM->steps_to_make == 0)
	{
		pSM->work_mode = SM_CONTINUOUS;
		pSM->interval = 0;
	}

	//	Update registers and start counting:
	LL_TIM_GenerateEvent_UPDATE(pSM->step_line->timer->TIMx);
	LL_TIM_ClearFlag_UPDATE(pSM->step_line->timer->TIMx);
	LL_TIM_EnableCounter(pSM->step_line->timer->TIMx);
}

void SM_set_steps(SM_t *pSM, const uint8_t dir, const uint16_t freq, const uint16_t steps)
{
	if(pSM->work_mode == SM_STOP)
	{
		if( ((pSM->flag_limit_switch_1 == 1) && (pSM->local_position == pSM->limit_switch_1 + pSM->homing_value) && (dir == SM_CCW) )
		||  ((pSM->flag_limit_switch_2 == 1) && (pSM->local_position == pSM->limit_switch_2 - pSM->homing_value) && (dir == SM_CW) ) )
		{
			return;
		}
		if((pSM->flag_limit_switch_1 == 1) && (GPIO_get_input_pin(&LIMIT_SWITCH_X1_LINE) == 1))
		{
			return;
		}
		if((pSM->flag_limit_switch_2 == 1) && (GPIO_get_input_pin(&LIMIT_SWITCH_X2_LINE) == 1))
		{
			return;
		}
		else
		{
			pSM->param_table[0][0] = dir;
			pSM->param_table[0][1] = freq;
			pSM->param_table[0][2] = steps;
			pSM->interval = 1;

			SM_start(pSM);
		}
	}
	else if(pSM->work_mode == SM_STANDBY)
	{
		return;
	}
	else
	{
		SM_stop(pSM);
	}
}

void SM_set_position(SM_t *pSM, const uint16_t position, const uint16_t freq)
{
	//	-skierowanie na pozycję domową wózka,
	//	-funkcja odpali się jedynie w przypadku jeśli ustawione już mamy flagi krańcówek oraz
	//	jeżeli zadana pozycja mieści się w dostępnym zakresie,


	if((pSM->flag_limit_switch_1 == 1) && (pSM->flag_limit_switch_2 == 1))
	{
		if((pSM->global_position != position) && (freq != 0))
		{
			if(position < pSM->global_position)
			{
				SM_set_steps(pSM, SM_CCW, freq, pSM->global_position - position);
			}
			else if(position > pSM->global_position)
			{
				SM_set_steps(pSM, SM_CW, freq, position - pSM->global_position);
			}
		}
	}
}

void SM_set_rotations(SM_t *pSM, const uint8_t direction, const uint16_t time_s, const uint16_t rotations)
{
	//	-wykonanie zadanej ilości obrotów w wybranym kierunku z zadanym czasem całkowitym ich wykonania,

	uint16_t steps = rotations * pSM->steps_per_revolution;
	uint16_t freq_steps = steps / time_s;

	SM_set_steps(pSM, direction, freq_steps, steps);
}

void SM_set_continuous(SM_t *pSM, const uint8_t direction, const uint16_t freq)
{
	//	-tryb pracy ciągłej z zadanym kierunkiem i częstotliwością,

	SM_set_steps(pSM, direction, freq, 0);
}

void SM_set_continuous_accel(SM_t *pSM, const uint8_t direction, const uint16_t time_s, const uint16_t freq, const uint8_t interval)
{
	//	-generowanie tablicy na parametry określające pracę silnika krokowego,
	//	-te trzy parametry to: kierunek obrotów, częstotliwość oraz ilość kroków do wykonania,
	//	-'interval' - ilość poziomów częstotliwości do zajścia wymaganej,

	//	PRZYKŁAD:
	//	time_s = 2400
	//	freq = 960
	//	interval = 24

	#define CONSTANT_ROTATION	1

	if(pSM->work_mode == SM_STOP)
	{
		if( ((pSM->flag_limit_switch_1 == 1) && (pSM->local_position == pSM->limit_switch_1 + pSM->homing_value) && (SM_get_direction(pSM) == SM_CCW) )
		||  ((pSM->flag_limit_switch_2 == 1) && (pSM->local_position == pSM->limit_switch_2 - pSM->homing_value) && (SM_get_direction(pSM) == SM_CW) ) )
		{
			return;
		}
		if((pSM->flag_limit_switch_1 == 1) && (GPIO_get_input_pin(&LIMIT_SWITCH_X1_LINE) == 1))
		{
			return;
		}
		if((pSM->flag_limit_switch_2 == 1) && (GPIO_get_input_pin(&LIMIT_SWITCH_X2_LINE) == 1))
		{
			return;
		}

		pSM->interval = interval;
		pSM->time_base = time_s / pSM->interval;		//	Czas trwania jednego interwału, 100 [ms]
		pSM->freq_base = (freq / pSM->interval)>>1;		//	Częstotliwość z jaką generowane są impulsy dla pierwszego interwału, 20
		pSM->steps_to_make = (pSM->time_base * pSM->freq_base) / 1000;	//	Ilość kroków do wykonania w pierwszym interwale,

		for(uint8_t i = 1; i<interval + 1; i++)
		{
			//	ROZPĘDZANIE:
			pSM->param_table[i-1][0] = direction;
			pSM->param_table[i-1][1] = pSM->freq_base * ((i<<1) - 1);
			pSM->param_table[i-1][2] = (pSM->time_base  * pSM->freq_base * ((i<<1) - 1)) / 1000;

			//	ZWALNIANIE:
			pSM->param_table[(interval<<1) - i + 1][0] = direction;
			pSM->param_table[(interval<<1) - i + 1][1] = pSM->freq_base * ((i<<1) - 1);
			pSM->param_table[(interval<<1) - i + 1][2] = (pSM->time_base * pSM->freq_base * ((i<<1) - 1)) / 1000;
		}

		//	USTAWIENIE PARAMETRÓW DLA PRACY CIĄGŁEJ:
		pSM->param_table[interval][0] = direction;
		pSM->param_table[interval][1] = freq;
		pSM->param_table[interval][2] = pSM->steps_per_revolution * CONSTANT_ROTATION;	//	Ilość obrotów ze stałą prędkością,
		pSM->interval = (interval<<1) + 1;	//	Całkowita ilość interwałów,

		SM_start(pSM);
	}
	else if(pSM->work_mode == SM_STANDBY)
	{
		return;
	}
	else
	{
		SM_stop(pSM);
	}

#undef CONSTANT_ROTATION
}

void SM_inc_dec_freq(SM_t *pSM, const int16_t value)
{
	//	-zmiana częstotliwości o zadaną wartość,

	if((pSM->work_mode == SM_STOP) || (pSM->work_mode == SM_CONTINUOUS))
	{
		if( ((pSM->flag_limit_switch_1 == 1) && (pSM->local_position == pSM->limit_switch_1 + pSM->homing_value) && (SM_get_direction(pSM) == SM_CCW) )
		||  ((pSM->flag_limit_switch_2 == 1) && (pSM->local_position == pSM->limit_switch_2 - pSM->homing_value) && (SM_get_direction(pSM) == SM_CW) ) )
		{
			return;
		}
		if((pSM->flag_limit_switch_1 == 1) && (GPIO_get_input_pin(&LIMIT_SWITCH_X1_LINE) == 1))
		{
			return;
		}
		if((pSM->flag_limit_switch_2 == 1) && (GPIO_get_input_pin(&LIMIT_SWITCH_X2_LINE) == 1))
		{
			return;
		}
		if((value < 0) && ((pSM->step_line->timer->frequency + value) > pSM->max_freq))
		{
			pSM->step_line->timer->frequency = 0;
		}
		else if((value > 0) && (pSM->step_line->timer->frequency > pSM->max_freq))
		{
			pSM->step_line->timer->frequency = pSM->max_freq;
		}
		else
		{
			pSM->step_line->timer->frequency = pSM->step_line->timer->frequency + value;
		}

		SM_set_freq(pSM, pSM->step_line->timer->frequency);
		pSM->steps_to_make = 0;
		pSM->interval = 0;

		if(pSM->step_line->timer->frequency == 0)
		{
			pSM->work_mode = SM_STOP;
		}
		else
		{
			if(pSM->work_mode == SM_STOP)
			{
				SM_set_continuous(pSM, SM_get_direction(pSM), pSM->step_line->timer->frequency);
			}
		}
	}
}

void SM_song(SM_t *pSM, SONG_t *pSONG)
{
	//	-zagranie utworu muzycznego na module liniowym,
	//	-parametry utworu są generowane ze struktury 'SONG_t',
	//	-są one umieszczane w tablicy w kolejności: kierunek obrotów silnika,
	//	częstotliwość, ilość kroków do wykonania,
	//	-tablica nie zostanie wygenerowana w przypadku ograniczenia silnika co do
	//	maksymalnej generowanej częstotliwości,
	//	-kierunek obrotów jest dobierany tak aby można było wykonać pełną sekwencję
	//	kroków z zadaną częstotliwością,
	//	-odtworzenie melodi możliwe jednynie w przypadku wykonania pełnego bazowania,

	if(pSM->work_mode == SM_STOP)
	{
		if(!(pSM->flag_limit_switch_1 == 1 && pSM->flag_limit_switch_2 == 1))
		{
			return;
		}

		if( ((pSM->flag_limit_switch_1 == 1) && (pSM->local_position == pSM->limit_switch_1 + pSM->homing_value) && (SM_get_direction(pSM) == SM_CCW) )
		||  ((pSM->flag_limit_switch_2 == 1) && (pSM->local_position == pSM->limit_switch_2 - pSM->homing_value) && (SM_get_direction(pSM) == SM_CW) ) )
		{
			return;
		}
		if((pSM->flag_limit_switch_1 == 1) && (GPIO_get_input_pin(&LIMIT_SWITCH_X1_LINE) == 1))
		{
			return;
		}
		if((pSM->flag_limit_switch_2 == 1) && (GPIO_get_input_pin(&LIMIT_SWITCH_X2_LINE) == 1))
		{
			return;
		}

		uint16_t number = number_of_notes(pSONG);

		//	Sprawdzenie warunku dostępnej częstotliwości:
		for(uint8_t i=0; i<number; i = i + 2)
		{
			if(pSONG->notes[i] > pSM->max_freq)
			{
				return;
			}
		}

		uint16_t i = 0;
		do
		{
			if(i % 4 == 2)
			{
				pSM->param_table[i>>1][0] = SM_CW;
			}
			else
			{
				pSM->param_table[i>>1][0] = SM_CCW;
			}
			//	Jeżeli do zagrania jest nuta o określonej częstotliwości:
			if(pSONG->notes[i] != 1)
			{
				pSM->param_table[i>>1][1] = pSONG->notes[i];	//	f
				pSM->param_table[i>>1][2] = (((pSONG->notes[i]<<2) * 60) / pSONG->notes[i + 1] ) / pSONG->tempo;
			}
			//	Jeżeli do zagrania jest PAUZA:
			else if(pSONG->notes[i] == 1)
			{
				pSM->param_table[i>>1][1] = 20000;
				pSM->param_table[i>>1][2] = (((20000<<2) * 60) / pSONG->notes[i + 1] ) / pSONG->tempo;
			}
			while(pSM->steps_to_make != 0);

			i = i + 2;
		}while(pSONG->notes[i] != 0);	//	Generowania dopuki nie napotka się końca utworu:

		pSM->interval = i>>1;			//	Ilość iteracji dwukrotnie mniejsza od licznika,

		SM_start(pSM);

	}
	else if(pSM->work_mode == SM_STANDBY)
	{
		return;
	}
	else
	{
		SM_stop(pSM);
	}
}


void SM_homing(SM_t *pSM, const uint8_t Xx)
{
	//	-wykonanie bazowania wózka w zależności od wymaganej pozycji końcowej,

	#define FREQ	220

	if((pSM->flag_limit_switch_1 == 0) && (pSM->flag_limit_switch_2 == 0) && (Xx == LIMIT_X1))
	{
		SM_set_continuous(pSM, SM_CW, FREQ);
		while(pSM->flag_limit_switch_2 == 0);
		while(pSM->steps_to_make != 0);
		LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_3);
		NVIC_EnableIRQ(EXTI3_IRQn);

		SM_set_continuous(pSM, SM_CCW, FREQ);
		while(pSM->flag_limit_switch_1 == 0);
		while(pSM->steps_to_make != 0);
		LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_2);
		NVIC_EnableIRQ(EXTI2_IRQn);
	}
	else if((pSM->flag_limit_switch_1 == 0) && (pSM->flag_limit_switch_2 == 0) && (Xx == LIMIT_X2))
	{
		SM_set_continuous(pSM, SM_CCW, FREQ);
		while(pSM->flag_limit_switch_1 == 0);
		while(pSM->steps_to_make != 0);
		LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_2);
		NVIC_EnableIRQ(EXTI2_IRQn);

		SM_set_continuous(pSM, SM_CW, FREQ);
		while(pSM->flag_limit_switch_2 == 0);
		while(pSM->steps_to_make != 0);
		LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_3);
		NVIC_EnableIRQ(EXTI3_IRQn);
	}
	//
	else if((pSM->flag_limit_switch_1 == 1) && (pSM->flag_limit_switch_2 == 0) && (Xx == LIMIT_X1))
	{
		SM_set_continuous(pSM, SM_CW, FREQ);
		while(pSM->flag_limit_switch_2 == 0);
		while(pSM->steps_to_make != 0);
		LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_3);
		NVIC_EnableIRQ(EXTI3_IRQn);

		SM_set_position(pSM, pSM->homing_value, FREQ);
	}
	else if((pSM->flag_limit_switch_1 == 1) && (pSM->flag_limit_switch_2 == 0) && (Xx == LIMIT_X2))
	{
		SM_set_continuous(pSM, SM_CW, FREQ);
		while(pSM->flag_limit_switch_2 == 0);
		while(pSM->steps_to_make != 0);
		LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_3);
		NVIC_EnableIRQ(EXTI3_IRQn);

		SM_set_position(pSM, SM_get_step_range(pSM) - pSM->homing_value, FREQ);
	}
	//
	else if((pSM->flag_limit_switch_1 == 0) && (pSM->flag_limit_switch_2 == 1) && (Xx == LIMIT_X1))
	{
		SM_set_continuous(pSM, SM_CCW, FREQ);
		while(pSM->flag_limit_switch_1 == 0);
		while(pSM->steps_to_make != 0);
		LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_2);
		NVIC_EnableIRQ(EXTI2_IRQn);

		SM_set_position(pSM, pSM->homing_value, FREQ);
	}
	else if((pSM->flag_limit_switch_1 == 0) && (pSM->flag_limit_switch_2 == 1) && (Xx == LIMIT_X2))
	{
		SM_set_continuous(pSM, SM_CCW, FREQ);
		while(pSM->flag_limit_switch_1 == 0);
		while(pSM->steps_to_make != 0);
		LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_2);
		NVIC_EnableIRQ(EXTI2_IRQn);

		SM_set_position(pSM, SM_get_step_range(pSM) - pSM->homing_value, FREQ);
	}
	//
	else if((pSM->flag_limit_switch_1 == 1) && (pSM->flag_limit_switch_2 == 1) && (Xx == LIMIT_X1))
	{
		SM_set_position(pSM, pSM->homing_value, FREQ);
	}
	else if((pSM->flag_limit_switch_1 == 1) && (pSM->flag_limit_switch_2 == 1) && (Xx == LIMIT_X2))
	{
		SM_set_position(pSM, SM_get_step_range(pSM) - pSM->homing_value, FREQ);
	}

	#undef FREQ
}

uint16_t SM_get_step_range(SM_t *pSM)
{
	//	-wyliczenie zakresu dostępnych pozycji wózka

	return pSM->limit_switch_2 - pSM->limit_switch_1;
}

uint8_t SM_get_direction(SM_t *pSM)
{
	//	-pobranie wartości ustawionej na linii sterującej od kierunku obrotów,

	return GPIO_get_output_pin(pSM->dir_line);
}

uint8_t SM_set_direction(SM_t *pSM, const uint8_t direction)
{
	//	-ustawienie kierunku obrotów silnika krokowego,
	//	-jeśli zadany kierunek nie jest zmieniany to nic nie jest wykonywane,
	//	-inne zachowanie jeśli zmiana następuje podczas trybu STOP, a inne
	//	dla każdegp innego trybu,

	//	Jeżeli kierunki obrotów są zgodne:
	if(pSM->work_mode == SM_STOP)
	{
		if(direction == SM_CW)
		{
			GPIO_set_pin(pSM->dir_line);
			return SM_CW;
		}
		//	Jeżeli kierunki obrotów są przeciwne:
		else if(direction == SM_CCW)
		{
			GPIO_reset_pin(pSM->dir_line);
			return SM_CCW;
		}
	}

	return 2;
}

void SM_freq_correction(SM_t *pSM, uint16_t freq)
{
	//	-sprawdzenie czy nowa częstotliwość nie przekracza wartości maksymalnej,

	if(freq > pSM->max_freq)
	{
		freq = pSM->max_freq;
	}

	pSM->step_line->timer->frequency = freq;
}

void SM_set_freq(SM_t *pSM, uint16_t freq)
{
	//	-ustawienie częstotliwości sygnału PWM, który steruje prędkością silnika,

	/*
	//	Jeżeli częstotliwości są takie same:
	if(freq == SM->step_line->timer->frequency)
	{
		return 0;
	}
	//	Jeżeli częstotliwość zadana jest większa od maksymalnej możliwej:
	else if(freq > SM->max_freq)
	{
		freq = SM->max_freq;

		if(freq == SM->step_line->timer->frequency)
		{
			return 0;
		}
	}
	//	Jeżeli częstotliwość jest równa zero to jest to równoważne z
	//	zatrzymaniem silnika,
	else if(freq == 0)
	{
		SM_stop(SM);
		return 0;
	}
*/
	//	Zmiana parametru częstotliwości:
	//SM->step_line->timer->frequency = freq;


	SM_freq_correction(pSM, freq);

	//uint32_t auto_reload = __LL_TIM_CALC_ARR(SYS_CORE_CLOCK, pSM->step_line->timer->preskaler - 1, pSM->step_line->timer->frequency) + 1;
	uint32_t auto_reload = __LL_TIM_CALC_ARR(SYS_CORE_CLOCK, LL_TIM_GetPrescaler(pSM->step_line->timer->TIMx), pSM->step_line->timer->frequency) + 1;
	LL_TIM_SetAutoReload(pSM->step_line->timer->TIMx, auto_reload - 1);
	LL_TIM_OC_SetCompareCH1(pSM->step_line->timer->TIMx, (auto_reload>>1) - 1);
}

void SM_set_microstepping(SM_t *pSM, uint8_t microstepping)
{
	//	-ustawienie wartości mikrokroków,

	if(microstepping == FULL_STEP)
	{
		GPIO_reset_pin(pSM->microstepping_1);
		GPIO_reset_pin(pSM->microstepping_2);
		GPIO_reset_pin(pSM->microstepping_3);
		pSM->microstep_mode = FULL_STEP;
	}
	else if(microstepping == HALF_STEP)
	{
		GPIO_set_pin(pSM->microstepping_1);
		GPIO_reset_pin(pSM->microstepping_2);
		GPIO_reset_pin(pSM->microstepping_3);
		pSM->microstep_mode = HALF_STEP;
	}
	else if(microstepping == QUARTER_STEP)
	{
		GPIO_reset_pin(pSM->microstepping_1);
		GPIO_set_pin(pSM->microstepping_2);
		GPIO_reset_pin(pSM->microstepping_3);
		pSM->microstep_mode = QUARTER_STEP;
	}
	else if(microstepping == EIGHT_STEP)
	{
		GPIO_set_pin(pSM->microstepping_1);
		GPIO_set_pin(pSM->microstepping_2);
		GPIO_reset_pin(pSM->microstepping_3);
		pSM->microstep_mode = EIGHT_STEP;
	}
	else if(microstepping == SIXTEENTH_STEP)
	{
		GPIO_set_pin(pSM->microstepping_1);
		GPIO_set_pin(pSM->microstepping_2);
		GPIO_set_pin(pSM->microstepping_3);
		pSM->microstep_mode = SIXTEENTH_STEP;
	}
}

void SM_change_step_value(SM_t *pSM)
{
	//	-zmiana wartości wielkości zadawanego kroku,

	static uint8_t number = 3;
	const uint8_t arr_step_value[8] = {1, 2, 5, 10, 20, 50, 100, 200};

	pSM->step_value = arr_step_value[number];
	number = inc(number, 7);
}

uint8_t SM_get_step_value(SM_t *pSM)
{
	return pSM->step_value;
}










uint16_t compute_note_duration_(uint16_t note_duration)
{
#define NOTE_MIN 2048

	//	Obliczenie ile najmniejszych stopni dźwięku trwa dana nuta:
	if(note_duration == 1)			note_duration = NOTE_MIN;			//	128,	nuta,
	else if(note_duration == 2)		note_duration = NOTE_MIN>>1;		//	64,		półnuta,
	else if(note_duration == 3)		note_duration = (NOTE_MIN>>2) * 3;	//	96,		półnuta + kropka,
	else if(note_duration == 4)		note_duration = NOTE_MIN>>2;		//	32,		ćwierćnuta,
	else if(note_duration == 6)		note_duration = (NOTE_MIN>>3) * 3;	//	48,		ćwierćnuta + kropka,
	else if(note_duration == 8)		note_duration = NOTE_MIN>>3;		//	16,		ósemka,
	else if(note_duration == 12)	note_duration = (NOTE_MIN>>4) * 3;	//	24,		ósemka + kropka,
	else if(note_duration == 16)	note_duration = NOTE_MIN>>4;		//	8,		szesnastka,
	else if(note_duration == 24)	note_duration = (NOTE_MIN>>5) * 3;	//	12,		szesnastka + kropka,
	else if(note_duration == 32)	note_duration = NOTE_MIN>>5;		//	4		32,
	else if(note_duration == 48)	note_duration = (NOTE_MIN>>6) * 3;	//	6
	else if(note_duration == 64)	note_duration = NOTE_MIN>>6;		//	2		64,
	else if(note_duration == 96)	note_duration = (NOTE_MIN>>7) * 3;	//	3
	else if(note_duration == 128)	note_duration = NOTE_MIN>>7;		//	1		128,
	else if(note_duration == TRI_8)	note_duration = 11;
	else if(note_duration == TRI_16)note_duration = 5;

	return note_duration;
#undef NOTE_MIN
}





void SM_song_2(SM_t *pSM, SONG_t *pSONG)
{

	/*
	 *
SONG_t PANIE_JANIE_song =
{
	.name = "PAnE",
	.tempo = 120,
	.notes =
	{
		G4, 4, A4, 4, H4, 4, G4, 4,
		G4, 4, A4, 4, H4, 4, G4, 4,
		H4, 4, C5, 4, D5, 2,
		H4, 4, C5, 4, D5, 2,
		D5, 8, E5, 8, D5, 8, C5, 8, H4, 4, G4, 4,
		D5, 8, E5, 8, D5, 8, C5, 8, H4, 4, G4, 4,
		A4, 4, A4, 4, G4, 2,
		A4, 4, A4, 4, G4, 2,
		END_SONG
	}
};

	 *
	 *
	 *
	 *
	 */



	uint16_t i = 0;

	do
	{
		if(pSONG->notes[i] != 1)
		{
			SM_set_steps(pSM, SM_CW, pSONG->notes[i], compute_note_duration_(pSONG->notes[i + 1]));
		}
		while(pSM->steps_to_make != 0);

		i = i + 2;
	}while(pSONG->notes[i] != 0);

	SM_stop(pSM);

}

void SM_song_3ok(SM_t *pSM, SONG_t *pSONG)
{

	/*
	 *
SONG_t PANIE_JANIE_song =
{
	.name = "PAnE",
	.tempo = 120,
	.notes =
	{
		G4, 4, A4, 4, H4, 4, G4, 4,
		G4, 4, A4, 4, H4, 4, G4, 4,
		H4, 4, C5, 4, D5, 2,
		H4, 4, C5, 4, D5, 2,
		D5, 8, E5, 8, D5, 8, C5, 8, H4, 4, G4, 4,
		D5, 8, E5, 8, D5, 8, C5, 8, H4, 4, G4, 4,
		A4, 4, A4, 4, G4, 2,
		A4, 4, A4, 4, G4, 2,
		END_SONG
	}
};

	 *
	 *
	 *
	 *
	 */

	//G4

	uint16_t i = 0;

	do
	{
		if(pSONG->notes[i] != 1)
		{
			uint16_t freq = pSONG->notes[i];
			uint16_t steps = (((pSONG->notes[i]<<2) * 60) / pSONG->notes[i + 1] ) / pSONG->tempo;
			SM_set_steps(pSM, SM_CW, freq, steps);
		}
		while(pSM->steps_to_make != 0);

		i = i + 2;
	}while(pSONG->notes[i] != 0);

	SM_stop(pSM);

}






/*
 * OLD 3

void SM_demo_2(SM_t *pSM)
{

//	F4, 8, E4, 8, F4, 8, G4, 8,		//	Przy - bie - że - li
	//A4, 8, G4, 8, A4, 8, H4b, 8,	//	do Be - tle - jem
	//C5, 4, D5, 4,					//	pa - ste -
	//C5, 2,							//	- rze


	for(uint8_t i = 0; i<1; i++)
	{
		SM_set_steps(pSM, SM_CW, F4, 200);
		while(pSM->steps_to_make != 0);
		SM_set_steps(pSM, SM_CW, E4, 200);
		while(pSM->steps_to_make != 0);
		SM_set_steps(pSM, SM_CW, F4, 200);
		while(pSM->steps_to_make != 0);
		SM_set_steps(pSM, SM_CW, G4, 200);
		while(pSM->steps_to_make != 0);

		SM_set_steps(pSM, SM_CW, A4, 200);
		while(pSM->steps_to_make != 0);
		SM_set_steps(pSM, SM_CW, G4, 200);
		while(pSM->steps_to_make != 0);
		SM_set_steps(pSM, SM_CW, A4, 200);
		while(pSM->steps_to_make != 0);
		SM_set_steps(pSM, SM_CW, H4b, 200);
		while(pSM->steps_to_make != 0);

		SM_set_steps(pSM, SM_CW, C5, 400);
		while(pSM->steps_to_make != 0);
		SM_set_steps(pSM, SM_CW, D5, 400);
		while(pSM->steps_to_make != 0);
		delay_ms(10);

		SM_set_steps(pSM, SM_CW, C5, 800);
		while(pSM->steps_to_make != 0);


		//SM_set_continuous(SM, SM_CW_CONTINUOUS, 600);
		//SM_set_continuous(SM, SM_CCW_CONTINUOUS, 600);
	}
}

void SM_demo(SM_t *pSM)
{

//	F4, 8, E4, 8, F4, 8, G4, 8,		//	Przy - bie - że - li
	//A4, 8, G4, 8, A4, 8, H4b, 8,	//	do Be - tle - jem
	//C5, 4, D5, 4,					//	pa - ste -
	//C5, 2,							//	- rze

	uint8_t j = 200;

	for(uint8_t i = 0; i<1; i++)
	{
		SM_set_continuous(pSM, SM_CW, F4);
		delay_ms(2 * j);
		SM_set_continuous(pSM, SM_CW, E4);
		delay_ms(2 * j);
		SM_set_continuous(pSM, SM_CW, F4);
		delay_ms(2 * j);
		SM_set_continuous(pSM, SM_CW, G4);
		delay_ms(2 * j);

		SM_set_continuous(pSM, SM_CW, A4);
		delay_ms(2 * j);
		SM_set_continuous(pSM, SM_CW, G4);
		delay_ms(2 * j);
		SM_set_continuous(pSM, SM_CW, A4);
		delay_ms(2 * j);
		SM_set_continuous(pSM, SM_CW, H4b);
		delay_ms(2 * j);

		SM_set_continuous(pSM, SM_CW, C5);
		delay_ms(4 * j);
		SM_set_continuous(pSM, SM_CW, D5);
		delay_ms(4 * j);

		SM_set_continuous(pSM, SM_CW, C5);
		delay_ms(8 * j);

		SM_stop(pSM);


		//SM_set_continuous(SM, SM_CW_CONTINUOUS, 600);
		//SM_set_continuous(SM, SM_CCW_CONTINUOUS, 600);
	}
}

void SM_demop(SM_t *pSM)
{
	for(uint16_t i = 0; i<pSM->max_freq; i++)
	{
		SM_set_continuous(pSM, SM_CW, i);
		delay_ms(20);
	}
	SM_stop(pSM);
}



void SM_set_steps_accel(SM_t *pSM, uint8_t time_s, uint16_t steps)
{
	//	PRZYKŁAD:
	//	time_s = 4
	//	steps = 300
	//	n = 5

	uint8_t n = 10;

	uint8_t time_accel = 1;

	uint8_t time_const = time_s - (time_accel<<1);	//	2

	uint16_t f_max = steps / (time_const + time_accel);	//	100

	uint16_t steps_per_const = f_max * time_const;	//	200

	uint16_t steps_per_accel = ((steps - steps_per_const)>>1);	//	50

	uint16_t f_new_basic = f_max / (n + 1);	//	16,66

	uint16_t steps_per_basic = f_new_basic / n;	//	3

	SM_set_steps(pSM, SM_CW, f_new_basic, steps_per_basic);

}



void SM_set_continuous_accel(SM_t *pSM, const uint8_t direction, uint16_t time_s, uint16_t freq, uint8_t interval)
{
	//	PRZYKŁAD:
	//	time_s = 250
	//	freq = 800
	//	interval = 10

	pSM->interval = interval;
	pSM->time_base = time_s / pSM->interval;		//	Czas trwania jednego interwału, 25
	pSM->freq_base = (freq / pSM->interval)>>1;		//	Częstotliwość z jaką generowane są impulsy dla pierwszego interwału, 80
	pSM->steps_to_make = (pSM->time_base * pSM->freq_base) / 1000;	//	Ilość kroków do wykonania w pierwszym interwale,

	SM_set_direction(pSM, direction);
	SM_set_freq(pSM, pSM->freq_base);

	if(pSM->work_mode == SM_STOP)
	{
		LL_TIM_GenerateEvent_UPDATE(pSM->step_line->timer->TIMx);
		LL_TIM_ClearFlag_UPDATE(pSM->step_line->timer->TIMx);
		pSM->work_mode = SM_ACCEL;
		LL_TIM_EnableCounter(pSM->step_line->timer->TIMx);
	}
}




void SM_set_steps(SM_t *pSM, const uint8_t work_mode, uint16_t freq, uint16_t steps)
{
	//	-wykonanie zadanej ilości kroków w wybranym kierunku oraz o zadanej
	//	częstotliwości wykonywania kroków,
	//	-parametr 'work_mode' oznacza zadany kierunek obrotu silnika,
	//	-jeżli obecny tryb pracy to 'STOP', a wartości argumentu 'work_mode' to 'CW' lub 'CCW' to
	//	oznacza, że nastąpią obroty silnika w zadanym kierunku,
	//	-jeżeli silnik jest w trybie innym niż 'STOP' i 'STANDBY' to następuje zatrzymanie,

	if(pSM->work_mode == SM_STOP)
	{
		SM_set_direction(pSM, work_mode);
		SM_set_freq(pSM, freq);
		pSM->steps_to_make = steps;
		pSM->work_mode = work_mode;
		LL_TIM_GenerateEvent_UPDATE(pSM->step_line->timer->TIMx);
		LL_TIM_ClearFlag_UPDATE(pSM->step_line->timer->TIMx);
		LL_TIM_EnableCounter(pSM->step_line->timer->TIMx);
	}
	else if(pSM->work_mode != SM_STANDBY)
	{
		SM_stop(pSM);
	}
}




void SM_set_continuous(SM_t *pSM, const uint8_t direction, const uint16_t freq)
{
	//	-tryb pracy ciągłej z zadanym kierunkiem i częstotliwością,
	//	-jeżeli silnik ma zadaną włączoną liczbę kroków do pokonania,
	//	oraz obraca się w zgodnym kierunkuto to zerujemy ją, co
	//	oznacza, że przechodzimy w tryb pracy ciągłej,


	if(pSM->work_mode == SM_STANDBY)
	{

	}
	//	Wejście do trbu pracy ciągłej z poprzednią częstotliwością:
	else if((pSM->steps_to_make != 0) && (SM_get_direction(pSM) == direction))
	{
		pSM->steps_to_make = 0;
	}
	else if((pSM->steps_to_make == 0) && (SM_get_direction(pSM) == direction))
	{
		if(pSM->step_line->timer->frequency == freq)
		{
			SM_stop(pSM);
		}
		else
		{
			SM_set_freq(pSM, freq);
			pSM->work_mode = direction;
			LL_TIM_EnableCounter(pSM->step_line->timer->TIMx);
		}
	}
	else	//	steps to make == 0 ORAZ niezgodne kierunki
	{
		//	Tryb pracy ciągłej ze stanu spoczynku, włączy się jeżeli direction = STOP,
		SM_set_steps(pSM, direction, freq, 0);
	}
}



 *
 */


/*
 *
 * OLD 2

uint8_t SM_get_direction(SM_t *SM)
{
	//	-pobranie wartości ustawionej na linii sterującej od kierunku obrotów,

	return GPIO_get_output_pin(SM->dir_line);
}

void SM_init(SM_t *SM)
{
	//	-procedura inicjalizacyjna SM,

	GPIO_set_pin_as_output(SM->dir_line); 			//	Ustawienie linii z kierunkiem obrotu,
	GPIO_set_pin_as_output(SM->step_line->line);	//	Ustawienia linii z sygnałem PWM,

	if(SM->microstepping_1 != NULL)
	{
		GPIO_set_pin_as_output(SM->microstepping_1);
	}
	if(SM->microstepping_2 != NULL)
	{
		GPIO_set_pin_as_output(SM->microstepping_2);
	}
	if(SM->microstepping_3 != NULL)
	{
		GPIO_set_pin_as_output(SM->microstepping_3);
	}
	if(SM->enable_line != NULL)
	{
		GPIO_set_pin_as_output(SM->enable_line);
	}

	uint32_t auto_reload = __LL_TIM_CALC_ARR(SYS_CORE_CLOCK, 500 - 1, 100) + 1;

	////////////////////////////////////////////////////////////////////////////
	LL_APB1_GRP1_EnableClock   (LL_APB1_GRP1_PERIPH_TIM2);
	LL_TIM_EnableARRPreload    (TIM2);
	LL_TIM_SetClockSource      (TIM2, LL_TIM_CLOCKSOURCE_INTERNAL);
	LL_TIM_SetCounterMode      (TIM2, LL_TIM_COUNTERMODE_UP);
	LL_TIM_SetPrescaler        (TIM2, 500 - 1);
	LL_TIM_SetAutoReload       (TIM2, auto_reload - 1);
	LL_TIM_OC_SetMode          (TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_PWM1);
	LL_TIM_OC_SetPolarity      (TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_OCPOLARITY_HIGH);
	LL_TIM_OC_EnablePreload    (TIM2, LL_TIM_CHANNEL_CH1);
	LL_TIM_OC_SetCompareCH1    (TIM2, 0);
	LL_TIM_CC_EnableChannel    (TIM2, LL_TIM_CHANNEL_CH1);
	LL_TIM_EnableIT_CC1        (TIM2);
	LL_TIM_GenerateEvent_UPDATE(TIM2);
	LL_TIM_ClearFlag_UPDATE    (TIM2);
	//LL_TIM_EnableCounter       (TIM2);
}

uint8_t SM_set_direction(SM_t *SM, const uint8_t direction)
{
	//	-ustawienie kierunku obrotów silnika krokowego,
	//	-jeśli zadany kierunek nie jest zmieniany to nic nie jest wykonywane,
	//	-inne zachowanie jeśli zmiana następuje podczas trybu STOP, a inne
	//	dla każdegp innego trybu,

	//	Jeżeli kierunki obrotów są zgodne:
	if(direction == SM_CW_STEP || direction == SM_CW_CONTINUOUS)
	{
		GPIO_set_pin(SM->dir_line);
		return 1;
	}
	//	Jeżeli kierunki obrotów są przeciwne:
	else if(direction == SM_CCW_STEP || direction == SM_CCW_CONTINUOUS)
	{
		GPIO_reset_pin(SM->dir_line);
		return 0;
	}

	return 2;
}

uint8_t SM_set_freq(SM_t *SM, uint16_t freq)
{
	//	-ustawienie częstotliwości sygnału PWM, który steruje prędkością silnika,

	//	Jeżeli częstotliwości są takie same:
	if(freq == SM->step_line->timer->frequency)
	{
		return 0;
	}
	//	Jeżeli częstotliwość zadana jest większa od maksymalnej możliwej:
	else if(freq > SM->max_freq)
	{
		freq = SM->max_freq;

		if(freq == SM->step_line->timer->frequency)
		{
			return 0;
		}
	}
	//	Jeżeli częstotliwość jest równa zero to jest to równoważne z
	//	zatrzymaniem silnika,
	else if(freq == 0)
	{
		SM_stop(SM);
		return 0;
	}

	//	Zmiana parametru częstotliwości:
	SM->step_line->timer->frequency = freq;
	uint32_t auto_reload = __LL_TIM_CALC_ARR(SYS_CORE_CLOCK, SM->step_line->timer->preskaler - 1, SM->step_line->timer->frequency) + 1;
	LL_TIM_SetAutoReload(SM->step_line->timer->TIMx, auto_reload - 1);
	LL_TIM_OC_SetCompareCH1(SM->step_line->timer->TIMx, (auto_reload>>1) - 1);

	return 1;
}



void SM_set_standby(SM_t *SM)
{
	//	-wyłączenie zasilania do silnika SM,

	GPIO_reset_pin(SM->enable_line);

	SM->step_line->timer->frequency = 0;
	LL_TIM_DisableCounter(SM->step_line->timer->TIMx);
	LL_TIM_SetCounter(SM->step_line->timer->TIMx, 0);

	SM->steps_counter = 0;
	SM->steps_to_make = 0;

	SM->work_mode = SM_STANDBY;
}

void SM_stop(SM_t *SM)
{
	//	-zatrzymanie obrotów silnika krokowego,
	//	-można to wykonać na kilka sposobów:
	//	-zmiana trybu pracy na: STOP,


	GPIO_set_pin(SM->enable_line);

	SM->step_line->timer->frequency = 0;

	LL_TIM_DisableCounter(SM->step_line->timer->TIMx);
	LL_TIM_SetCounter(SM->step_line->timer->TIMx, 0);
	SM->steps_counter = 0;
	SM->steps_to_make = 0;

	uint32_t auto_reload = __LL_TIM_CALC_ARR(SYS_CORE_CLOCK, SM->step_line->timer->preskaler - 1, SM->step_line->timer->frequency) + 1;
	LL_TIM_SetAutoReload(SM->step_line->timer->TIMx, auto_reload - 1);
	LL_TIM_OC_SetCompareCH1(SM->step_line->timer->TIMx, (auto_reload>>1) - 1);

	SM->work_mode = SM_STOP;
}

void SM_set_steps(SM_t *SM, const uint8_t direction, uint16_t freq, uint16_t steps)
{
	//	-wykonanie zadanej ilości kroków w wybranym kierunku oraz o zadanej
	//	częstotliwości wykonywania kroków,

	if(SM->work_mode == SM_STANDBY)
	{

	}
	//else if(SM->work_mode == SM_STOP || SM->work_mode == direction)	//	Brak zatrzymania przy powtórzonym trybie pracy,
	else if(SM->work_mode == SM_STOP)	//	Zatrzymanie przy powtórzonym wywołaniu trybu pracy,
	{
		SM->work_mode = direction;	//	Wchodzimy do trybu obrotu o zadany kąt,

		SM_set_direction(SM, direction);
		SM_set_freq(SM, freq);
		SM->steps_to_make = steps;
		LL_TIM_EnableCounter(SM->step_line->timer->TIMx);
	}
	else
	{
		SM_stop(SM);
	}
}

void SM_set_rotations(SM_t *SM, const uint8_t direction, const uint16_t time_s, const uint16_t rotations)
{
	//	-wykonanie zadanej ilości obrotów w wybranym kierunku z zadanym czasem całkowitym ich wykonania,

	uint16_t steps = rotations * SM->steps_per_revolution;
	uint16_t freq_steps = steps / time_s;

	SM_set_steps(SM, direction, freq_steps, steps);
}

void SM_set_continuous(SM_t *SM, const uint8_t direction, const uint16_t freq)
{
	//	-tryb pracy ciągłej z zadanym kierunkiem i częstotliwością,

	if(SM->work_mode == SM_STANDBY)
	{

	}
	else if(SM->work_mode == SM_STOP || SM->work_mode == direction)	//	Brak zatrzymania przy powtórzonym trybie pracy,
	//else if(SM->work_mode == SM_STOP)	//	Zatrzymanie przy powtórzonym wywołaniu trybu pracy,
	{
		SM->work_mode = direction;	//	Wchodzimy do trybu obrotu o zadany kąt,

		SM_set_direction(SM, direction);
		SM_set_freq(SM, freq);

		if(SM->step_line->timer->frequency == 0)
		{

		}
		else
		{
			LL_TIM_EnableCounter(SM->step_line->timer->TIMx);
		}
	}
	//	Jeżeli wybrano ten sam kierunek ALE tryb pracy przechodzi z krokowego na ciągły:
	else if((SM->work_mode == SM_CW_STEP && direction == SM_CW_CONTINUOUS) || (SM->work_mode == SM_CCW_STEP && direction == SM_CCW_CONTINUOUS))
	{
		SM->work_mode = direction;	//	Wchodzimy do trybu obrotu o zadany kąt,

		SM_set_direction(SM, direction);
		SM_set_freq(SM, freq);
		SM->steps_to_make = 0;
		LL_TIM_EnableCounter(SM->step_line->timer->TIMx);
	}
	else
	{
		SM_stop(SM);
	}
}

void SM_set_microstepping(SM_t *SM, uint8_t microstepping)
{
	//	-ustawienie wartości mikrokroków,

	if(microstepping == FULL_STEP)
	{
		GPIO_reset_pin(SM->microstepping_1);
		GPIO_reset_pin(SM->microstepping_2);
		GPIO_reset_pin(SM->microstepping_3);
		SM->microstep_mode = FULL_STEP;
	}
	else if(microstepping == HALF_STEP)
	{
		GPIO_set_pin(SM->microstepping_1);
		GPIO_reset_pin(SM->microstepping_2);
		GPIO_reset_pin(SM->microstepping_3);
		SM->microstep_mode = HALF_STEP;
	}
	else if(microstepping == QUARTER_STEP)
	{
		GPIO_reset_pin(SM->microstepping_1);
		GPIO_set_pin(SM->microstepping_2);
		GPIO_reset_pin(SM->microstepping_3);
		SM->microstep_mode = QUARTER_STEP;
	}
	else if(microstepping == EIGHT_STEP)
	{
		GPIO_set_pin(SM->microstepping_1);
		GPIO_set_pin(SM->microstepping_2);
		GPIO_reset_pin(SM->microstepping_3);
		SM->microstep_mode = EIGHT_STEP;
	}
	else if(microstepping == SIXTEENTH_STEP)
	{
		GPIO_set_pin(SM->microstepping_1);
		GPIO_set_pin(SM->microstepping_2);
		GPIO_set_pin(SM->microstepping_3);
		SM->microstep_mode = SIXTEENTH_STEP;
	}
}

void SM_inc_dec_freq(SM_t *SM, const int16_t value)
{
	//	-zmiana częstotliwości o zadaną wartość,

	if(SM->work_mode == SM_CW_CONTINUOUS || SM->work_mode == SM_CCW_CONTINUOUS)
	{
		uint16_t new_value = 0;

		if(value < 0)
		{
			new_value = (-1) * value;
		}
		else
		{
			new_value = value;
		}


		uint16_t new_freq = SM->step_line->timer->frequency + new_value;


		if((new_freq > SM->max_freq) && (value > 0))
		{
			new_freq = SM->max_freq;
		}
		else if((new_freq > SM->max_freq) && (value < 0))
		{
			new_freq = 0;
		}
		else
		{
			new_freq = SM->step_line->timer->frequency + value;
		}

		SM_set_continuous(SM, SM->work_mode, new_freq);
	}
}

void SM_demo_2(SM_t *SM)
{

//	F4, 8, E4, 8, F4, 8, G4, 8,		//	Przy - bie - że - li
	//A4, 8, G4, 8, A4, 8, H4b, 8,	//	do Be - tle - jem
	//C5, 4, D5, 4,					//	pa - ste -
	//C5, 2,							//	- rze


	for(uint8_t i = 0; i<1; i++)
	{
		SM_set_steps(SM, SM_CW_STEP, F4, 200);
		while(SM->steps_to_make != 0);
		SM_set_steps(SM, SM_CW_STEP, E4, 200);
		while(SM->steps_to_make != 0);
		SM_set_steps(SM, SM_CW_STEP, F4, 200);
		while(SM->steps_to_make != 0);
		SM_set_steps(SM, SM_CW_STEP, G4, 200);
		while(SM->steps_to_make != 0);

		SM_set_steps(SM, SM_CW_STEP, A4, 200);
		while(SM->steps_to_make != 0);
		SM_set_steps(SM, SM_CW_STEP, G4, 200);
		while(SM->steps_to_make != 0);
		SM_set_steps(SM, SM_CW_STEP, A4, 200);
		while(SM->steps_to_make != 0);
		SM_set_steps(SM, SM_CW_STEP, H4b, 200);
		while(SM->steps_to_make != 0);

		SM_set_steps(SM, SM_CW_STEP, C5, 400);
		while(SM->steps_to_make != 0);
		SM_set_steps(SM, SM_CW_STEP, D5, 400);
		while(SM->steps_to_make != 0);
		delay_ms(10);

		SM_set_steps(SM, SM_CW_STEP, C5, 800);
		while(SM->steps_to_make != 0);


		//SM_set_continuous(SM, SM_CW_CONTINUOUS, 600);
		//SM_set_continuous(SM, SM_CCW_CONTINUOUS, 600);
	}
}

void SM_demo_(SM_t *SM)
{

//	F4, 8, E4, 8, F4, 8, G4, 8,		//	Przy - bie - że - li
	//A4, 8, G4, 8, A4, 8, H4b, 8,	//	do Be - tle - jem
	//C5, 4, D5, 4,					//	pa - ste -
	//C5, 2,							//	- rze

	uint8_t j = 200;

	for(uint8_t i = 0; i<1; i++)
	{
		SM_set_continuous(SM, SM_CW_STEP, F4);
		delay_ms(2 * j);
		SM_set_continuous(SM, SM_CW_STEP, E4);
		delay_ms(2 * j);
		SM_set_continuous(SM, SM_CW_STEP, F4);
		delay_ms(2 * j);
		SM_set_continuous(SM, SM_CW_STEP, G4);
		delay_ms(2 * j);

		SM_set_continuous(SM, SM_CW_STEP, A4);
		delay_ms(2 * j);
		SM_set_continuous(SM, SM_CW_STEP, G4);
		delay_ms(2 * j);
		SM_set_continuous(SM, SM_CW_STEP, A4);
		delay_ms(2 * j);
		SM_set_continuous(SM, SM_CW_STEP, H4b);
		delay_ms(2 * j);

		SM_set_continuous(SM, SM_CW_STEP, C5);
		delay_ms(4 * j);
		SM_set_continuous(SM, SM_CW_STEP, D5);
		delay_ms(4 * j);

		SM_set_continuous(SM, SM_CW_STEP, C5);
		delay_ms(8 * j);

		SM_stop(SM);


		//SM_set_continuous(SM, SM_CW_CONTINUOUS, 600);
		//SM_set_continuous(SM, SM_CCW_CONTINUOUS, 600);
	}
}

void SM_demo(SM_t *SM)
{
	for(uint16_t i = 0; i<SM->max_freq; i++)
	{
		SM_set_continuous(SM, SM_CW_STEP, i);
		delay_ms(20);
	}
	SM_stop(SM);
}



*/


/*

OLD 1

uint8_t SM_get_direction(SM_t *SM)
{
	//	-pobranie wartości ustawionej na linii sterującej od kierunku obrotów,

	return GPIO_get_output_pin(SM->dir_line);
}

void SM_init(SM_t *SM)
{
	//	-procedura inicjalizacyjna SM,

	GPIO_set_pin_as_output(SM->dir_line); 			//	Ustawienie linii z kierunkiem obrotu,
	GPIO_set_pin_as_output(SM->step_line->line);	//	Ustawienia linii z sygnałem PWM,

	if(SM->microstepping_1 != NULL)
	{
		GPIO_set_pin_as_output(SM->microstepping_1);
	}
	if(SM->microstepping_2 != NULL)
	{
		GPIO_set_pin_as_output(SM->microstepping_2);
	}
	if(SM->microstepping_3 != NULL)
	{
		GPIO_set_pin_as_output(SM->microstepping_3);
	}
	if(SM->enable_line != NULL)
	{
		GPIO_set_pin_as_output(SM->enable_line);
	}

	uint32_t auto_reload = __LL_TIM_CALC_ARR(SYS_CORE_CLOCK, 500 - 1, 100) + 1;

	////////////////////////////////////////////////////////////////////////////
	LL_APB1_GRP1_EnableClock   (LL_APB1_GRP1_PERIPH_TIM2);
	LL_TIM_EnableARRPreload    (TIM2);
	LL_TIM_SetClockSource      (TIM2, LL_TIM_CLOCKSOURCE_INTERNAL);
	LL_TIM_SetCounterMode      (TIM2, LL_TIM_COUNTERMODE_UP);
	LL_TIM_SetPrescaler        (TIM2, 500 - 1);
	LL_TIM_SetAutoReload       (TIM2, auto_reload - 1);
	LL_TIM_OC_SetMode          (TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_PWM1);
	LL_TIM_OC_SetPolarity      (TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_OCPOLARITY_HIGH);
	LL_TIM_OC_EnablePreload    (TIM2, LL_TIM_CHANNEL_CH1);
	LL_TIM_OC_SetCompareCH1    (TIM2, 0);
	LL_TIM_CC_EnableChannel    (TIM2, LL_TIM_CHANNEL_CH1);
	LL_TIM_EnableIT_CC1        (TIM2);
	LL_TIM_GenerateEvent_UPDATE(TIM2);
	LL_TIM_ClearFlag_UPDATE    (TIM2);
	//LL_TIM_EnableCounter       (TIM2);
}

void SM_stop(SM_t *SM)
{
	//	-zatrzymanie obrotów silnika krokowego,
	//	-można to wykonać na kilka sposobów:
	//	-zmiana trybu pracy na: STOP,


	GPIO_set_pin(SM->enable_line);

	SM->step_line->timer->frequency = 0;
	LL_TIM_DisableCounter(SM->step_line->timer->TIMx);
	LL_TIM_SetCounter(SM->step_line->timer->TIMx, 0);

	SM->work_mode = SM_STOP;
}

uint8_t SM_set_direction(SM_t *SM, const uint8_t direction)
{
	//	-ustawienie kierunku obrotów silnika krokowego,
	//	-jeśli zadany kierunek nie jest zmieniany to nic nie jest wykonywane,
	//	-inne zachowanie jeśli zmiana następuje podczas trybu STOP, a inne
	//	dla każdegp innego trybu,

	//	Jeżeli kierunki obrotów są zgodne:
	if(direction == SM_get_direction(SM))
	{
		return 0;
	}
	//	Jeżeli kierunki obrotów są przeciwne:
	else
	{
		//	Jeżeli silnik wykonuje obroty to trzeba go zatrzymać:
		if(SM->work_mode != SM_STOP)
		{
			uint8_t tmp_work_mode = SM->work_mode;
			SM_stop(SM);
			delay_ms(500);
			SM->work_mode = tmp_work_mode;
		}

		GPIO_toggle_pin(SM->dir_line);

		return 1;
	}
}

uint8_t SM_set_freq(SM_t *SM, uint16_t freq)
{
	//	-ustawienie częstotliwości sygnału PWM, który steruje prędkością silnika,

	//	Jeżeli częstotliwości są takie same:
	if(freq == SM->step_line->timer->frequency)
	{
		return 0;
	}
	//	Jeżeli częstotliwość zadana jest większa od maksymalnej możliwej:
	else if(freq > SM->max_freq)
	{
		freq = SM->max_freq;

		if(freq == SM->step_line->timer->frequency)
		{
			return 0;
		}
	}
	//	Jeżeli częstotliwość jest równa zero to jest to równoważne z
	//	zatrzymaniem silnika,
	else if(freq == 0)
	{
		SM_stop(SM);
		return 0;
	}

	//	Zmiana parametru częstotliwości:
	SM->step_line->timer->frequency = freq;
	uint32_t auto_reload = __LL_TIM_CALC_ARR(SYS_CORE_CLOCK, SM->step_line->timer->preskaler - 1, SM->step_line->timer->frequency) + 1;
	LL_TIM_SetAutoReload(SM->step_line->timer->TIMx, auto_reload - 1);
	LL_TIM_OC_SetCompareCH1(SM->step_line->timer->TIMx, (auto_reload>>1) - 1);

	return 1;
}

void SM_set_steps(SM_t *SM, const uint8_t direction, uint16_t freq, uint16_t steps)
{
	//	-wykonanie zadanej ilości kroków w wybranym kierunku oraz o zadanej
	//	częstotliwości wykonywania kroków,

	if(SM->work_mode == SM_STANDBY)
	{

	}
	else if(SM->work_mode == SM_STOP || SM->work_mode != direction)
	{
		SM_stop(SM);
	}
	else
	{
		SM->work_mode = direction;	//	Wchodzimy do trybu obrotu o zadany kąt,

		SM_set_direction(SM, direction);
		SM_set_freq(SM, freq);
		SM->steps_to_make = steps;
		LL_TIM_EnableCounter(SM->step_line->timer->TIMx);
	}
}

void SM_set_rotations(SM_t *SM, const uint8_t direction, const uint16_t time_s, const uint16_t rotations)
{
	//	-wykonanie zadanej ilości obrotów w wybranym kierunku z zadanym czasem całkowitym ich wykonania,

	uint16_t steps = rotations * SM->steps_per_revolution;
	uint16_t freq_steps = steps / time_s;

	SM_set_steps(SM, direction, freq_steps, steps);
}

void SM_set_continuous(SM_t *SM, const uint8_t direction, const uint16_t freq)
{
	//	-tryb pracy ciągłej z zadanym kierunkiem i częstotliwością,

	if(SM_set_direction(SM, direction) || SM_set_freq(SM, freq))
	{
		SM->work_mode = CONTINUOUS;
		LL_TIM_EnableCounter(SM->step_line->timer->TIMx);
	}
}

void SM_set_microstepping(SM_t *SM, uint8_t microstepping)
{
	//	-ustawienie wartości mikrokroków,

	if(microstepping == FULL_STEP)
	{
		GPIO_reset_pin(SM->microstepping_1);
		GPIO_reset_pin(SM->microstepping_2);
		GPIO_reset_pin(SM->microstepping_3);
		SM->microstep_mode = FULL_STEP;
	}
	else if(microstepping == HALF_STEP)
	{
		GPIO_set_pin(SM->microstepping_1);
		GPIO_reset_pin(SM->microstepping_2);
		GPIO_reset_pin(SM->microstepping_3);
		SM->microstep_mode = HALF_STEP;
	}
	else if(microstepping == QUARTER_STEP)
	{
		GPIO_reset_pin(SM->microstepping_1);
		GPIO_set_pin(SM->microstepping_2);
		GPIO_reset_pin(SM->microstepping_3);
		SM->microstep_mode = QUARTER_STEP;
	}
	else if(microstepping == EIGHT_STEP)
	{
		GPIO_set_pin(SM->microstepping_1);
		GPIO_set_pin(SM->microstepping_2);
		GPIO_reset_pin(SM->microstepping_3);
		SM->microstep_mode = EIGHT_STEP;
	}
	else if(microstepping == SIXTEENTH_STEP)
	{
		GPIO_set_pin(SM->microstepping_1);
		GPIO_set_pin(SM->microstepping_2);
		GPIO_set_pin(SM->microstepping_3);
		SM->microstep_mode = SIXTEENTH_STEP;
	}
}

void SM_set_standby(SM_t *SM)
{
	//	-wyłączenie zasilania do silnika SM,

	GPIO_reset_pin(SM->enable_line);

	SM->step_line->timer->frequency = 0;
	LL_TIM_DisableCounter(SM->step_line->timer->TIMx);
	LL_TIM_SetCounter(SM->step_line->timer->TIMx, 0);

	SM->work_mode = SM_STANDBY;
}

void SM_reset_enable(SM_t *SM)
{
	//	-wyłączenie zasilania do silnika SM,

	GPIO_set_pin(SM->enable_line);
}
*/

