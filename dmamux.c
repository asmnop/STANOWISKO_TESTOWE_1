/*
 * dmamux.c
 *
 *  Created on: 22 paź 2024
 *      Author: asmnop
 */


#include "../../LIB/Inc/dmamux.h"

	volatile uint8_t WS2812_arr[20] = {
			5, 20, 10, 22, 10, 10, 10, 10, 10, 20,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	};

	volatile uint8_t WS2812_arrr[20] = {
			15, 10, 10, 10, 10, 10, 10, 10, 10, 10,
			10, 10, 10, 10, 10, 10, 10, 10, 10, 0,
	};
	//	 DUBLET powstaje przy 15 !!!!!!!

//	////////////////////////////////////////////////////////////////////////////
static void DMA_enable_clock(DMA_t *pDMAx)
{
	//	-podłączenie zegara do peryferium:

	if(pDMAx->DMAx == DMA1)
	{
		LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
	}
	else if(pDMAx->DMAx == DMA2)
	{
		LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2);
	}
}

static void DMA_enable_NVIC(DMA_t *pDMAx)
{
	//	Włączenie przerwań od DMA:

	if(pDMAx->DMAx == DMA1)
	{
		if(pDMAx->channel == LL_DMA_CHANNEL_1)
		{
			NVIC_SetPriority(DMA1_Channel1_IRQn, pDMAx->IT_priority);
			NVIC_EnableIRQ(DMA1_Channel1_IRQn);
		}
		else if(pDMAx->channel == LL_DMA_CHANNEL_2)
		{
			NVIC_SetPriority(DMA1_Channel2_IRQn, pDMAx->IT_priority);
			NVIC_EnableIRQ(DMA1_Channel2_IRQn);
		}
		else if(pDMAx->channel == LL_DMA_CHANNEL_3)
		{
			NVIC_SetPriority(DMA1_Channel3_IRQn, pDMAx->IT_priority);
			NVIC_EnableIRQ(DMA1_Channel3_IRQn);
		}
		else if(pDMAx->channel == LL_DMA_CHANNEL_4)
		{
			NVIC_SetPriority(DMA1_Channel4_IRQn, pDMAx->IT_priority);
			NVIC_EnableIRQ(DMA1_Channel4_IRQn);
		}
		else if(pDMAx->channel == LL_DMA_CHANNEL_5)
		{
			NVIC_SetPriority(DMA1_Channel5_IRQn, pDMAx->IT_priority);
			NVIC_EnableIRQ(DMA1_Channel5_IRQn);
		}
		else if(pDMAx->channel == LL_DMA_CHANNEL_6)
		{
			NVIC_SetPriority(DMA1_Channel6_IRQn, pDMAx->IT_priority);
			NVIC_EnableIRQ(DMA1_Channel6_IRQn);
		}
		else if(pDMAx->channel == LL_DMA_CHANNEL_7)
		{
			NVIC_SetPriority(DMA1_Channel7_IRQn, pDMAx->IT_priority);
			NVIC_EnableIRQ(DMA1_Channel7_IRQn);
		}
	}
	else if(pDMAx->DMAx == DMA2)
	{
		if(pDMAx->channel == LL_DMA_CHANNEL_1)
		{
			NVIC_SetPriority(DMA2_Channel1_IRQn, pDMAx->IT_priority);
			NVIC_EnableIRQ(DMA2_Channel1_IRQn);
		}
		else if(pDMAx->channel == LL_DMA_CHANNEL_2)
		{
			NVIC_SetPriority(DMA2_Channel2_IRQn, pDMAx->IT_priority);
			NVIC_EnableIRQ(DMA2_Channel2_IRQn);
		}
		else if(pDMAx->channel == LL_DMA_CHANNEL_3)
		{
			NVIC_SetPriority(DMA2_Channel3_IRQn, pDMAx->IT_priority);
			NVIC_EnableIRQ(DMA2_Channel3_IRQn);
		}
		else if(pDMAx->channel == LL_DMA_CHANNEL_4)
		{
			NVIC_SetPriority(DMA2_Channel4_IRQn, pDMAx->IT_priority);
			NVIC_EnableIRQ(DMA2_Channel4_IRQn);
		}
		else if(pDMAx->channel == LL_DMA_CHANNEL_5)
		{
			NVIC_SetPriority(DMA2_Channel5_IRQn, pDMAx->IT_priority);
			NVIC_EnableIRQ(DMA2_Channel5_IRQn);
		}
		else if(pDMAx->channel == LL_DMA_CHANNEL_6)
		{
			NVIC_SetPriority(DMA2_Channel6_IRQn, pDMAx->IT_priority);
			NVIC_EnableIRQ(DMA2_Channel6_IRQn);
		}
		else if(pDMAx->channel == LL_DMA_CHANNEL_7)
		{
			NVIC_SetPriority(DMA2_Channel7_IRQn, pDMAx->IT_priority);
			NVIC_EnableIRQ(DMA2_Channel7_IRQn);
		}
	}
}

static void DMA_enable_local_IT(DMA_t *pDMAx)
{
	if(pDMAx->enable_IT_TC == 1)
	{
		LL_DMA_EnableIT_TC(pDMAx->DMAx, pDMAx->channel);
	}

	if(pDMAx->enable_IT_HT == 1)
	{
		LL_DMA_EnableIT_HT(pDMAx->DMAx, pDMAx->channel);
	}

	if(pDMAx->enable_IT_TE == 1)
	{
		LL_DMA_EnableIT_TE(pDMAx->DMAx, pDMAx->channel);
	}
}

static void DMAMUX_enable_clock(void)
{
	//	-podłączenie zegara do peryferium,
#if defined (STM32G4)
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMAMUX1);
#endif
}

static void DMA_enable_request(DMA_t *pDMAx)
{

	switch(pDMAx->pCHANNEL->channel)
	{
		case LL_TIM_CHANNEL_CH1:
		{
			LL_TIM_EnableDMAReq_CC1(pDMAx->pCHANNEL->timer->TIMx);
			break;
		}
		case LL_TIM_CHANNEL_CH2:
		{
			LL_TIM_EnableDMAReq_CC2(pDMAx->pCHANNEL->timer->TIMx);
			break;
		}
		case LL_TIM_CHANNEL_CH3:
		{
			LL_TIM_EnableDMAReq_CC3(pDMAx->pCHANNEL->timer->TIMx);
			break;
		}
		case LL_TIM_CHANNEL_CH4:
		{
			LL_TIM_EnableDMAReq_CC4(pDMAx->pCHANNEL->timer->TIMx);
			break;
		}
	}
}

//	////////////////////////////////////////////////////////////////////////////
void DMA_ADC1_init(DMA_t *pDMAx)
{
	//	-konfiguracja i włączenie trybu DMA dla ADC,

	//	Nadanie wartosci poczatkowych:


	for(uint8_t i=0; i<16; i++)
	{
		pDMAx->average[i] = 0;
	}

	ADC_TypeDef *ADCx = NULL;

#ifdef STM32G4
	if(pDMAx->periph_request == LL_DMAMUX_REQ_ADC1)
	{
		ADCx = ADC1;
	}
	else if(pDMAx->periph_request == LL_DMAMUX_REQ_ADC2)
	{
		ADCx = ADC2;
	}
	else if(pDMAx->periph_request == LL_DMAMUX_REQ_ADC3)
	{
		ADCx = ADC3;
	}
#endif
	//	Podłączenie zegara do peryferium:
	//DMA_enable_clock(pDMAx);
	DMA_enable_clock(pDMAx);
	DMAMUX_enable_clock();

	//	Konfiguracja rejestrów po stronie ADC. Włączenie transferu z trybem cyklicznym, circular,
	LL_ADC_REG_SetDMATransfer(ADCx, LL_ADC_REG_DMA_TRANSFER_UNLIMITED);

	//	Dopasowanie kanału ze źródłem (peryferium), z których będziemy korzystać: 0, 1, 2, 3, 4, 5, 6, 7:
	LL_DMA_SetPeriphRequest(pDMAx->DMAx, pDMAx->channel, pDMAx->periph_request);

	//	Ustawienie rodzaju transmisji dla kanału: PERIPH_TO_MEMORY, MEMORY_TO_PERIPH, MEMORY_TO_MEMORY:
	LL_DMA_SetDataTransferDirection(pDMAx->DMAx, pDMAx->channel, pDMAx->direction);

	//	Ustawienie priorytetu transmisji: LOW, MEDIUM, HIGH, VERYHIGH:
	LL_DMA_SetChannelPriorityLevel(pDMAx->DMAx, pDMAx->channel, pDMAx->priority);

	//	Ustawienie trybu pracy: NORMAL, CIRCULAR:
	LL_DMA_SetMode(pDMAx->DMAx, pDMAx->channel, pDMAx->work_mode);

	//	Czy adres źródła ma być zwiększany czy bez zmian: INCREMENT, NOINCREMENT:
	LL_DMA_SetPeriphIncMode(pDMAx->DMAx, pDMAx->channel, pDMAx->periph_inc_mode);

	//	Czy adres miejsca końcowego ma być zwiększany czy bez zmian: INCREMENT, NOINCREMENT:
	LL_DMA_SetMemoryIncMode(pDMAx->DMAx, pDMAx->channel, pDMAx->memory_inc_mode);

	//	Wielkość typu pobieranego ze źródła: BYTE, HALFWORD, WORD:
	LL_DMA_SetPeriphSize(pDMAx->DMAx, pDMAx->channel, pDMAx->periph_size);

	//	Rodzaj typu miejsca docelowego: BYTE, HALFWORD, WORD:
	LL_DMA_SetMemorySize(pDMAx->DMAx, pDMAx->channel, pDMAx->memory_size);

	//	Zezwolenie na obsługę przerwań od DMAx:
	DMA_enable_NVIC(pDMAx);

	//	Włączenie przerwań lokalnych:
	DMA_enable_local_IT(pDMAx);

	//	Funkcja ustawiająca adresy pamięci celu i źródła oraz rodzaj trybu transmisji:
	LL_DMA_ConfigAddresses(pDMAx->DMAx, pDMAx->channel,
	LL_ADC_DMA_GetRegAddr(ADCx, LL_ADC_DMA_REG_REGULAR_DATA),		//	Źródło
	(uint32_t)(pDMAx->ptr_32),										//	Cel,
	LL_DMA_GetDataTransferDirection(pDMAx->DMAx, pDMAx->channel));	//	Kierunek,

	//	Ilość danych do transferu:
	LL_DMA_SetDataLength(pDMAx->DMAx, pDMAx->channel, pDMAx->number_of_data);

	//	Aktywacja kanału,
	LL_DMA_EnableChannel(pDMAx->DMAx, pDMAx->channel);
}

void DMA_USART2_TX_init(DMA_t *pDMAx)
{
	//	-konfiguracja i włączenie trybu DMA dla USART2,

	//	Podłączenie zegara do peryferium:
	DMA_enable_clock(pDMAx);
	DMAMUX_enable_clock();

	//	Dopasowanie kanału ze źródłem (peryferium), z których będziemy korzystać: 0, 1, 2, 3, 4, 5, 6, 7:
	LL_DMA_SetPeriphRequest(pDMAx->DMAx, pDMAx->channel, pDMAx->periph_request);

	//	Ustawienie rodzaju transmisji dla kanału: PERIPH_TO_MEMORY, MEMORY_TO_PERIPH, MEMORY_TO_MEMORY:
	LL_DMA_SetDataTransferDirection(pDMAx->DMAx, pDMAx->channel, pDMAx->direction);

	//	Ustawienie priorytetu transmisji: LOW, MEDIUM, HIGH, VERYHIGH:
	LL_DMA_SetChannelPriorityLevel(pDMAx->DMAx, pDMAx->channel, pDMAx->priority);

	//	Ustawienie trybu pracy: NORMAL, CIRCULAR:
	LL_DMA_SetMode(pDMAx->DMAx, pDMAx->channel, pDMAx->work_mode);

	//	Czy adres w peryferium ma być zwiększany czy bez zmian: INCREMENT, NOINCREMENT:
	LL_DMA_SetPeriphIncMode(pDMAx->DMAx, pDMAx->channel, pDMAx->periph_inc_mode);

	//	Czy adres w pamięci ma być zwiększany czy bez zmian: INCREMENT, NOINCREMENT:
	LL_DMA_SetMemoryIncMode(pDMAx->DMAx, pDMAx->channel, pDMAx->memory_inc_mode);

	//	Wielkość typu w peryferium: BYTE, HALFWORD, WORD:
	LL_DMA_SetPeriphSize(pDMAx->DMAx, pDMAx->channel, pDMAx->periph_size);

	//	Wielkość typu w pamięci: BYTE, HALFWORD, WORD:
	LL_DMA_SetMemorySize(pDMAx->DMAx, pDMAx->channel, pDMAx->memory_size);

	//	Zezwolenie na obsługę przerwań od DMAx:
	DMA_enable_NVIC(pDMAx);

	//	Włączenie przerwań lokalnych:
	DMA_enable_local_IT(pDMAx);

	//	Funkcja ustawiająca adresy początkowe źródła i celu oraz rodzaj trybu transmisji:
	LL_DMA_ConfigAddresses(pDMAx->DMAx, pDMAx->channel,
	(uint32_t)(pDMAx->ptr_8),											//	Źródło
	LL_USART_DMA_GetRegAddr(USART2, LL_USART_DMA_REG_DATA_TRANSMIT),	//	Cel,
	LL_DMA_GetDataTransferDirection(pDMAx->DMAx, pDMAx->channel));		//	Kierunek,

	//	Ilość danych do transferu:
	LL_DMA_SetDataLength(pDMAx->DMAx, pDMAx->channel, pDMAx->number_of_data);

	//	Włączenie trybu transmisji po DMA:
	LL_USART_EnableDMAReq_TX(USART2);
}

void DMA_TIM15_CH1_init(DMA_t *pDMAx)
{
	//	-konfiguracja i włączenie trybu DMA dla TIM15_CH1,

	//	Podłączenie zegara do peryferium:
	DMA_enable_clock(pDMAx);
	DMAMUX_enable_clock();

	//	Dopasowanie kanału ze źródłem (peryferium), z których będziemy korzystać: 0, 1, 2, 3, 4, 5, 6, 7:
	LL_DMA_SetPeriphRequest(pDMAx->DMAx, pDMAx->channel, pDMAx->periph_request);

	//	Ustawienie rodzaju transmisji dla kanału: PERIPH_TO_MEMORY, MEMORY_TO_PERIPH, MEMORY_TO_MEMORY:
	LL_DMA_SetDataTransferDirection(pDMAx->DMAx, pDMAx->channel, pDMAx->direction);

	//	Ustawienie priorytetu transmisji: LOW, MEDIUM, HIGH, VERYHIGH:
	LL_DMA_SetChannelPriorityLevel(pDMAx->DMAx, pDMAx->channel, pDMAx->priority);

	//	Ustawienie trybu pracy: NORMAL, CIRCULAR:
	LL_DMA_SetMode(pDMAx->DMAx, pDMAx->channel, pDMAx->work_mode);

	//	Czy adres w peryferium ma być zwiększany czy bez zmian: INCREMENT, NOINCREMENT:
	LL_DMA_SetPeriphIncMode(pDMAx->DMAx, pDMAx->channel, pDMAx->periph_inc_mode);

	//	Czy adres w pamięci ma być zwiększany czy bez zmian: INCREMENT, NOINCREMENT:
	LL_DMA_SetMemoryIncMode(pDMAx->DMAx, pDMAx->channel, pDMAx->memory_inc_mode);

	//	Wielkość typu w peryferium: BYTE, HALFWORD, WORD:
	LL_DMA_SetPeriphSize(pDMAx->DMAx, pDMAx->channel, pDMAx->periph_size);

	//	Wielkość typu w pamięci: BYTE, HALFWORD, WORD:
	LL_DMA_SetMemorySize(pDMAx->DMAx, pDMAx->channel, pDMAx->memory_size);

	//	Zezwolenie na obsługę przerwań od DMAx:
	DMA_enable_NVIC(pDMAx);

	//	Włączenie przerwań lokalnych:
	DMA_enable_local_IT(pDMAx);

	//	Funkcja ustawiająca adresy początkowe źródła i celu oraz rodzaj trybu transmisji:
	LL_DMA_ConfigAddresses(pDMAx->DMAx, pDMAx->channel,
	(uint32_t)(pDMAx->ptr_16),											//	Źródło
	(uint32_t)(&TIM15->CCR1),											//	Cel,
	LL_DMA_GetDataTransferDirection(pDMAx->DMAx, pDMAx->channel));		//	Kierunek,

	//	Ilość danych do transferu:
	LL_DMA_SetDataLength(pDMAx->DMAx, pDMAx->channel, pDMAx->number_of_data);

	//	Włączenie trybu transmisji po DMA:
	LL_TIM_EnableDMAReq_CC1(TIM15);
}

void DMA_TIMx_CHx_init(DMA_t *pDMAx)
{
	//	-konfiguracja trybu DMA dla TIMx_CHx,
	//	-na wyjściu z funkcji przesył danych przez DMA nie jest jeszcze aktywowany!!!,

	//	Podłączenie zegara do peryferium:
	DMA_enable_clock(pDMAx);
	DMAMUX_enable_clock();

	//	Dopasowanie kanału ze źródłem (peryferium), z których będziemy korzystać: 0, 1, 2, 3, 4, 5, 6, 7:
	LL_DMA_SetPeriphRequest(pDMAx->DMAx, pDMAx->channel, pDMAx->periph_request);

	//	Ustawienie rodzaju transmisji dla kanału: PERIPH_TO_MEMORY, MEMORY_TO_PERIPH, MEMORY_TO_MEMORY:
	LL_DMA_SetDataTransferDirection(pDMAx->DMAx, pDMAx->channel, pDMAx->direction);

	//	Ustawienie priorytetu transmisji: LOW, MEDIUM, HIGH, VERYHIGH:
	LL_DMA_SetChannelPriorityLevel(pDMAx->DMAx, pDMAx->channel, pDMAx->priority);

	//	Ustawienie trybu pracy: NORMAL, CIRCULAR:
	LL_DMA_SetMode(pDMAx->DMAx, pDMAx->channel, pDMAx->work_mode);

	//	Czy adres w peryferium ma być zwiększany czy bez zmian: INCREMENT, NOINCREMENT:
	LL_DMA_SetPeriphIncMode(pDMAx->DMAx, pDMAx->channel, pDMAx->periph_inc_mode);

	//	Czy adres w pamięci ma być zwiększany czy bez zmian: INCREMENT, NOINCREMENT:
	LL_DMA_SetMemoryIncMode(pDMAx->DMAx, pDMAx->channel, pDMAx->memory_inc_mode);

	//	Wielkość typu w peryferium: BYTE, HALFWORD, WORD:
	LL_DMA_SetPeriphSize(pDMAx->DMAx, pDMAx->channel, pDMAx->periph_size);

	//	Wielkość typu w pamięci: BYTE, HALFWORD, WORD:
	LL_DMA_SetMemorySize(pDMAx->DMAx, pDMAx->channel, pDMAx->memory_size);

	//	Zezwolenie na obsługę przerwań od DMAx:
	DMA_enable_NVIC(pDMAx);

	//	Włączenie przerwań lokalnych:
	DMA_enable_local_IT(pDMAx);

	//	Funkcja ustawiająca adresy początkowe źródła i celu oraz rodzaj trybu transmisji:
	LL_DMA_ConfigAddresses(pDMAx->DMAx, pDMAx->channel,
	(uint32_t)(pDMAx->ptr_8),											//	Źródło,
	(uint32_t)(&pDMAx->pCHANNEL->timer->TIMx->CCR1),					//	Cel,
	LL_DMA_GetDataTransferDirection(pDMAx->DMAx, pDMAx->channel));		//	Kierunek,

	//	Ilość danych do transferu:
	LL_DMA_SetDataLength(pDMAx->DMAx, pDMAx->channel, pDMAx->number_of_data);

	//	Włączenie żądania aktywacji transmisji po DMA za pomocą zgodnego porównania:
	DMA_enable_request(pDMAx);
}

void DMA_start(DMA_t *pDMAx)
{
	//	Aktywacja kanału:

	LL_DMA_EnableChannel(pDMAx->DMAx, pDMAx->channel);
}

void DMA_stop(DMA_t *pDMAx)
{
	LL_DMA_DisableIT_TC(pDMAx->DMAx, pDMAx->channel);
	LL_DMA_DisableIT_HT(pDMAx->DMAx, pDMAx->channel);
	LL_DMA_DisableIT_TE(pDMAx->DMAx, pDMAx->channel);

	LL_DMA_ClearFlag_GI1(pDMAx->DMAx);
	LL_DMA_ClearFlag_TC1(pDMAx->DMAx);
	LL_DMA_ClearFlag_HT1(pDMAx->DMAx);
	LL_DMA_ClearFlag_TE1(pDMAx->DMAx);

	//LL_ADC_REG_SetDMATransfer(ADC1, LL_ADC_REG_DMA_TRANSFER_NONE);
	//LL_ADC_REG_StopConversion(ADC1);

	LL_DMA_DisableChannel(pDMAx->DMAx, pDMAx->channel);
}

void DMA_disable_clock(DMA_t *pDMAx)
{
	//	Deaktywacja podłączenia zegara do instancji DMAx:

	if(pDMAx->DMAx == DMA1)
	{
		LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_DMA1);
	}
	else if(pDMAx->DMAx == DMA2)
	{
		LL_AHB1_GRP1_DisableClock(LL_AHB1_GRP1_PERIPH_DMA2);
	}
}



//	////////////////////////////////////////////////////////////////////////////
uint32_t read_ADC_data[10];

void DMA_first(void)
{
	//	-konfiguracja i włączenie trybu DMA dla ADC,

	//uint32_t read_ADC_data[10];

	//	Aktywacja podłączenia zegara do trybu DMA1:
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

	//	Konfiguracja rejestrów po stronie ADC. Włączenie transferu z trybem cyklicznym, circular,
	LL_ADC_REG_SetDMATransfer(ADC1, LL_ADC_REG_DMA_TRANSFER_UNLIMITED);

	//	Dopasowanie kanału ze źródłem (peryferium), z których będziemy korzystać: 0, 1, 2, 3, 4, 5, 6, 7:
	//LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_1, LL_DMA_REQUEST_0);

	//	Ustawienie rodzaju transmisji dla kanału: PERIPH_TO_MEMORY, MEMORY_TO_PERIPH, MEMORY_TO_MEMORY:
	LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_1, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

	//	Ustawienie priorytetu transmisji: LOW, MEDIUM, HIGH, VERYHIGH:
	LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PRIORITY_HIGH);

	//	Ustawienie trybu pracy: NORMAL, CIRCULAR:
	LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MODE_CIRCULAR);

	//	Czy adres źródła ma być zwiększany czy bez zmian: INCREMENT, NOINCREMENT:
	LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PERIPH_NOINCREMENT);

	//	Czy adres miejsca końcowego ma być zwiększany czy bez zmian: INCREMENT, NOINCREMENT:
	LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MEMORY_INCREMENT);

	//	Wielkość typu pobieranego ze źródła: BYTE, HALFWORD, WORD:
	LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_PDATAALIGN_HALFWORD);

	//	Rodzaj typu miejsca docelowego: BYTE, HALFWORD, WORD:
	LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_1, LL_DMA_MDATAALIGN_WORD);

	//	Włączenie przerwań od DMA:
	NVIC_SetPriority(DMA1_Channel1_IRQn, 0);
	NVIC_EnableIRQ(DMA1_Channel1_IRQn);
	LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_1);
	//LL_DMA_EnableIT_HT(DMA1, LL_DMA_CHANNEL_1);
	//LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_1);

	//	Funkcja ustawiająca adresy pamięci celu i źródła oraz rodzaj trybu transmisji:
	LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_1, LL_ADC_DMA_GetRegAddr(ADC1, LL_ADC_DMA_REG_REGULAR_DATA), (uint32_t)&read_ADC_data, LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_1));

	//	Ilość danych do transferu:
	LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_1, 2);

	//	Aktywacja kanału,
	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
}




