/*
 * dmamux.h
 *
 *  Created on: 22 paź 2024
 *      Author: asmnop
 */

#ifndef DMAMUX_H_
#define DMAMUX_H_


#include "universal_lib.h"
#include "../../LIB/Inc/pwm.h"



typedef struct
{
	DMA_TypeDef *DMAx;			//	Jednostka DMA,
	CHANNEL_t *pCHANNEL;		//	Używany jeśli DMA podłączamy pod timer,
	uint32_t channel;			//	Kanał, który obsługuje źródło,
	uint32_t periph_request;	//	Źródło, peryferium, z którego korzystamy,
	uint32_t direction;			//	Rodzaj transmisji,
	uint32_t priority;			//	Priorytet transmisji,
	uint32_t work_mode;			//	Tryb pracy,
	uint32_t periph_inc_mode;	//	Tryb obsługi zmiany adresu źródła,
	uint32_t memory_inc_mode;	//	Tryb obsługi zmiany adresu miejsca docelowego,
	uint32_t periph_size;		//	Typ komórki pamięci źródła,
	uint32_t memory_size;		//	Typ komórki pamięci miejsca docelowego,
	uint32_t enable_IT_TC;		//	Zezwolenie na obsługę przerwania od TC,
	uint32_t enable_IT_HT;		//	Zezwolenie na obsługę przerwania od HT,
	uint32_t enable_IT_TE;		//	Zezwolenie na obsługę przerwania od TE,
	uint32_t IT_priority;		//	Priorytet przerwania,
	volatile uint32_t number_of_data;	//	Ilość danych do transmisji,
	volatile uint32_t average[16];		//	Średni wynik dla danego kanału,
	volatile uint8_t  flag;
	volatile uint8_t  *ptr_8;			//	Wkaźnik na komórkę pamięci typu uint8_t,
	volatile uint16_t *ptr_16;
	volatile uint32_t *ptr_32;
}DMA_t;


typedef struct
{


}DMAMUX_t;

volatile extern uint8_t WS2812_arr[];
volatile extern uint8_t WS2812_arrr[];

void DMA_ADC1_init(DMA_t *pDMAx);
void DMA_USART2_TX_init(DMA_t *pDMAx);
void DMA_TIM15_CH1_init(DMA_t *pDMAx);
void DMA_TIMx_CHx_init(DMA_t *pDMAx);
void DMA_start(DMA_t *pDMAx);
void DMA_stop(DMA_t *pDMAx);


#endif /* DMAMUX_H_ */







