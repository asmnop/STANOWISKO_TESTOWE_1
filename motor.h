/*
 * motor.h
 *
 *  Created on: 31 paź 2024
 *      Author: asmnop
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#include "universal_lib.h"

#include "../../LIB/Inc/gpio.h"
#include "../../LIB/Inc/encoder.h"
#include "../../LIB/Inc/DC_driver.h"

#include "../../LIB/Inc/pid.h"





typedef struct
{
	DC_DRIVER_t *driver;		//	Sterownik silnika,
	ENCODER_t *encoder;			//	Enkoder obsługujący silnik, nie musi być podłączony,
	PID_t *pid;					//	Algorytm PID, nie musi być podłączony,
	uint8_t *driver_state;		//	Stan pracy sterownika: CW_MOTOR, CCW_MOTOR, STOP_MOTOR, BRAKE_MOTOR, STANDBY_MOTOR,
	volatile uint16_t set_point;//	Wartość zadana do uzyskania na wyjściu,
}MOTOR_t;


typedef enum
{
	STOP_,
	BRAKE_,
	CCW_,
	CW_,
	STANDBY_,
}MOTOR_STATE_e;



void MOTOR_short_brake(MOTOR_t *pMOTOR);
void MOTOR_stop(MOTOR_t *pMOTOR);

//void MOTOR_go_(MOTOR_t *pMOTOR, const uint16_t speed, const uint8_t direction);
void MOTOR_seq(MOTOR_t *pMOTOR, uint16_t arr[]);
uint8_t MOTOR_seq_slow(MOTOR_t *pMOTOR, uint16_t arr[]);

void MOTOR_rotations(MOTOR_t *pMOTOR, const uint8_t direction, const uint16_t speed, const uint32_t counts);

void MOTOR_ride(MOTOR_t *pMOTOR, const uint16_t speed, uint8_t driver_state);
void MOTOR_brake(MOTOR_t *pMOTOR, uint8_t brake_mode);


#endif /* MOTOR_H_ */




