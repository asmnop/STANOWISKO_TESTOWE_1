/*
 * led.c
 *
 *  Created on: 13 lis 2024
 *      Author: asmnop
 */


#include "..\..\LIB\Inc\led.h"

void LED_LINE_init_all(CHANNEL_t **pPWM, const uint8_t length)
{
	//	inicjalizacja kilku diód LED leżących na tzw. listwie,

	for(uint8_t i=0; i<length; i++)
	{
		LED_LINE_init(*(pPWM + i));
	}
}


void LED_LINE_set_duty(CHANNEL_t *pPWM, const uint32_t duty)
{
	pPWM->duty = duty;

	if(pPWM->channel == LL_TIM_CHANNEL_CH1)			LL_TIM_OC_SetCompareCH1(pPWM->timer->TIMx, duty);
	else if((*pPWM).channel == LL_TIM_CHANNEL_CH2)	LL_TIM_OC_SetCompareCH2(pPWM->timer->TIMx, duty);
	else if((*pPWM).channel == LL_TIM_CHANNEL_CH3)	LL_TIM_OC_SetCompareCH3(pPWM->timer->TIMx, duty);
	else if((*pPWM).channel == LL_TIM_CHANNEL_CH4)	LL_TIM_OC_SetCompareCH4(pPWM->timer->TIMx, duty);
}

void LED_LINE_set_duty_all(CHANNEL_t **pPWM, uint32_t *duty, const uint8_t length)
{
	//	-nadanie wszystkim diodom współczynnika wypełnienia,

	for(uint8_t i=0; i<length; i++)
	{
		LED_LINE_set_duty(*(pPWM + i), *(duty + i));
	}
}

uint32_t LED_LINE_get_duty(CHANNEL_t *pPWM)
{
	if(pPWM->channel == LL_TIM_CHANNEL_CH1)			return LL_TIM_OC_GetCompareCH1(pPWM->timer->TIMx);
	else if((*pPWM).channel == LL_TIM_CHANNEL_CH2)	return LL_TIM_OC_GetCompareCH2(pPWM->timer->TIMx);
	else if((*pPWM).channel == LL_TIM_CHANNEL_CH3)	return LL_TIM_OC_GetCompareCH3(pPWM->timer->TIMx);
	else if((*pPWM).channel == LL_TIM_CHANNEL_CH4)	return LL_TIM_OC_GetCompareCH4(pPWM->timer->TIMx);
	return SYS_CORE_CLOCK;
}


void LED_LINE_enable_IT(CHANNEL_t *pPWM)
{
	//	-włączenie przerwań od zgodnego porównania wartości CNT z CCRx,

	if(pPWM->channel == LL_TIM_CHANNEL_CH1)			LL_TIM_EnableIT_CC1(pPWM->timer->TIMx);
	else if((*pPWM).channel == LL_TIM_CHANNEL_CH2)	LL_TIM_EnableIT_CC2(pPWM->timer->TIMx);
	else if((*pPWM).channel == LL_TIM_CHANNEL_CH3)	LL_TIM_EnableIT_CC3(pPWM->timer->TIMx);
	else if((*pPWM).channel == LL_TIM_CHANNEL_CH4)	LL_TIM_EnableIT_CC4(pPWM->timer->TIMx);
}

void LED_LINE_enable_IT_all(CHANNEL_t **pPWM, const uint8_t length)
{
	//	-włączenie obsługi przerwań od zgodnego porównania wartości CNT z CCRx dla
	//	wybranych obiektów,

	for(uint8_t i=0; i<length; i++)
	{
		LED_LINE_enable_IT(*pPWM++);
	}
}


void LED_LINE_disable_IT(CHANNEL_t *pPWM)
{
	//	-wyłączenie przerwań od zgodnego porównania wartości CNT z CCRx,

	if(pPWM->channel == LL_TIM_CHANNEL_CH1)			LL_TIM_DisableIT_CC1(pPWM->timer->TIMx);
	else if((*pPWM).channel == LL_TIM_CHANNEL_CH2)	LL_TIM_DisableIT_CC2(pPWM->timer->TIMx);
	else if((*pPWM).channel == LL_TIM_CHANNEL_CH3)	LL_TIM_DisableIT_CC3(pPWM->timer->TIMx);
	else if((*pPWM).channel == LL_TIM_CHANNEL_CH4)	LL_TIM_DisableIT_CC4(pPWM->timer->TIMx);
}

void LED_LINE_disable_IT_all(CHANNEL_t **pPWM, const uint8_t length)
{
	//	-wyłączenie obsługi przerwań od zgodnego porównania wartości CNT z CCRx dla
	//	wybranych obiektów,

	for(uint8_t i=0; i<length; i++)
	{
		LED_LINE_disable_IT(*pPWM++);
	}
}


void LED_LINE_set_mode(CHANNEL_t *pPWM, const uint32_t work_mode)
{
	pPWM->work_mode = work_mode;

	LL_TIM_OC_SetMode(pPWM->timer->TIMx, (*pPWM).channel, (*pPWM).work_mode);
}

uint32_t LED_LINE_get_mode(CHANNEL_t *pPWM)
{
	return LL_TIM_OC_GetMode(pPWM->timer->TIMx, pPWM->channel);
}

void LED_LINE_set_mode_all(CHANNEL_t **pPWM, uint32_t *work_mode, const uint8_t length)
{
	for(uint8_t i=0; i<length; i++)
	{
		LED_LINE_set_mode(*(pPWM + i), *(work_mode + i));
		//(**(pPWM + i)).work_mode = *(work_mode + i);
		//LL_TIM_OC_SetMode((**(pPWM + i)).timer->TIMx, (**(pPWM + i)).channel, (**(pPWM + i)).work_mode);
	}
}


void LED_LINE_set(CHANNEL_t *pPWM)
{
	//	-ustawienie maksymalnego wypełnienia sygnału PWM na danej linii,
	//	-używane do zapalenia diody LED z całą mocą,

	LL_TIM_OC_SetMode(pPWM->timer->TIMx, pPWM->channel, LL_TIM_OCMODE_FORCED_ACTIVE);

	/*
	if(pPWM->work_mode == LL_TIM_OCMODE_PWM1)
	{
		LED_LINE_set_duty(pPWM, __LL_TIM_CALC_ARR(SYS_CORE_CLOCK, pPWM->timer->preskaler - 1, pPWM->timer->frequency) + 1);

	}
	else if(pPWM->work_mode == LL_TIM_OCMODE_PWM2)
	{
		LED_LINE_set_duty(pPWM, 0);
	}
	*/
}

void LED_LINE_reset(CHANNEL_t *pPWM)
{
	//	-ustawienie zerowego wypełnienia sygnału PWM na danel linii,
	//	-używane do zgaszenia diody LED,

	LL_TIM_OC_SetMode(pPWM->timer->TIMx, pPWM->channel, LL_TIM_OCMODE_FORCED_INACTIVE);

	/*
	if(pPWM->work_mode == LL_TIM_OCMODE_PWM1)
	{
		LED_LINE_set_duty(pPWM, 0);
	}
	else if(pPWM->work_mode == LL_TIM_OCMODE_PWM2)
	{
		LED_LINE_set_duty(pPWM, __LL_TIM_CALC_ARR(SYS_CORE_CLOCK, pPWM->timer->preskaler - 1, pPWM->timer->frequency) + 1);
	}
	*/
}

void LED_LINE_toggle(CHANNEL_t *pPWM)
{
	if(LED_LINE_get_duty(pPWM) == 0)
	{
		LED_LINE_set(pPWM);
	}
	else if(LED_LINE_get_duty(pPWM) == 1)
	{
		LED_LINE_reset(pPWM);
	}
	else
	{
		LED_LINE_reset(pPWM);
	}
}

void LED_LINE_state(CHANNEL_t *pPWM, const uint8_t state)
{
	if(state == 1)
	{
		LED_LINE_set(pPWM);
	}
	else
	{
		LED_LINE_reset(pPWM);
	}

}

void LED_LINE_TERMINAL_driver(CHANNEL_t **pPWM, const uint8_t data_read)
{
	for(uint8_t i=0; i<8; i++)
	{
		if(data_read == '1' + i)
		{
			LED_LINE_toggle(*(pPWM + i));
		}
	}

	if(data_read == '9')
	{
		uint32_t duty[8] = {1000, 1000, 1000, 1000, 1000, 1000, 1000, 1000};
		LED_LINE_set_duty_all(pPWM, duty, 8);
	}

	if(data_read == '0')
	{
		uint32_t duty[8] = {0, 0, 0, 0, 0, 0, 0, 0};
		LED_LINE_set_duty_all(pPWM, duty, 8);
	}
}

void LED_LINE_return_to_PWM(CHANNEL_t *pPWM)
{

	LL_TIM_OC_SetMode(pPWM->timer->TIMx, pPWM->channel, LL_TIM_OCMODE_PWM1);

}








