/*
 * DC_driver.h
 *
 *  Created on: 7 lis 2024
 *      Author: asmnop
 */

#ifndef DC_DRIVER_H_
#define DC_DRIVER_H_


#include "universal_lib.h"
#include "../../LIB/Inc/gpio.h"
#include "../../LIB/Inc/pwm.h"
#include "../../LIB/Inc/DRV8833.h"
#include "../../LIB/Inc/TB6612FNG.h"





typedef struct
{
	DRV8833_t *DRV8833_driver;
	TB6612FNG_t *TB6612FNG_driver;

}DC_DRIVER_t;

typedef enum
{
	STOP_MOTOR,
	BRAKE_MOTOR,
	CCW_MOTOR,
	CW_MOTOR,
	STANDBY_MOTOR,
}DRIVER_STATE_e;


void DC_DRIVER_init(const DC_DRIVER_t *pDC);
void DC_DRIVER_short_brake(DC_DRIVER_t *pDC);
void DC_DRIVER_stop(DC_DRIVER_t *pDC);
void DC_DRIVER_CW(DC_DRIVER_t *pDC);
void DC_DRIVER_CCW(DC_DRIVER_t *pDC);
void DC_DRIVER_standby(DC_DRIVER_t *pDC);
void DC_DRIVER_set_duty(DC_DRIVER_t *pDC, const uint16_t duty);
uint16_t DC_DRIVER_get_duty(DC_DRIVER_t *pDC);
void DC_DRIVER_set_direction(DC_DRIVER_t *pDC, const uint8_t direction);
DRIVER_STATE_e DC_DRIVER_get_driver_state(DC_DRIVER_t *pDC);


#endif /* DC_DRIVER_H_ */








