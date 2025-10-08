/*
 * software_timers.c
 *
 *  Created on: Apr 24, 2024
 *      Author: asmnop
 */

#include "software_timers.h"



volatile uint16_t timer_1 = 0;
volatile uint16_t timer_2 = 0;
volatile uint16_t timer_3 = 0;

volatile uint16_t timer_text = 0;	//	Timer do obsługi czasu wyświetlania na TM1637,
volatile uint16_t timer_key = 0;
volatile uint16_t timer_while = 0;
volatile uint16_t timer_buzzer = 0;



//	############################################################################
/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follows :
  *            System Clock source            = PLL (MSI)
  *            HSI Frequency(Hz)              = 16 000 000
  *
  *            PLL_M                          = 2
  *            PLL_N                          = 10
  *            PLL_R                          = 2
  *            SYSCLK(Hz)                     = 40 000 000
  *
  *            Flash Latency(WS)              = 1
  *
  *            AHB Prescaler                  = 1
  *            HCLK(Hz)                       = 40 000 000 - F_CPU
  *            APB1 Prescaler                 = 1
  *            PCLK1(Hz)                      = 40 000 000
  *            APB2 Prescaler                 = 1
  *            PCLK2(Hz)                      = 40 000 000
  * @param  None
  * @retval None
  */
void SetSysClk(void)
{
	//	Konfiguracja opóźnienia dostępu do pamięci FLASH, w zależności od SYSCLK:
	LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);

	//	Konfiguracja parametrów bloku PLL pod kątem użycia go jako źródła SYSCLK:
	//	Źródło: NONE, MSI, HSI, HSE,
	//	Dzielnik M: 1 do 16,
	//	Mnożnik N: 8 do 127,
	//	Dzielnik R: 2, 4, 6, 8,
	LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_2, 10, LL_RCC_PLLR_DIV_2);

	//	Uruchomienie bloku PLL i odczekanie aż się włączy:
	LL_RCC_PLL_Enable();
	while(LL_RCC_PLL_IsReady() != 1);

	//	Włączenie wyjścia PLLR:
	LL_RCC_PLL_EnableDomain_SYS();

	//	Ustawienie dzielnika dla szyn AHB, APB1, APB2:
	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);	//	RCC->CFGR - F_CPU
	LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);		//	RCC->CFGR
	LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);		//	RCC->CFGR

	//	Wybór źródła SYSCLK jako PLL i odczekanie aż się włączy:
	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);					//	RCC->CFGR
	while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL);	//	RCC->CFGR
}


//	############################################################################
void SysTick_Handler(void)
{
	SYS_TICK_set();

	if(timer_1)		timer_1--;
	if(timer_2)		timer_2--;
	if(timer_3)		timer_3--;

	if(timer_text)		timer_text--;
	if(timer_key)		timer_key--;
	if(timer_buzzer)	timer_buzzer--;

	SYS_TICK_latch();
}


/**
 * @brief Ustawienie bezpośrednio zmiennej SystemCoreClock
 *        -jest to tylko zmienna!!!, to nie jest wartość HCLK !!!
 *        -ta zmienna odnosi się jedynie do obliczenia wartości 'SysTick->LOAD'
 *        -jest to wartość, od której zlicza SysTick w dół,
 *        -po obliczeniu HCLK należy taką samą wartość wpisać jako argument tej funkcji,
 * @param value Wartość SystemCoreClock
 */
void SetSysCoreClock(const uint32_t value)
{
	LL_SetSystemCoreClock(value);
	//SystemCoreClockUpdate();
}

/**
 * @brief Konfiguracja timera systemowego - SysTick
 *        -na proces konfiguracji składają się: ustawienie timera, włączenie go oraz
 *        aktywacja przerwań,
 *        -licznik 24-bitowy, zliczanie w dół, przerwanie przy przejściu przez zero,
 * @param value Wartość 'SystemCoreClock' w Hz
 */
void SetSysTick(const uint32_t value)
{
	LL_Init1msTick(value);
	LL_SYSTICK_EnableIT();
}









