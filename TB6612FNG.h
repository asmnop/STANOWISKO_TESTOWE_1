/*
 * TB6612FNG.h
 *
 *  Created on: 7 lis 2024
 *      Author: asmnop
 */

#ifndef TB6612FNG_H_
#define TB6612FNG_H_


#include "universal_lib.h"
#include "../../LIB/Inc/gpio.h"
#include "../../LIB/Inc/pwm.h"


typedef struct
{
	LINE_t *input_1;		//	Linia sterująca numer 1,
	LINE_t *input_2;		//	Linia sterująca numer 2,
	LINE_t *standby;		//	Linia do obsługi stanu wstrzymania,
	LINE_t *pwm;			//	Linia do obsługi sygnału PWM,
	CHANNEL_t *tim_mode;	//	Timer skonfigurowany do pracy jako PWM,
	uint8_t driver_state;	//	Stan pracy sterownika: CW_MOTOR, CCW_MOTOR, STOP_MOTOR, BRAKE_MOTOR, STANDBY_MOTOR,
	uint16_t duty;			//	Wypełnienie sygnału PWM, wartość w zakresie: 0..( ( SYSCLK / preskaler ) / frequency )
}TB6612FNG_t;

typedef enum
{
	STOP_TB6612FNG,
	BRAKE_TB6612FNG,
	CCW_TB6612FNG,
	CW_TB6612FNG,
	STANDBY_TB6612FNG,
}TB6612FNG_STATE_e;


void TB6612FNG_init(const TB6612FNG_t *pDC);
void TB6612FNG_short_brake(TB6612FNG_t *pDC);
void TB6612FNG_stop(TB6612FNG_t *pDC);
void TB6612FNG_CW(TB6612FNG_t *pDC);
void TB6612FNG_CCW(TB6612FNG_t *pDC);
void TB6612FNG_standby(TB6612FNG_t *pDC);
void TB6612FNG_set_duty(TB6612FNG_t *pDC, const uint16_t duty);
uint16_t TB6612FNG_get_duty(TB6612FNG_t *pDC);
void TB6612FNG_set_direction(TB6612FNG_t *pDC, const uint8_t direction);
TB6612FNG_STATE_e TB6612FNG_get_driver_state(TB6612FNG_t *pDC);


#endif /* TB6612FNG_H_ */








