/*
 * DRV8833.h
 *
 *  Created on: 7 lis 2024
 *      Author: asmnop
 */

#ifndef DRV8833_H_
#define DRV8833_H_


#include "universal_lib.h"
#include "../../LIB/Inc/gpio.h"
#include "../../LIB/Inc/pwm.h"


typedef struct
{
	LINE_t *input_1;		//	Linia sterująca numer 1,
	LINE_t *input_2;		//	Linia sterująca numer 2,
	LINE_t *standby;		//	Linia do obsługi stanu wstrzymania,
	CHANNEL_t *tim_mode_1;	//	Timer skonfigurowany do pracy jako PWM na kanał x,
	CHANNEL_t *tim_mode_2;	//	Timer skonfigurowany do pracy jako PWM na kanał y,
	uint8_t driver_state;	//	Stan pracy sterownika: CW_MOTOR, CCW_MOTOR, STOP_MOTOR, BRAKE_MOTOR, STANDBY_MOTOR,
	//uint16_t duty;			//	Wypełnienie sygnału PWM, wartość w zakresie: 0..( ( SYSCLK / preskaler ) / frequency )
}DRV8833_t;



typedef enum
{
	FORWARD_IDLE,
	FORWARD_BRAKE,
	REVERSE_IDLE,
	REVERSE_BRAKE,
	IDLE,
	BRAKE,
	STANDBY,
	GOING_FORWARD_IDLE,
	GOING_FORWARD_BRAKE,
	GOING_REVERSE_IDLE,
	GOING_REVERSE_BRAKE,
	GOING_IDLE,
	GOING_BRAKE,
	GOING_STANDBY,


}STATE_e;


extern const char* MOTOR_STATE[];


void DRV8833_init(const DRV8833_t *pDC);
void DRV8833_brake(DRV8833_t *pDC);
void DRV8833_idle(DRV8833_t *pDC);
void DRV8833_CW(DRV8833_t *pDC);
void DRV8833_CCW(DRV8833_t *pDC);
void DRV8833_standby(DRV8833_t *pDC);
void DRV8833_set_duty(DRV8833_t *pDC, const uint16_t duty_1, const uint16_t duty_2);
uint16_t DRV8833_get_duty(DRV8833_t *pDC);
void DRV8833_set_direction(DRV8833_t *pDC, const uint8_t direction);
uint8_t DRV8833_get_driver_state(DRV8833_t *pDC);
void DRV8833_set_driver_state(DRV8833_t *pDC, uint8_t driver_state);

void DRV8833_ride(DRV8833_t *pMOTOR, uint16_t speed, uint8_t direction);
void DRV8833_inc_dec(DRV8833_t *pMOTOR, int16_t inc);
uint8_t DRV8833_inc_dec_value(DRV8833_t *pMOTOR, uint8_t go);

#endif /* DRV8833_H_ */








