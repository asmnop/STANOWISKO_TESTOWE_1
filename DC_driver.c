/*
 * DC_driver.c
 *
 *  Created on: 7 lis 2024
 *      Author: asmnop
 */


#include "..\..\LIB\Inc\DC_driver.h"
#include "..\..\LIB\Inc\universal_functions.h"


//	TODO: -dodanie informacji o obecnym stanie pracy sterownika,

//	SPOSOBY PISANIA KODU:
//	-utworzenie funkcji sterujących włączeniem hamulca, wrzuceniem na luz, CW, CCW,
//	-PYTANIE: czy takie funkcje pozostawić na wyjściu do obsługi czy może wykonać
//	funkcję zbiorczą, której parametrem będzie tryb pracy sterownika,
//	-albo dwie funkcjące wyjściowe: jedna do sterowania hamowaniem, druga do sterowania
//	kierunkiem i wypełnieniem,
//	-stosując funkcję sterującą hamowaniem, wypełnienie z automatu wynosi 0,


static void DC_DRIVER_set_driver_state(DC_DRIVER_t *pDC, DRIVER_STATE_e driver_state)
{

	if(pDC->DRV8833_driver != NULL)
	{
		DRV8833_set_driver_state(pDC->DRV8833_driver, driver_state);
	}
	else if(pDC->TB6612FNG_driver != NULL)
	{
		TB6612FNG_set_driver_state(pDC->TB6612FNG_driver, driver_state);
	}

	//pDC->driver_state = driver_state;

	/*
	if(DC_DRIVER_get_driver_state(pDC) != driver_state)
	{
		switch(driver_state)
		{
			case CW_MOTOR:
			{
				DC_DRIVER_CW(pDC);
				break;
			}
			case CCW_MOTOR:
			{
				DC_DRIVER_CCW(pDC);
				break;
			}
			case STOP_MOTOR:
			{
				DC_DRIVER_stop(pDC);
				break;
			}
			case BRAKE_MOTOR:
			{
				DC_DRIVER_short_brake(pDC);
				break;
			}
		}

		pDC->driver_state = driver_state;
	}
	*/
}


void DC_DRIVER_init(const DC_DRIVER_t *pDC)
{
	//	-inicjalizacja działania sterownika slnika DC,

	if(pDC->DRV8833_driver != NULL)
	{
		DRV8833_init(pDC->DRV8833_driver);
	}
	else if(pDC->TB6612FNG_driver != NULL)
	{
		TB6612FNG_init(pDC->TB6612FNG_driver);
	}
}

void DC_DRIVER_short_brake(DC_DRIVER_t *pDC)
{
	//	-gwałtowne hamowanie, pedał hamulca wciśnięty do końca,

	if(pDC->DRV8833_driver != NULL)
	{
		DRV8833_brake(pDC->DRV8833_driver);
	}
	else if(pDC->TB6612FNG_driver != NULL)
	{
		TB6612FNG_short_brake(pDC->TB6612FNG_driver);
	}
}

void DC_DRIVER_stop(DC_DRIVER_t *pDC)
{
	//	-hamowanie swobodne, wrzucenie na luz i czekanie na zatrzymanie,

	if(pDC->DRV8833_driver != NULL)
	{
		DRV8833_idle(pDC->DRV8833_driver);
	}
	else if(pDC->TB6612FNG_driver != NULL)
	{
		TB6612FNG_stop(pDC->TB6612FNG_driver);
	}
}

void DC_DRIVER_CW(DC_DRIVER_t *pDC)
{
	//	-przełączenie na liniach sterujących do generowania obrotów w CW_MOTOR,
	//	-jeśli wybrany kierunek obrotów jest niezgodny z oczekiwanym to można
	//	albo zmienić funkcję na CCW_MOTOR albo zamienić linie sterujące ze sobą,
	//	-funkcja nie sprawdza jaki stan pracy był poprzednio na liniach,


	if(pDC->DRV8833_driver != NULL)
	{
		DRV8833_CW(pDC->DRV8833_driver);
	}
	else if(pDC->TB6612FNG_driver != NULL)
	{
		TB6612FNG_CW(pDC->TB6612FNG_driver);
	}
}

void DC_DRIVER_CCW(DC_DRIVER_t *pDC)
{
	//	-przełączenie na liniach sterujących do generowania obrotów w CCW_MOTOR,
	//	-jeśli wybrany kierunek obrotów jest niezgodny z oczekiwanym to można
	//	albo zmienić funkcję na CW_MOTOR albo zamienić linie sterujące ze sobą,
	//	-funkcja nie sprawdza jaki stan pracy był poprzednio na liniach,

	if(pDC->DRV8833_driver != NULL)
	{
		DRV8833_CCW(pDC->DRV8833_driver);
	}
	else if(pDC->TB6612FNG_driver != NULL)
	{
		TB6612FNG_CCW(pDC->TB6612FNG_driver);
	}
}

void DC_DRIVER_standby(DC_DRIVER_t *pDC)
{
	//	-ustawienie sterownika w stan wstrzymania,

	if(pDC->DRV8833_driver != NULL)
	{
		DRV8833_standby(pDC->DRV8833_driver);
	}
	else if(pDC->TB6612FNG_driver != NULL)
	{
		TB6612FNG_standby(pDC->TB6612FNG_driver);
	}
}

void DC_DRIVER_set_duty(DC_DRIVER_t *pDC, const uint16_t duty)
{
	//	-ustawienie prędkości poprzez ustawienie wypełnienia sygnału PWM,

	if(pDC->DRV8833_driver != NULL)
	{
		//DRV8833_set_duty(pDC->DRV8833_driver, duty);
	}
	else if(pDC->TB6612FNG_driver != NULL)
	{
		TB6612FNG_set_duty(pDC->TB6612FNG_driver, duty);
	}
}

uint16_t DC_DRIVER_get_duty(DC_DRIVER_t *pDC)
{
	//	-ustawienie prędkości poprzez ustawienie wypełnienia sygnału PWM,

	if(pDC->DRV8833_driver != NULL)
	{
		return DRV8833_get_duty(pDC->DRV8833_driver);
	}
	else if(pDC->TB6612FNG_driver != NULL)
	{
		return TB6612FNG_get_duty(pDC->TB6612FNG_driver);
	}

	return 9;
}

void DC_DRIVER_set_direction(DC_DRIVER_t *pDC, const uint8_t direction)
{
	//	-ustawienie kierunku obrotów,
	//	-zabezpieczenie przed zmianą kierunku obrotów jeżeli wał obraca
	//	się w przeciwnym kierunku,

	//if( (pDC->direction != direction) )
	{
		//pDC->direction = direction;

		if(direction == CW_MOTOR)
		{
			DC_DRIVER_CW(pDC);
		}
		else if(direction == CCW_MOTOR)
		{
			DC_DRIVER_CCW(pDC);
		}
	}
}

DRIVER_STATE_e DC_DRIVER_get_driver_state(DC_DRIVER_t *pDC)
{
	//	-ustawienie informacji o aktywnym stanie pracy sterownika,

	if(pDC->DRV8833_driver != NULL)
	{
		return DRV8833_get_driver_state(pDC->DRV8833_driver);
	}
	else if(pDC->TB6612FNG_driver != NULL)
	{
		return TB6612FNG_get_driver_state(pDC->TB6612FNG_driver);
	}

	return 0;
}



