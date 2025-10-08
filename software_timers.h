/*
 * software_timers.h
 *
 *  Created on: Apr 24, 2024
 *      Author: asmnop
 */


#ifndef INC_SOFTWARE_TIMERS_H_
#define INC_SOFTWARE_TIMERS_H_


#include "universal_lib.h"
#include "../../LIB/Inc/sys_tick.h"


void SetSysClk(void);
void SetSysCoreClock(const uint32_t value);
void SetSysTick(const uint32_t value);


volatile extern uint16_t timer_1;
volatile extern uint16_t timer_2;
volatile extern uint16_t timer_3;

volatile extern uint16_t timer_text;
volatile extern uint16_t timer_key;
volatile extern uint16_t timer_while;
volatile extern uint16_t timer_buzzer;


#endif /* INC_SOFTWARE_TIMERS_H_ */




