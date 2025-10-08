/*
 * dma.h
 *
 *  Created on: 22 paź 2024
 *      Author: asmnop
 */

#ifndef DMA_H_
#define DMA_H_


#include "universal_lib.h"
//#include "..\..\LIB\Inc\usart.h"



typedef struct
{
	DMA_TypeDef *DMAx;			//	Jednostka DMA,
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
	volatile uint32_t number_of_data;	//	Ilość danych do transmisji,
	volatile uint32_t average[16];		//	Średni wynik dla danego kanału,
	volatile uint8_t flag;
	volatile uint8_t *ptr_8;			//	Wkaźnik na komórkę pamięci typu uint8_t,
	volatile uint16_t *ptr_16;
	volatile uint32_t *ptr_32;
}DMA_t_;


//void DMA_ADC1_init(DMA_tt *pDMAx);
//void DMA_USART2_TX_init(DMA_tt *pDMAx);
//void DMA_TIM15_CH1_init(DMA_tt *pDMAx);
//void DMA_start(DMA_tt *pDMAx);
//void DMA_stop(DMA_tt *pDMAx);


#endif /* DMA_H_ */







