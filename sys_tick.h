/*
 * sys_tick.h
 *
 *  Created on: 13 mar 2025
 *      Author: asmnop
 */

#ifndef SYS_TICK_H_
#define SYS_TICK_H_


#include "universal_lib.h"
#include "../../LIB/Inc/terminal.h"


typedef struct
{
	volatile uint64_t counter;
	volatile uint16_t ms;
	volatile uint8_t s;
	volatile uint8_t min;
	volatile uint8_t h;
	volatile uint16_t d;
}SYS_TICK_t;

extern SYS_TICK_t SYS_TICK;

void SYS_TICK_set(void);
void SYS_TICK_latch(void);
void SYS_TICK_terminal_info(TERMINAL_t *pTERMINAL, const uint8_t line, const uint8_t row_1, const uint8_t row_2);
void SYS_TICK_terminal_info_time(TERMINAL_t *pTERMINAL, const uint8_t line, const uint8_t row_1, const uint8_t row_2);
void SYS_TICK_DMA_info_time(TERMINAL_t *pTERMINAL, const uint8_t line, const uint8_t row_1, const uint8_t row_2);

volatile extern uint16_t latch;
volatile extern uint16_t latch_value_now;
volatile extern uint16_t latch_value_prev;
volatile extern uint16_t latch_value_diff;

volatile extern uint16_t latch_DMA;
volatile extern uint16_t latch_value_now_DMA;
volatile extern uint16_t latch_value_prev_DMA;
volatile extern uint16_t latch_value_diff_DMA;

volatile extern uint16_t latch_DMA_END;
volatile extern uint16_t latch_value_now_DMA_END;
volatile extern uint16_t latch_value_prev_DMA_END;
volatile extern uint16_t latch_value_diff_DMA_END;


#endif /* SYS_TICK_H_ */











