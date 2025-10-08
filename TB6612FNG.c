/*
 * TB6612FNG.c
 *
 *  Created on: 7 lis 2024
 *      Author: asmnop
 */


#include "..\..\LIB\Inc\TB6612FNG.h"
#include "..\..\LIB\Inc\universal_functions.h"


//	TODO: -dodanie informacji o obecnym stanie pracy sterownika,

//	SPOSOBY PISANIA KODU:
//	-utworzenie funkcji sterujących włączeniem hamulca, wrzuceniem na luz, CW, CCW,
//	-PYTANIE: czy takie funkcje pozostawić na wyjściu do obsługi czy może wykonać
//	funkcję zbiorczą, której parametrem będzie tryb pracy sterownika,
//	-albo dwie funkcjące wyjściowe: jedna do sterowania hamowaniem, druga do sterowania
//	kierunkiem i wypełnieniem,
//	-stosując funkcję sterującą hamowaniem, wypełnienie z automatu wynosi 0,


static void TB6612FNG_set_driver_state(TB6612FNG_t *pDC, TB6612FNG_STATE_e driver_state)
{
	pDC->driver_state = driver_state;

	/*
	if(TB6612FNG_get_driver_state(pDC) != driver_state)
	{
		switch(driver_state)
		{
			case CW_MOTOR:
			{
				TB6612FNG_CW(pDC);
				break;
			}
			case CCW_MOTOR:
			{
				TB6612FNG_CCW(pDC);
				break;
			}
			case STOP_MOTOR:
			{
				TB6612FNG_stop(pDC);
				break;
			}
			case BRAKE_MOTOR:
			{
				TB6612FNG_short_brake(pDC);
				break;
			}
		}

		pDC->driver_state = driver_state;
	}
	*/
}


void TB6612FNG_init(const TB6612FNG_t *pDC)
{
	//	-inicjalizacja działania sterownika slnika DC,

	GPIO_set_pin_as_output(pDC->input_1);
	GPIO_set_pin_as_output(pDC->input_2);
	GPIO_set_pin_as_output(pDC->standby);
	GPIO_set_pin_as_output(pDC->pwm);
	TIM_basic_init(pDC->tim_mode->timer);
	TIM_mode_init(pDC->tim_mode);
	LL_TIM_CC_EnableChannel(pDC->tim_mode->timer->TIMx, pDC->tim_mode->channel);
	LL_TIM_EnableUpdateEvent(pDC->tim_mode->timer->TIMx);
	LL_TIM_EnableCounter(pDC->tim_mode->timer->TIMx);
	TIM_start_counting(pDC->tim_mode->timer->TIMx);
}

void TB6612FNG_short_brake(TB6612FNG_t *pDC)
{
	//	-gwałtowne hamowanie, pedał hamulca wciśnięty do końca,

	GPIO_set_pin(pDC->input_1);
	GPIO_set_pin(pDC->input_2);
	GPIO_set_pin(pDC->standby);
	TB6612FNG_set_duty(pDC, 0);

	TB6612FNG_set_driver_state(pDC, BRAKE_TB6612FNG);
}

void TB6612FNG_stop(TB6612FNG_t *pDC)
{
	//	-hamowanie swobodne, wrzucenie na luz i czekanie na zatrzymanie,

	GPIO_reset_pin(pDC->input_1);
	GPIO_reset_pin(pDC->input_2);
	GPIO_set_pin(pDC->standby);
	TB6612FNG_set_duty(pDC, 0);

	TB6612FNG_set_driver_state(pDC, STOP_TB6612FNG);
}

void TB6612FNG_CW(TB6612FNG_t *pDC)
{
	//	-przełączenie na liniach sterujących do generowania obrotów w CW_MOTOR,
	//	-jeśli wybrany kierunek obrotów jest niezgodny z oczekiwanym to można
	//	albo zmienić funkcję na CCW_MOTOR albo zamienić linie sterujące ze sobą,
	//	-funkcja nie sprawdza jaki stan pracy był poprzednio na liniach,

	GPIO_set_pin(pDC->input_1);
	GPIO_reset_pin(pDC->input_2);
	GPIO_set_pin(pDC->standby);

	TB6612FNG_set_driver_state(pDC, CW_TB6612FNG);
}

void TB6612FNG_CCW(TB6612FNG_t *pDC)
{
	//	-przełączenie na liniach sterujących do generowania obrotów w CCW_MOTOR,
	//	-jeśli wybrany kierunek obrotów jest niezgodny z oczekiwanym to można
	//	albo zmienić funkcję na CW_MOTOR albo zamienić linie sterujące ze sobą,
	//	-funkcja nie sprawdza jaki stan pracy był poprzednio na liniach,

	GPIO_reset_pin(pDC->input_1);
	GPIO_set_pin(pDC->input_2);
	GPIO_set_pin(pDC->standby);

	TB6612FNG_set_driver_state(pDC, CCW_TB6612FNG);
}

void TB6612FNG_standby(TB6612FNG_t *pDC)
{
	//	-ustawienie sterownika w stan wstrzymania,

	GPIO_reset_pin(pDC->standby);

	TB6612FNG_set_driver_state(pDC, STANDBY_TB6612FNG);
}

void TB6612FNG_set_duty(TB6612FNG_t *pDC, const uint16_t duty)
{
	//	-ustawienie prędkości poprzez ustawienie wypełnienia sygnału PWM,

	pDC->duty = duty;
	if(pDC->tim_mode->channel == LL_TIM_CHANNEL_CH1)	LL_TIM_OC_SetCompareCH1(pDC->tim_mode->timer->TIMx, pDC->duty);
	else if(pDC->tim_mode->channel == LL_TIM_CHANNEL_CH2)	LL_TIM_OC_SetCompareCH2(pDC->tim_mode->timer->TIMx, pDC->duty);
	else if(pDC->tim_mode->channel == LL_TIM_CHANNEL_CH3)	LL_TIM_OC_SetCompareCH3(pDC->tim_mode->timer->TIMx, pDC->duty);
	else if(pDC->tim_mode->channel == LL_TIM_CHANNEL_CH4)	LL_TIM_OC_SetCompareCH4(pDC->tim_mode->timer->TIMx, pDC->duty);
	else if(pDC->tim_mode->channel == LL_TIM_CHANNEL_CH5)	LL_TIM_OC_SetCompareCH5(pDC->tim_mode->timer->TIMx, pDC->duty);
	else if(pDC->tim_mode->channel == LL_TIM_CHANNEL_CH6)	LL_TIM_OC_SetCompareCH6(pDC->tim_mode->timer->TIMx, pDC->duty);
}

uint16_t TB6612FNG_get_duty(TB6612FNG_t *pDC)
{
	return pDC->tim_mode->duty;
}

void TB6612FNG_set_direction(TB6612FNG_t *pDC, const uint8_t direction)
{
	//	-ustawienie kierunku obrotów,
	//	-zabezpieczenie przed zmianą kierunku obrotów jeżeli wał obraca
	//	się w przeciwnym kierunku,

	//if( (pDC->direction != direction) )
	{
		//pDC->direction = direction;

		if(direction == CW_TB6612FNG)
		{
			TB6612FNG_CW(pDC);
		}
		else if(direction == CCW_TB6612FNG)
		{
			TB6612FNG_CCW(pDC);
		}
	}
}

TB6612FNG_STATE_e TB6612FNG_get_driver_state(TB6612FNG_t *pDC)
{
	//	-ustawienie informacji o aktywnym stanie pracy sterownika,

	return pDC->driver_state;
}



