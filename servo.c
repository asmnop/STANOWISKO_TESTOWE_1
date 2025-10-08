/*
 * servo.c
 *
 *  Created on: 25 lut 2025
 *      Author: asmnop
 */


#include "../../LIB/Inc/servo.h"
#include "../../LIB/Inc/universal_functions.h"


void SERVO_init(SERVO_t *pSERVO)
{
	//	-procedura inicjalizacyjna serva,

	GPIO_set_pin_as_output(pSERVO->signal_line->line); 			//	Ustawienie linii z kierunkiem obrotu,

	////////////////////////////////////////////////////////////////////////////
	LL_APB2_GRP1_EnableClock   (LL_APB2_GRP1_PERIPH_TIM15);

	uint32_t prescal = __LL_TIM_CALC_PSC(SYS_CORE_CLOCK, pSERVO->signal_line->timer->NEW_FREQ);

	//	Wartość preskalera, preskaler 16-bitowy!!! wartości od 1..65535:
	//LL_TIM_SetPrescaler(pSM->step_line->timer->TIMx, prescal);		//	TIM_CR1_DIR | TIM_CR1_CMS

	uint32_t auto_reload = __LL_TIM_CALC_ARR(SYS_CORE_CLOCK, prescal, pSERVO->signal_line->timer->frequency) + 1;



	LL_TIM_EnableARRPreload    (pSERVO->signal_line->timer->TIMx);
	LL_TIM_SetClockSource      (pSERVO->signal_line->timer->TIMx, pSERVO->signal_line->timer->clock_source);
	LL_TIM_SetCounterMode      (pSERVO->signal_line->timer->TIMx, pSERVO->signal_line->timer->counter_mode);
	LL_TIM_SetPrescaler        (pSERVO->signal_line->timer->TIMx, prescal);
	LL_TIM_SetAutoReload       (pSERVO->signal_line->timer->TIMx, auto_reload - 1);
	LL_TIM_OC_SetMode          (pSERVO->signal_line->timer->TIMx, pSERVO->signal_line->channel, pSERVO->signal_line->work_mode);
	LL_TIM_OC_SetPolarity      (pSERVO->signal_line->timer->TIMx, pSERVO->signal_line->channel, LL_TIM_OCPOLARITY_HIGH);
	LL_TIM_OC_EnablePreload    (pSERVO->signal_line->timer->TIMx, pSERVO->signal_line->channel);
	LL_TIM_OC_SetCompareCH1    (pSERVO->signal_line->timer->TIMx, pSERVO->signal_line->duty);
	LL_TIM_CC_EnableChannel    (pSERVO->signal_line->timer->TIMx, pSERVO->signal_line->channel);
	//LL_TIM_EnableIT_CC1        (pServo->signal_line->timer->TIMx);
	LL_TIM_GenerateEvent_UPDATE(pSERVO->signal_line->timer->TIMx);
	LL_TIM_ClearFlag_UPDATE    (pSERVO->signal_line->timer->TIMx);
	LL_TIM_EnableAllOutputs    (pSERVO->signal_line->timer->TIMx);
	LL_TIM_EnableCounter       (pSERVO->signal_line->timer->TIMx);

	for(uint16_t i = 0; i<pSERVO->total_size; i++)
	{
		pSERVO->data[i] = i + pSERVO->min_value;
		pSERVO->data[i + (pSERVO->max_value - pSERVO->min_value)] = pSERVO->max_value - i;
	}
}

static void SERVO_is_duty_correct(SERVO_t *pSERVO, uint16_t value)
{
	if(value > pSERVO->max_value)
	{
		pSERVO->signal_line->duty = pSERVO->max_value;
	}
	else if(value < pSERVO->min_value)
	{
		pSERVO->signal_line->duty = pSERVO->min_value;
	}
	else
	{
		pSERVO->signal_line->duty = value;
	}
}

void SERVO_set_duty(SERVO_t *pSERVO, const uint16_t value)
{
	SERVO_is_duty_correct(pSERVO, value);
	LL_TIM_OC_SetCompareCH1(pSERVO->signal_line->timer->TIMx, pSERVO->signal_line->duty);
}

void SERVO_set_min(SERVO_t *pSERVO)
{
	SERVO_set_duty(pSERVO, pSERVO->min_value);
}

void SERVO_set_max(SERVO_t *pSERVO)
{
	SERVO_set_duty(pSERVO, pSERVO->max_value);
}

void SERVO_set_center(SERVO_t *pSERVO)
{
	SERVO_set_duty(pSERVO, pSERVO->min_value + ((pSERVO->max_value - pSERVO->min_value)>>1));
}

void SERVO_inc(SERVO_t *pSERVO)
{
	const uint16_t duty = LL_TIM_OC_GetCompareCH1(pSERVO->signal_line->timer->TIMx);
	SERVO_set_duty(pSERVO, duty + 1);
}

void SERVO_dec(SERVO_t *pSERVO)
{
	const uint16_t duty = LL_TIM_OC_GetCompareCH1(pSERVO->signal_line->timer->TIMx);
	SERVO_set_duty(pSERVO, duty - 1);
}

void SERVO_set_angle(SERVO_t *pSERVO, uint16_t angle)
{
	uint16_t value = pSERVO->min_value + (angle * (pSERVO->max_value - pSERVO->min_value)) / 180;
	SERVO_set_duty(pSERVO, value);
}

uint16_t SERVO_get_global_position(SERVO_t *pSERVO)
{
	//	-podaje globalną pozycję w krokach od minimalnej dolnej pozycji,

	return LL_TIM_OC_GetCompareCH1(pSERVO->signal_line->timer->TIMx) - pSERVO->min_value;
}

uint16_t SERVO_get_angle_global_position(SERVO_t *pSERVO)
{
	//	-podaje globalną pozycję w kącie od minimalnej dolnej pozycji,

	return ((LL_TIM_OC_GetCompareCH1(pSERVO->signal_line->timer->TIMx) - pSERVO->min_value) * 180) / (pSERVO->max_value - pSERVO->min_value);
}




void SERVO_demo(SERVO_t *pSERVO)
{
	for(uint8_t i = 50; i< 100; i++)
	{
		SERVO_set_duty(pSERVO, i);
		delay_ms(500);
	}


	SERVO_set_duty(pSERVO, 55);
	delay_ms(500);
	SERVO_set_duty(pSERVO, 80);
	delay_ms(500);
	SERVO_set_duty(pSERVO, 55);
	delay_ms(500);
	SERVO_set_duty(pSERVO, 90);
	delay_ms(500);
}




