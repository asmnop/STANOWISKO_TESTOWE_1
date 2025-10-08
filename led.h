/*
 * led.h
 *
 *  Created on: 13 lis 2024
 *      Author: mm
 */

#ifndef LED_H_
#define LED_H_


#include "universal_lib.h"
#include "..\..\LIB\Inc\gpio.h"
#include "..\..\LIB\Inc\pwm.h"


typedef struct
{
	LINE_t *pwm;			//	Linia do obsługi sygnału PWM,
	CHANNEL_t *tim_mode;	//	Timer skonfigurowany do pracy jako PWM,
	uint16_t *duty;			//	Wypełnienie sygnału PWM, wartość w zakresie: 0..( ( SYSCLK / preskaler ) / frequency )
}LED_DRIVER_t;


void LED_LINE_init(CHANNEL_t *pLED);
void LED_LINE_init_all(CHANNEL_t **pLED_DRIVER, const uint8_t length);

void LED_LINE_set_duty(CHANNEL_t *pPWM, const uint32_t duty);
void LED_LINE_set_duty_all(CHANNEL_t **pLED_DRIVER, uint32_t *data, const uint8_t length);

void LED_LINE_enable_IT(CHANNEL_t *pPWM);
void LED_LINE_enable_IT_all(CHANNEL_t **pPWM, const uint8_t length);

void LED_LINE_disable_IT(CHANNEL_t *pPWM);
void LED_LINE_disable_IT_all(CHANNEL_t **pPWM, const uint8_t length);

void LED_LINE_set_mode(CHANNEL_t *pPWM, const uint32_t work_mode);
uint32_t LED_LINE_get_mode(CHANNEL_t *pPWM);
void LED_LINE_set_mode_all(CHANNEL_t **pPWM, uint32_t *work_mode, const uint8_t length);

void LED_LINE_set(CHANNEL_t *pPWM);
void LED_LINE_reset(CHANNEL_t *pPWM);
void LED_LINE_toggle(CHANNEL_t *pPWM);
void LED_LINE_state(CHANNEL_t *pPWM, const uint8_t state);
void LED_LINE_TERMINAL_driver(CHANNEL_t **pPWM, const uint8_t data_read);

void LED_LINE_return_to_PWM(CHANNEL_t *pPWM);

uint32_t LED_LINE_get_duty(CHANNEL_t *pPWM);

#endif /* LED_H_ */









