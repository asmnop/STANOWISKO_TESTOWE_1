/*
 * sys_tick.c
 *
 *  Created on: 13 mar 2025
 *      Author: asmnop
 */


#include "../../LIB/Inc/sys_tick.h"


SYS_TICK_t SYS_TICK =
{
	.counter = 0,
	.ms      = 0,
	.s       = 0,
	.min     = 0,
	.h       = 0,
	.d       = 0,
};

//	Zmienne do pomiaru czasu powtarzania znaku:
volatile uint16_t latch            = 0;
volatile uint16_t latch_value_now  = 0;
volatile uint16_t latch_value_prev = 0;
volatile uint16_t latch_value_diff = 0;

volatile uint16_t latch_DMA            = 0;
volatile uint16_t latch_value_now_DMA  = 0;
volatile uint16_t latch_value_prev_DMA = 0;
volatile uint16_t latch_value_diff_DMA = 0;

volatile uint16_t latch_DMA_END            = 0;
volatile uint16_t latch_value_now_DMA_END  = 0;
volatile uint16_t latch_value_prev_DMA_END = 0;
volatile uint16_t latch_value_diff_DMA_END = 0;


void SYS_TICK_set(void)
{
	SYS_TICK.counter++;

	SYS_TICK.ms++;
	if(SYS_TICK.ms == 1000)
	{
		SYS_TICK.ms = 0;
		SYS_TICK.s++;
		if(SYS_TICK.s == 60)
		{
			SYS_TICK.s = 0;
			SYS_TICK.min++;
			if(SYS_TICK.min == 60)
			{
				SYS_TICK.min = 0;
				SYS_TICK.h++;
				if(SYS_TICK.h == 24)
				{
					SYS_TICK.h = 0;
					SYS_TICK.d++;
				}
			}
		}
	}
}

void SYS_TICK_latch(void)
{
	if(latch == 1)
	{
		latch = 0;
		latch_value_now = (SYS_TICK.s * 1000) + SYS_TICK.ms;
		latch_value_diff = latch_value_now - latch_value_prev;
		latch_value_prev = latch_value_now;
	}

	if(latch_DMA == 1)
	{
		latch_DMA = 0;
		latch_value_now_DMA = (SYS_TICK.s * 1000) + SYS_TICK.ms;
		latch_value_diff_DMA = latch_value_now_DMA - latch_value_prev_DMA;
		latch_value_prev_DMA = latch_value_now_DMA;
	}

	if(latch_DMA_END == 1)
	{
		latch_DMA_END = 0;
		latch_value_now_DMA_END = (SYS_TICK.s * 1000) + SYS_TICK.ms;
		latch_value_diff_DMA_END = latch_value_now_DMA_END - latch_value_prev_DMA;
		latch_value_prev_DMA_END = latch_value_now_DMA_END;
	}
}

void SYS_TICK_terminal_info(TERMINAL_t *pTERMINAL, const uint8_t line, const uint8_t row_1, const uint8_t row_2)
{
	//	-informacja o czasie, który minął od ostatniego resetu lub włączenia zasilania,

	TERMINAL_line(pTERMINAL, line, row_1, "SYS_TICK  [ms] - ");
	TERMINAL_number(pTERMINAL, line, row_2, SYS_TICK.ms, 3);

	TERMINAL_line(pTERMINAL, line + 1, row_1, "SYS_TICK   [s] - ");
	TERMINAL_number(pTERMINAL, line + 1, row_2, SYS_TICK.s, 2);

	TERMINAL_line(pTERMINAL, line + 2, row_1, "SYS_TICK [min] - ");
	TERMINAL_number(pTERMINAL, line + 2, row_2, SYS_TICK.min, 2);

	TERMINAL_line(pTERMINAL, line + 3, row_1, "SYS_TICK   [h] - ");
	TERMINAL_number(pTERMINAL, line + 3, row_2, SYS_TICK.h, 2);
}

void SYS_TICK_terminal_info_time(TERMINAL_t *pTERMINAL, const uint8_t line, const uint8_t row_1, const uint8_t row_2)
{
	//	-informacja o czasie, który minął od ostatniego resetu lub włączenia zasilania,

	TERMINAL_line(pTERMINAL, line, row_1, "SYS_TICK [hh:mm:ss:msmsms] - ");

	//	GODZINY:
	if(SYS_TICK.h < 10)
	{
		TERMINAL_line(pTERMINAL, line, row_2, "0");
		TERMINAL_number(pTERMINAL, line, row_2 + 1, SYS_TICK.h, 1);
	}
	else
	{
		TERMINAL_number(pTERMINAL, line, row_2, SYS_TICK.h, 2);
	}
	TERMINAL_line(pTERMINAL, line, row_2 + 2, ":");

	//	MINUTY:
	if(SYS_TICK.min < 10)
	{
		TERMINAL_line(pTERMINAL, line, row_2 + 3, "0");
		TERMINAL_number(pTERMINAL, line, row_2 + 4, SYS_TICK.min, 1);
	}
	else
	{
		TERMINAL_number(pTERMINAL, line, row_2 + 3, SYS_TICK.min, 2);
	}
	TERMINAL_line(pTERMINAL, line, row_2 + 5, ":");

	//	SEKUNDY:
	if(SYS_TICK.s < 10)
	{
		TERMINAL_line(pTERMINAL, line, row_2 + 6, "0");
		TERMINAL_number(pTERMINAL, line, row_2 + 7, SYS_TICK.s, 1);
	}
	else
	{
		TERMINAL_number(pTERMINAL, line, row_2 + 6, SYS_TICK.s, 2);
	}
	TERMINAL_line(pTERMINAL, line, row_2 + 8, ":");

	//	MILISEKUNDY:
	TERMINAL_number(pTERMINAL, line, row_2 + 9, SYS_TICK.ms, 3);
}

void SYS_TICK_DMA_info_time(TERMINAL_t *pTERMINAL, const uint8_t line, const uint8_t row_1, const uint8_t row_2)
{
	//	-informacje o czasie trwania cyklu DMA,

	TERMINAL_line(pTERMINAL, line, row_1, "Okres ISR DMA USART [ms]:");
	TERMINAL_line(pTERMINAL, line, row_2, "     ");
	TERMINAL_number(pTERMINAL, line, row_2, latch_value_diff_DMA, 8);

	TERMINAL_line(pTERMINAL, line + 1, row_1, "Czas odswierzania [ms]:");
	TERMINAL_line(pTERMINAL, line + 1, row_2, "     ");
	TERMINAL_number(pTERMINAL, line + 1, row_2, latch_value_diff_DMA_END, 8);
}







