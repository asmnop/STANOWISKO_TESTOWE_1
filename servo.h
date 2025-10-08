/*
 * servo.h
 *
 *  Created on: 25 lut 2025
 *      Author: asmnop
 */

#ifndef SERVO_H_
#define SERVO_H_


#include "universal_lib.h"
#include "../../LIB/Inc/gpio.h"
#include "../../LIB/Inc/pwm.h"


typedef struct
{
	CHANNEL_t *signal_line;		//	Linia sterująca wypełnieniem PWM,
	const uint16_t min_value;	//	Wartość minimalna wypełnienia,
	const uint16_t max_value;	//	Wartość maksymalna wypełnienia,
	const uint16_t total_size;	//	Liczba pozycji, w których może się zatrzymać serwo,
	volatile uint16_t data[];	//	Tablica na wszystkie położenia serwa,
}SERVO_t;



void SERVO_demo(SERVO_t *pSERVO);
void SERVO_init(SERVO_t *pSERVO);
void SERVO_set_duty(SERVO_t *pSERVO, const uint16_t value);
void SERVO_set_min(SERVO_t *pSERVO);
void SERVO_set_max(SERVO_t *pSERVO);
void SERVO_set_center(SERVO_t *pSERVO);
void SERVO_inc(SERVO_t *pSERVO);
void SERVO_dec(SERVO_t *pSERVO);
void SERVO_set_angle(SERVO_t *pSERVO, uint16_t angle);

uint16_t SERVO_get_global_position(SERVO_t *pSERVO);
uint16_t SERVO_get_angle_global_position(SERVO_t *pSERVO);

#endif /* SERVO_H_ */












