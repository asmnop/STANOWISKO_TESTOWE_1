/*
 * motor.c
 *
 *  Created on: 31 paź 2024
 *      Author: asmnop
 */

#include "..\..\LIB\Inc\motor.h"

#include "..\..\LIB\Inc\VT100.h"

#include <stdlib.h>

#include "software_timers.h"
//#include "..\..\LIB\Inc\universal_data.h"
#include "..\..\LIB\Inc\universal_functions.h"
#include "..\..\LIB\Inc\pwm.h"
#include "software_timers.h"






void MOTOR_seq(MOTOR_t *pMOTOR, uint16_t arr[])
{
	//	-ustawienie nowych parametrów działania silnika,
	//	-ustawienie kierunku obrotów i prędkości,
	//	-w przypadku zmiany kierunku obrotów, następuje zatrzymanie silnika a następnie
	//	po czym następują obroty w zadanym kierunku,

	if(*(pMOTOR->driver_state) != *(arr+1))
	{
		if(DC_DRIVER_get_duty(pMOTOR->driver) != 0)
		{
			DC_DRIVER_set_duty(pMOTOR->driver, 0);
			delay_ms(200);
		}

		*(pMOTOR->driver_state) = *(arr+1);

		if(*(arr+1) == CW_)
		{
			DC_DRIVER_CW(pMOTOR->driver);
		}
		else if(*(arr+1) == CCW_)
		{
			DC_DRIVER_CCW(pMOTOR->driver);
		}
	}

	DC_DRIVER_set_duty(pMOTOR->driver, *(arr+0));
	//delay_ms(*(arr+2));
	//timer_2 = *(arr+2);
}

uint8_t MOTOR_seq_slow(MOTOR_t *pMOTOR, uint16_t arr[])
{
	//	-ustawienie nowych parametrów działania silnika,
	//	-ustawienie kierunku obrotów i prędkości,
	//	-w przypadku zmiany kierunku obrotów, następuje zatrzymanie silnika a następnie
	//	po czym następują obroty w zadanym kierunku,

	//	Jeżeli są różne kierunki obrotów:
	if(*(pMOTOR->driver_state) != *(arr+1))
	{
		if(DC_DRIVER_get_duty(pMOTOR->driver) != 0 && timer_3 == 0)	//	Zmniejszamy prędkość do zera,
		{
			DC_DRIVER_set_duty(pMOTOR->driver, DC_DRIVER_get_duty(pMOTOR->driver) - 1);
			timer_3 = 10;
			return 1;
		}

		//	Jeżeli prędkość spadnie do zera to zmieniamy kierunek obrotów:
		if(DC_DRIVER_get_duty(pMOTOR->driver) == 0 && timer_3 == 0)
		{
			*(pMOTOR->driver_state) = *(arr+1);

			if(*(arr+1) == CW_)
			{
				DC_DRIVER_CW(pMOTOR->driver);
			}
			else if(*(arr+1) == CCW_)
			{
				DC_DRIVER_CCW(pMOTOR->driver);
			}
		}
	}

	//	Zmieniamy obroty do zadanej wartości:
	if(DC_DRIVER_get_duty(pMOTOR->driver) != *(arr+0) && timer_3 == 0)
	{
		if(DC_DRIVER_get_duty(pMOTOR->driver) < *(arr+0))
		{
			DC_DRIVER_set_duty(pMOTOR->driver, DC_DRIVER_get_duty(pMOTOR->driver) + 1);
		}
		if(DC_DRIVER_get_duty(pMOTOR->driver) > *(arr+0))
		{
			DC_DRIVER_set_duty(pMOTOR->driver, DC_DRIVER_get_duty(pMOTOR->driver) - 1);
		}

		timer_3 = 10;

		return 2;
	}

	return 0;
}

void MOTOR_acceleration_time(MOTOR_t *pMOTOR, uint16_t arr[])
{
	//	-funkcja sterująca silnikiem DC,
	//	-ustawiamy w niej kierunek obrotów, prędkość obrotową oraz czas trwania dochodzenia
	//	do zadanej prędkości,
	//	-czas trwania dochodzenia do zadanej prędkości musi być mniejszy od czasu trwania prędkości,
}

void MOTOR_rotations(MOTOR_t *pMOTOR, const uint8_t direction, const uint16_t speed, const uint32_t counts)
{
	//	-wykonanie przez silnik zadanej ilości obrotów w zadanym kierunku,

	if(*(pMOTOR->driver_state) != direction)
	{
		if(DC_DRIVER_get_duty(pMOTOR->driver) != 0)
		{
			MOTOR_stop(pMOTOR);
			delay_ms(200);
		}
	}

	ENCO_set_ENCODER_parameters(pMOTOR->encoder, direction, counts);
	DC_DRIVER_set_direction(pMOTOR->driver, direction);
	DC_DRIVER_set_duty(pMOTOR->driver, speed);
}

void MOTOR_number_of_rotations_(MOTOR_t *pMOTOR, const uint16_t rotations, const uint8_t direction, const uint16_t speed)
{
	//	-wykonanie przez silnik zadanej ilości obrotów w zadanym kierunku,

	if(*(pMOTOR->driver_state) != direction)
	{
		if(DC_DRIVER_get_duty(pMOTOR->driver) != 0)
		{
			DC_DRIVER_set_duty(pMOTOR->driver, 0);

		}

		delay_ms(200);

		*(pMOTOR->driver_state) = direction;

		if(direction == CW_MOTOR)
		{
			DC_DRIVER_CW(pMOTOR->driver);
		}
		else if(direction == CCW_MOTOR)
		{
			DC_DRIVER_CCW(pMOTOR->driver);
		}
	}

	uint8_t start_steps = ENCO_get_steps(pMOTOR->encoder);
	pMOTOR->encoder->UPDATE_counter = 0;
	uint8_t full = 0;

	DC_DRIVER_set_duty(pMOTOR->driver, speed);

	do
	{
		full = pMOTOR->encoder->UPDATE_counter;
		if(full < 0)
		{
			full = full * (-1);
		}
	}while(full != rotations);


	do
	{

	}while(start_steps > ENCO_get_steps(pMOTOR->encoder));



	DC_DRIVER_set_duty(pMOTOR->driver, 0);
}


//////////////////////////////////////////////////////////////////////////////////////////
void MOTOR_short_brake(MOTOR_t *pMOTOR)
{
	//	-gwałtowne hamowanie, pedał hamulca wciśnięty do końca,

	DC_DRIVER_short_brake(pMOTOR->driver);
}

void MOTOR_stop(MOTOR_t *pMOTOR)
{
	//	-hamowanie swobodne, wrzucenie na luz i czekanie na zatrzymanie,

	DC_DRIVER_stop(pMOTOR->driver);
}

void MOTOR_brake(MOTOR_t *pMOTOR, uint8_t brake_mode)
{
	//	-sterowanie zatrzymaniem obrotów silnika,

	if(DC_DRIVER_get_driver_state(pMOTOR->driver) != brake_mode)
	{
		if(brake_mode == STOP_)
		{
			DC_DRIVER_stop(pMOTOR->driver);
		}
		else if(brake_mode == BRAKE_)
		{
			DC_DRIVER_short_brake(pMOTOR->driver);
		}
	}
}

void MOTOR_ride(MOTOR_t *pMOTOR, const uint16_t speed, uint8_t driver_state)
{
	//	-ustawienie nowych parametrów działania silnika,
	//	-ustawienie kierunku obrotów i prędkości,
	//	-w przypadku zmiany kierunku obrotów, następuje zatrzymanie silnika a następnie,
	//	po czym następują obroty w zadanym kierunku,

	//VT100_show_data("*(pMOTOR->driver_state: ", *(pMOTOR->driver_state));
	//VT100_show_data("driver_state: ", driver_state);

	if(DC_DRIVER_get_driver_state(pMOTOR->driver) != driver_state)	//	Jeżeli ma nastąpić zmiana kierunku obrotów,
	{
		if(DC_DRIVER_get_duty(pMOTOR->driver) != 0)	//	Oraz prędkość obecna jest różna od zera,
		{
			//DC_DRIVER_stop(pMOTOR->driver);
			MOTOR_brake(pMOTOR, STOP_MOTOR);
			delay_ms(50);
		}

		//DC_DRIVER_set_direction(pMOTOR->driver, direction);
		DC_DRIVER_set_direction(pMOTOR->driver, driver_state);
	}

	DC_DRIVER_set_duty(pMOTOR->driver, speed);
}










