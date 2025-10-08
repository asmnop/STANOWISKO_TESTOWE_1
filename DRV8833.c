/*
 * DRV8833.c
 *
 *  Created on: 7 lis 2024
 *      Author: asmnop
 */


#include "../../LIB/Inc/DRV8833.h"
#include "../../LIB/Inc/universal_functions.h"


//	TODO: -dodanie informacji o obecnym stanie pracy sterownika,

//	SPOSOBY PISANIA KODU:
//	-utworzenie funkcji sterujących włączeniem hamulca, wrzuceniem na luz, CW, CCW,
//	-PYTANIE: czy takie funkcje pozostawić na wyjściu do obsługi czy może wykonać
//	funkcję zbiorczą, której parametrem będzie tryb pracy sterownika,
//	-albo dwie funkcjące wyjściowe: jedna do sterowania hamowaniem, druga do sterowania
//	kierunkiem i wypełnieniem,
//	-stosując funkcję sterującą hamowaniem, wypełnienie z automatu wynosi 0,


/*
 * UWAGI:
 * -stan domyślny na liniach:
 * 00 --> ZZ - szybkie zamykanie,
 * 11 --> LL - hamulec, wolne zamykanie,
 * -w trybie sterowania PWM/PWM nie można ustawić parametru kierunku obrotów bez ich włączenia,
 * -nie pozwala na to tabela prawdy,
 * -są dwie opcje konfiguracji: sterowanie za pomocą linii ustawionych jako wyjścia czyli tylko
 * ustawiamy stan H/L i wtedy mamy sterowanie z wypełnieniem 100%,
 * -sterowanie za pomocą linii ustawionych jako AF z podawaniem sygnału PWM na kanały podłączone
 * do linii uC, wtedy możemy sterować tak jak powyżej czyli podająć współczynnik wypełnienia CCR=ARR lub CCR = 0
 * ale można również sterować wypełnieniem sygnału,
 * -tak więc kod funkcji 'DRV8833_CW' oraz 'DRV8833_CCW' jest pusty,
 * https://www.allaboutcircuits.com/technical-articles/difference-slow-decay-mode-fast-decay-mode-h-bridge-dc-motor-applications/
 *
 */

const char* MOTOR_STATE[] =
{
	"FORWARD / IDLE",	//	0
	"FORWARD / BRAKE",	//	1
	"REVERSE / IDLE",	//	2
	"REVERSE / BRAKE",	//	3
	"IDLE",				//	4
	"BRAKE",			//	5
};

void DRV8833_set_driver_state(DRV8833_t *pDC, uint8_t driver_state)
{
	pDC->driver_state = driver_state;

	/*
	if(DRV8833_get_driver_state(pDC) != driver_state)
	{
		switch(driver_state)
		{
			case CW_MOTOR:
			{
				DRV8833_CW(pDC);
				break;
			}
			case CCW_MOTOR:
			{
				DRV8833_CCW(pDC);
				break;
			}
			case STOP_MOTOR:
			{
				DRV8833_idle(pDC);
				break;
			}
			case BRAKE_MOTOR:
			{
				DRV8833_brake(pDC);
				break;
			}
		}

		pDC->driver_state = driver_state;
	}
	*/
}

void DRV8833_init(const DRV8833_t *pDC)
{
	//	-inicjalizacja działania sterownika silnika DC,

	GPIO_set_pin_as_output(pDC->input_1);
	GPIO_set_pin_as_output(pDC->input_2);

	if(pDC->standby != NULL)
	{
		GPIO_set_pin_as_output(pDC->standby);
	}

	TIM_basic_init(pDC->tim_mode_1->timer);

	TIM_mode_init(pDC->tim_mode_1);
	TIM_mode_init(pDC->tim_mode_2);

	LL_TIM_CC_EnableChannel(pDC->tim_mode_1->timer->TIMx, pDC->tim_mode_1->channel);
	LL_TIM_CC_EnableChannel(pDC->tim_mode_2->timer->TIMx, pDC->tim_mode_2->channel);

	LL_TIM_EnableUpdateEvent(pDC->tim_mode_1->timer->TIMx);
	LL_TIM_EnableIT_UPDATE(pDC->tim_mode_1->timer->TIMx);
	LL_TIM_EnableCounter(pDC->tim_mode_1->timer->TIMx);
	TIM_start_counting(pDC->tim_mode_1->timer->TIMx);
}

void DRV8833_brake(DRV8833_t *pDC)
{
	//	-gwałtowne hamowanie, pedał hamulca wciśnięty do końca,

	const uint16_t ARR = LL_TIM_GetAutoReload(pDC->tim_mode_1->timer->TIMx) + 1;

	NVIC_DisableIRQ(TIM1_UP_TIM16_IRQn);
	DRV8833_set_driver_state(pDC, BRAKE);
	pDC->tim_mode_1->duty = ARR;
	pDC->tim_mode_2->duty = ARR;
	NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);

	if(pDC->standby != NULL)
	{
		GPIO_set_pin(pDC->standby);
	}
}

void DRV8833_idle(DRV8833_t *pDC)
{
	//	-bieg jałowy, hamowanie swobodne, wrzucenie na luz i czekanie na zatrzymanie silnika,

	NVIC_DisableIRQ(TIM1_UP_TIM16_IRQn);
	DRV8833_set_driver_state(pDC, IDLE);
	pDC->tim_mode_1->duty = 0;
	pDC->tim_mode_2->duty = 0;
	NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);
}

void DRV8833_CW(DRV8833_t *pDC)
{
	//	-przełączenie na liniach sterujących do generowania obrotów w CW_MOTOR,
	//	-jeśli wybrany kierunek obrotów jest niezgodny z oczekiwanym to można
	//	albo zmienić funkcję na CCW_MOTOR albo zamienić linie sterujące ze sobą,
	//	-funkcja nie sprawdza jaki stan pracy był poprzednio na liniach,

	GPIO_set_pin(pDC->input_1);
	GPIO_reset_pin(pDC->input_2);
	if(pDC->standby != NULL)
	{
		GPIO_set_pin(pDC->standby);
	}

	//DRV8833_set_driver_state(pDC, CW_DRV8833);
}

void DRV8833_CCW(DRV8833_t *pDC)
{
	//	-przełączenie na liniach sterujących do generowania obrotów w CCW_MOTOR,
	//	-jeśli wybrany kierunek obrotów jest niezgodny z oczekiwanym to można
	//	albo zmienić funkcję na CW_MOTOR albo zamienić linie sterujące ze sobą,
	//	-funkcja nie sprawdza jaki stan pracy był poprzednio na liniach,

	GPIO_reset_pin(pDC->input_1);
	GPIO_set_pin(pDC->input_2);
	if(pDC->standby != NULL)
	{
		GPIO_set_pin(pDC->standby);
	}

	//DRV8833_set_driver_state(pDC, CCW_DRV8833);
}

void DRV8833_standby(DRV8833_t *pDC)
{
	//	-ustawienie sterownika w stan wstrzymania,

	GPIO_reset_pin(pDC->standby);

	DRV8833_set_driver_state(pDC, STANDBY);
}

void DRV8833_set_duty(DRV8833_t *pDC, const uint16_t duty_1, const uint16_t duty_2)
{
	//	-ustawienie prędkości poprzez ustawienie wypełnienia sygnału PWM,

	NVIC_DisableIRQ(TIM1_UP_TIM16_IRQn);
	pDC->tim_mode_1->duty = duty_1;

	/*
	if(pDC->tim_mode_1->channel == LL_TIM_CHANNEL_CH1)	LL_TIM_OC_SetCompareCH1(pDC->tim_mode_1->timer->TIMx, pDC->tim_mode_1->duty);
	else if(pDC->tim_mode_1->channel == LL_TIM_CHANNEL_CH2)	LL_TIM_OC_SetCompareCH2(pDC->tim_mode_1->timer->TIMx, pDC->tim_mode_1->duty);
	else if(pDC->tim_mode_1->channel == LL_TIM_CHANNEL_CH3)	LL_TIM_OC_SetCompareCH3(pDC->tim_mode_1->timer->TIMx, pDC->tim_mode_1->duty);
	else if(pDC->tim_mode_1->channel == LL_TIM_CHANNEL_CH4)	LL_TIM_OC_SetCompareCH4(pDC->tim_mode_1->timer->TIMx, pDC->tim_mode_1->duty);
	else if(pDC->tim_mode_1->channel == LL_TIM_CHANNEL_CH5)	LL_TIM_OC_SetCompareCH5(pDC->tim_mode_1->timer->TIMx, pDC->tim_mode_1->duty);
	else if(pDC->tim_mode_1->channel == LL_TIM_CHANNEL_CH6)	LL_TIM_OC_SetCompareCH6(pDC->tim_mode_1->timer->TIMx, pDC->tim_mode_1->duty);
*/
	pDC->tim_mode_2->duty = duty_2;
	NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);

	/*
	if(pDC->tim_mode_2->channel == LL_TIM_CHANNEL_CH1)	LL_TIM_OC_SetCompareCH1(pDC->tim_mode_2->timer->TIMx, pDC->tim_mode_2->duty);
	else if(pDC->tim_mode_2->channel == LL_TIM_CHANNEL_CH2)	LL_TIM_OC_SetCompareCH2(pDC->tim_mode_2->timer->TIMx, pDC->tim_mode_2->duty);
	else if(pDC->tim_mode_2->channel == LL_TIM_CHANNEL_CH3)	LL_TIM_OC_SetCompareCH3(pDC->tim_mode_2->timer->TIMx, pDC->tim_mode_2->duty);
	else if(pDC->tim_mode_2->channel == LL_TIM_CHANNEL_CH4)	LL_TIM_OC_SetCompareCH4(pDC->tim_mode_2->timer->TIMx, pDC->tim_mode_2->duty);
	else if(pDC->tim_mode_2->channel == LL_TIM_CHANNEL_CH5)	LL_TIM_OC_SetCompareCH5(pDC->tim_mode_2->timer->TIMx, pDC->tim_mode_2->duty);
	else if(pDC->tim_mode_2->channel == LL_TIM_CHANNEL_CH6)	LL_TIM_OC_SetCompareCH6(pDC->tim_mode_2->timer->TIMx, pDC->tim_mode_2->duty);
*/
}

uint16_t DRV8833_get_duty(DRV8833_t *pMOTOR)
{
	//	-wyznaczene wartości współczynnika wypełnienia w zależności od
	//	obecnego trybu pracy,

	const uint8_t driver_state = DRV8833_get_driver_state(pMOTOR);

	if((driver_state == FORWARD_IDLE) || (driver_state == REVERSE_BRAKE))
	{
		return pMOTOR->tim_mode_1->duty;
	}
	else if((driver_state == FORWARD_BRAKE) || (driver_state == REVERSE_IDLE))
	{
		return pMOTOR->tim_mode_2->duty;
	}
	else
	{
		return 0;
	}
}

void DRV8833_set_direction(DRV8833_t *pDC, const uint8_t direction)
{
	//	-ustawienie kierunku obrotów,
	//	-zabezpieczenie przed zmianą kierunku obrotów jeżeli wał obraca
	//	się w przeciwnym kierunku,

	//if( (pDC->direction != direction) )
	{
		//pDC->direction = direction;

		//if(direction == CW_DRV8833)
		{
			DRV8833_CW(pDC);
		}
		//else if(direction == CCW_DRV8833)
		{
			DRV8833_CCW(pDC);
		}
	}
}

uint8_t DRV8833_get_driver_state(DRV8833_t *pDC)
{
	//	-ustawienie informacji o aktywnym stanie pracy sterownika,

	return pDC->driver_state;
}

void DRV8833_check_direction(DRV8833_t *pMOTOR, uint8_t driver_state)
{
	//	-sprawdzenie czy nowo zadany kierunek obrotu pokrywa się z obecnie trwającym,
	//	-jeśli kierunki są niezgodne to generowane jest zatrzymanie silnika,

	const uint16_t delay = 500;	//	Czas odczekania na zatrzymanie silnika,

	if((DRV8833_get_driver_state(pMOTOR) == FORWARD_IDLE) || (DRV8833_get_driver_state(pMOTOR) == FORWARD_BRAKE))
	{
		if((driver_state == REVERSE_IDLE) || (driver_state == REVERSE_BRAKE))
		{
			DRV8833_idle(pMOTOR);
			delay_ms(delay);
		}
	}
	else if((DRV8833_get_driver_state(pMOTOR) == REVERSE_IDLE) || (DRV8833_get_driver_state(pMOTOR) == REVERSE_BRAKE))
	{
		if((driver_state == FORWARD_IDLE) || (driver_state == FORWARD_BRAKE))
		{
			DRV8833_idle(pMOTOR);
			delay_ms(delay);
		}
	}
}

void DRV8833_ride_(DRV8833_t *pMOTOR, uint16_t speed, const uint8_t driver_state)
{
	//	-rozpoczęcie obrotów silnika na zadane wypełnienie i tryb pracy,

	const uint16_t ARR = LL_TIM_GetAutoReload(pMOTOR->tim_mode_1->timer->TIMx) + 1;

	if((driver_state == FORWARD_BRAKE) || (driver_state == REVERSE_BRAKE))
	{
		speed = ARR - speed;
	}

	if((DRV8833_get_duty(pMOTOR) == 0) && (DRV8833_get_duty(pMOTOR) == ARR))
	{
		const uint16_t IN1[4] = {speed, ARR,   0,     speed};
		const uint16_t IN2[4] = {0,     speed, speed, ARR};

		//DRV8833_check_direction(pMOTOR, driver_state);
		DRV8833_set_duty(pMOTOR, IN1[driver_state], IN2[driver_state]);
		DRV8833_set_driver_state(pMOTOR, driver_state + GOING_FORWARD_IDLE);
	}
	else if(DRV8833_get_driver_state(pMOTOR) == driver_state)
	{
		const uint16_t IN1[4] = {speed, ARR,   0,     speed};
		const uint16_t IN2[4] = {0,     speed, speed, ARR};

		//DRV8833_check_direction(pMOTOR, driver_state);
		DRV8833_set_duty(pMOTOR, IN1[driver_state], IN2[driver_state]);
		DRV8833_set_driver_state(pMOTOR, driver_state + GOING_FORWARD_IDLE);
	}
	//else if(DRV8833_set_driver_state(pMOTOR) == FORWARD_IDLE && driver_state == FORWARD_BRAKE)
	else
	{
		const uint16_t IN1[4] = {speed, ARR,   0,     speed};
		const uint16_t IN2[4] = {0,     speed, speed, ARR};

		DRV8833_set_duty(pMOTOR, IN1[driver_state], IN2[driver_state]);
		DRV8833_set_driver_state(pMOTOR, driver_state + GOING_FORWARD_IDLE);
	}



/*

	if(((DRV8833_get_duty(pMOTOR) != 0) && (DRV8833_get_duty(pMOTOR) != ARR)) || speed != 0)
	{
		const uint16_t IN1[4] = {speed, ARR,   0,     speed};
		const uint16_t IN2[4] = {0,     speed, speed, ARR};

		DRV8833_check_direction(pMOTOR, driver_state);
		DRV8833_set_duty(pMOTOR, IN1[driver_state], IN2[driver_state]);
	}
*/
	DRV8833_set_driver_state(pMOTOR, driver_state + GOING_FORWARD_IDLE);
}

void DRV8833_inc_dec(DRV8833_t *pMOTOR, int16_t value)
{
	//	-zmiana współczynnika wypełnienia o zadaną wartość,

	if((DRV8833_get_driver_state(pMOTOR) == FORWARD_BRAKE) || (DRV8833_get_driver_state(pMOTOR) == REVERSE_BRAKE))
	{
		value = value * (-1);
	}


	uint16_t new_duty = DRV8833_get_duty(pMOTOR) + value;

	//uint32_t auto_reload = __LL_TIM_CALC_ARR(SYS_CORE_CLOCK, pMOTOR->tim_mode_1->timer->preskaler - 1, pMOTOR->tim_mode_1->timer->frequency) + 1;
	uint32_t auto_reload = __LL_TIM_CALC_ARR(SYS_CORE_CLOCK, LL_TIM_GetPrescaler(pMOTOR->tim_mode_1->timer->TIMx), pMOTOR->tim_mode_1->timer->frequency) + 1;

	if((new_duty > auto_reload) && (value > 0))
	{
		new_duty = auto_reload;
	}
	else if((new_duty > auto_reload) && (value < 0))
	{
		new_duty = 0;
	}


	const uint16_t ARR = LL_TIM_GetAutoReload(pMOTOR->tim_mode_1->timer->TIMx) + 1;

	if((DRV8833_get_driver_state(pMOTOR) == FORWARD_BRAKE) || (DRV8833_get_driver_state(pMOTOR) == REVERSE_BRAKE))
	{
		new_duty = ARR - new_duty;
	}

	if(DRV8833_get_driver_state(pMOTOR) < 4)
	{
		DRV8833_ride(pMOTOR, new_duty, DRV8833_get_driver_state(pMOTOR));
	}
}

uint8_t DRV8833_inc_dec_value(DRV8833_t *pMOTOR, uint8_t go)
{
	//	-zmiana kroku,


	static uint8_t jump = 4;

	if(go)
	{
		jump = inc(jump, 6);
	}

	uint8_t arr[7] = {1, 2, 5, 10, 20, 50, 100};

	return arr[jump];
}


void DRV8833_ride__(DRV8833_t *pMOTOR, uint16_t speed, const uint8_t driver_state)
{
	//	-rozpoczęcie obrotów silnika na zadane wypełnienie i tryb pracy,

	const uint16_t ARR = LL_TIM_GetAutoReload(pMOTOR->tim_mode_1->timer->TIMx) + 1;

	if((driver_state == FORWARD_BRAKE) || (driver_state == REVERSE_BRAKE))
	{
		speed = ARR - speed;
	}



	//	/////////////////////////////////////////
	if(DRV8833_get_driver_state(pMOTOR) == FORWARD_BRAKE)




	if((DRV8833_get_duty(pMOTOR) == 0) && (DRV8833_get_duty(pMOTOR) == ARR))
	{
		const uint16_t IN1[4] = {speed, ARR,   0,     speed};
		const uint16_t IN2[4] = {0,     speed, speed, ARR};

		DRV8833_check_direction(pMOTOR, driver_state);
		DRV8833_set_duty(pMOTOR, IN1[driver_state], IN2[driver_state]);
		DRV8833_set_driver_state(pMOTOR, driver_state + GOING_FORWARD_IDLE);
	}
	else if(DRV8833_get_driver_state(pMOTOR) == driver_state)
	{
		const uint16_t IN1[4] = {speed, ARR,   0,     speed};
		const uint16_t IN2[4] = {0,     speed, speed, ARR};

		//DRV8833_check_direction(pMOTOR, driver_state);
		DRV8833_set_duty(pMOTOR, IN1[driver_state], IN2[driver_state]);
		DRV8833_set_driver_state(pMOTOR, driver_state + GOING_FORWARD_IDLE);
	}
	//else if(DRV8833_set_driver_state(pMOTOR) == FORWARD_IDLE && driver_state == FORWARD_BRAKE)
	else
	{
		const uint16_t IN1[4] = {speed, ARR,   0,     speed};
		const uint16_t IN2[4] = {0,     speed, speed, ARR};

		DRV8833_set_duty(pMOTOR, IN1[driver_state], IN2[driver_state]);
		DRV8833_set_driver_state(pMOTOR, driver_state + GOING_FORWARD_IDLE);
	}



/*

	if(((DRV8833_get_duty(pMOTOR) != 0) && (DRV8833_get_duty(pMOTOR) != ARR)) || speed != 0)
	{
		const uint16_t IN1[4] = {speed, ARR,   0,     speed};
		const uint16_t IN2[4] = {0,     speed, speed, ARR};

		DRV8833_check_direction(pMOTOR, driver_state);
		DRV8833_set_duty(pMOTOR, IN1[driver_state], IN2[driver_state]);
	}
*/
	DRV8833_set_driver_state(pMOTOR, driver_state + GOING_FORWARD_IDLE);
}

void DRV8833_ride4(DRV8833_t *pMOTOR, uint16_t speed, uint8_t driver_state)
{
	//	-sprawdzenie czy nowo zadany kierunek obrotu pokrywa się z obecnie trwającym,
	//	-jeśli kierunki są niezgodne to generowane jest zatrzymanie silnika,

	const uint16_t ARR = LL_TIM_GetAutoReload(pMOTOR->tim_mode_1->timer->TIMx) + 1;

	if((driver_state == FORWARD_BRAKE) || (driver_state == REVERSE_BRAKE))
	{
		speed = ARR - speed;
	}

	const uint16_t delay = 500;	//	Czas odczekania na zatrzymanie silnika,

	if((speed == 0))
	{
		const uint16_t IN1[4] = {speed, ARR,   0,     speed};
		const uint16_t IN2[4] = {0,     speed, speed, ARR};

		//DRV8833_check_direction(pMOTOR, driver_state);
		DRV8833_set_duty(pMOTOR, IN1[driver_state], IN2[driver_state]);
		DRV8833_set_driver_state(pMOTOR, driver_state + GOING_FORWARD_IDLE);
	}
	else if((DRV8833_get_driver_state(pMOTOR) == FORWARD_IDLE) || (DRV8833_get_driver_state(pMOTOR) == FORWARD_BRAKE))
	{
		if((driver_state == REVERSE_IDLE) || (driver_state == REVERSE_BRAKE))
		{
			DRV8833_idle(pMOTOR);
			delay_ms(delay);
		}
		else
		{
			const uint16_t IN1[4] = {speed, ARR,   0,     speed};
			const uint16_t IN2[4] = {0,     speed, speed, ARR};

			//DRV8833_check_direction(pMOTOR, driver_state);
			DRV8833_set_duty(pMOTOR, IN1[driver_state], IN2[driver_state]);
			DRV8833_set_driver_state(pMOTOR, driver_state + GOING_FORWARD_IDLE);
		}
	}
	else if((DRV8833_get_driver_state(pMOTOR) == REVERSE_IDLE) || (DRV8833_get_driver_state(pMOTOR) == REVERSE_BRAKE))
	{
		if((driver_state == FORWARD_IDLE) || (driver_state == FORWARD_BRAKE))
		{
			DRV8833_idle(pMOTOR);
			delay_ms(delay);
		}
		else
		{
			const uint16_t IN1[4] = {speed, ARR,   0,     speed};
			const uint16_t IN2[4] = {0,     speed, speed, ARR};

			//DRV8833_check_direction(pMOTOR, driver_state);
			DRV8833_set_duty(pMOTOR, IN1[driver_state], IN2[driver_state]);
			DRV8833_set_driver_state(pMOTOR, driver_state + GOING_FORWARD_IDLE);
		}
	}
}

void DRV8833_ride(DRV8833_t *pMOTOR, uint16_t speed, uint8_t driver_state)
{
	//	-sprawdzenie czy nowo zadany kierunek obrotu pokrywa się z obecnie trwającym,
	//	-jeśli kierunki są niezgodne to generowane jest zatrzymanie silnika,

	const uint16_t ARR = LL_TIM_GetAutoReload(pMOTOR->tim_mode_1->timer->TIMx) + 1;

	if((driver_state == FORWARD_BRAKE) || (driver_state == REVERSE_BRAKE))
	{
		speed = ARR - speed;
	}

	//const uint16_t delay = 500;	//	Czas odczekania na zatrzymanie silnika,
	const uint16_t IN1[4] = {speed, ARR,   0,     speed};
	const uint16_t IN2[4] = {0,     speed, speed, ARR};

	if((DRV8833_get_driver_state(pMOTOR) == IDLE) || (DRV8833_get_driver_state(pMOTOR) == BRAKE))
	{
		DRV8833_set_duty(pMOTOR, IN1[driver_state], IN2[driver_state]);
		DRV8833_set_driver_state(pMOTOR, driver_state);
	}
	else if((DRV8833_get_driver_state(pMOTOR) == FORWARD_IDLE) || (DRV8833_get_driver_state(pMOTOR) == FORWARD_BRAKE))
	{
		if((driver_state == REVERSE_IDLE))
		{
			DRV8833_idle(pMOTOR);
		}
		else if((driver_state == REVERSE_BRAKE))
		{
			DRV8833_brake(pMOTOR);
		}
		else
		{
			DRV8833_set_duty(pMOTOR, IN1[driver_state], IN2[driver_state]);
		}
		DRV8833_set_driver_state(pMOTOR, driver_state);
	}
	else if((DRV8833_get_driver_state(pMOTOR) == REVERSE_IDLE) || (DRV8833_get_driver_state(pMOTOR) == REVERSE_BRAKE))
	{
		if((driver_state == FORWARD_IDLE))
		{
			DRV8833_idle(pMOTOR);
		}
		else if((driver_state == FORWARD_BRAKE))
		{
			DRV8833_brake(pMOTOR);
		}
		else
		{
			DRV8833_set_duty(pMOTOR, IN1[driver_state], IN2[driver_state]);
		}
		DRV8833_set_driver_state(pMOTOR, driver_state);
	}
}



