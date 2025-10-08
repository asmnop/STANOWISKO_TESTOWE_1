/*
 * pwm.c
 *
 *  Created on: 2 wrz 2024
 *      Author: mm
 */



#include "..\..\LIB\Inc\pwm.h"
#include "..\..\LIB\Inc\VT100.h"



//#define SYS_CORE_CLOCK	10000000	//	Parametr do SysTick,


void LED_LINE_disableIT(void)
{
#ifdef STM32G4
	TIM_TypeDef *pTIM = TIM3;

	LL_TIM_DisableIT_CC1(pTIM);
	LL_TIM_DisableIT_CC2(pTIM);
	LL_TIM_DisableIT_CC3(pTIM);
	LL_TIM_DisableIT_CC4(pTIM);
	LL_TIM_OC_SetCompareCH1(pTIM, 0);
	LL_TIM_OC_SetCompareCH2(pTIM, 0);
	LL_TIM_OC_SetCompareCH3(pTIM, 0);
	LL_TIM_OC_SetCompareCH4(pTIM, 0);
	LL_TIM_CC_DisableChannel(pTIM, LL_TIM_CHANNEL_CH1);
	LL_TIM_CC_DisableChannel(pTIM, LL_TIM_CHANNEL_CH2);
	LL_TIM_CC_DisableChannel(pTIM, LL_TIM_CHANNEL_CH3);
	LL_TIM_CC_DisableChannel(pTIM, LL_TIM_CHANNEL_CH4);

	pTIM = TIM2;
	LL_TIM_DisableIT_CC1(pTIM);
	LL_TIM_DisableIT_CC2(pTIM);
	LL_TIM_DisableIT_CC3(pTIM);
	LL_TIM_DisableIT_CC4(pTIM);
	LL_TIM_OC_SetCompareCH1(pTIM, 0);
	LL_TIM_OC_SetCompareCH2(pTIM, 0);
	LL_TIM_OC_SetCompareCH3(pTIM, 0);
	LL_TIM_OC_SetCompareCH4(pTIM, 0);
	LL_TIM_CC_DisableChannel(pTIM, LL_TIM_CHANNEL_CH1);
	LL_TIM_CC_DisableChannel(pTIM, LL_TIM_CHANNEL_CH2);
	LL_TIM_CC_DisableChannel(pTIM, LL_TIM_CHANNEL_CH3);
	LL_TIM_CC_DisableChannel(pTIM, LL_TIM_CHANNEL_CH4);
#endif
}

void TIM_OutputCompareMode(TIM_TypeDef *TIMx, uint32_t Channel, uint16_t duty)
{
	//	Generowanie sygnału na wyjściu TIMx_CHy,
	//	Licznik CNT oparty na timerze TIMx zlicza od wartości 0 do ARR,
	//	W międzyczasie wartość CNT zrównuje się z wartością rejestru CCR,
	//	W wyniku zgodnego porównania następuje zmiana stanu na kanale TIMx_CHy
	//	Kanał jest podłączony do pinu P__ na którym generowany jest stan,
	//	W funkcji występuje obsługa przerwania od zgodnego porównania,
	//	Można ustawić czy dla CNT<CCR na wyjściu panuje stan wysoki lub niski,
	//	Generowany jest sygnał prostokątny o stałej częstotliwości i wypełnieniu 50%,
	//	Zmieniając wartość rejestru CCR generujemy "przesunięcie" jakie następuje w
	//	zmianach stanu,

	//	Funkcja służy do sterowania przebiegiem wyjściowym lub wskazywania kiedy upłynął
	//	określony okres czasu.
	//	Kiedy nastąpi dopasowanie pomiędzy CCR a CNT to:
	//	-Przypisuje odpowiedni pin wyjściowy do programowalnej wartości zdefiniowanej
	//	przez tryb porównania wyjścia (bity OCxM w rejestrze TIMx_CCMRx) i polaryzację
	//	wyjściową (bit CCxP w rejestrze TIMx_CCER). Pin wyjściowy może zachować swój
	//	poziom (OCXM=000), być ustawiony jako aktywny (OCxM=001), być ustawiony jako
	//	nieaktywny (OCxM=010) lub może przełączać się (OCxM=011) po dopasowaniu.
	//	-ustawiana jest flaga zajścia przerwania (bit CCxIF bit w rejestrze TIMx_SR).
	//	---flaga ustawiana sprzętowo, zdejmowana przez zapis '0' lub poprzez odczyt CCR



	//	1. Select the counter clock (internal, external, prescaler).
	//TIM_TimerInit(TIMx);

	//	3. Set the CCxIE and/or CCxDE bits if an interrupt and/or a DMA request is to be generated.

	//	4. Select the output mode. For example, one must write OCxM=011, OCxPE=0, CCxP=0
	//	and CCxE=1 to toggle OCx output pin when CNT matches CCRx, CCRx preload is not
	//	used, OCx is enabled and active high.
	//	WYBÓR TRYBU PRACY:
	//LL_TIM_OC_ConfigOutput(TIM3, LL_TIM_CHANNEL_CH1, Configuration);
	LL_TIM_OC_SetMode(TIMx, Channel, LL_TIM_OCMODE_TOGGLE);
	LL_TIM_OC_SetPolarity(TIMx, Channel, LL_TIM_OCPOLARITY_HIGH);
	LL_TIM_OC_EnablePreload(TIMx, Channel);

	//	2. Write the desired data in the TIMx_ARR and TIMx_CCRx registers.
	if(Channel == LL_TIM_CHANNEL_CH1)	LL_TIM_OC_SetCompareCH1(TIMx, duty);
	if(Channel == LL_TIM_CHANNEL_CH2)	LL_TIM_OC_SetCompareCH2(TIMx, duty);
	if(Channel == LL_TIM_CHANNEL_CH3)	LL_TIM_OC_SetCompareCH3(TIMx, duty);
	if(Channel == LL_TIM_CHANNEL_CH4)	LL_TIM_OC_SetCompareCH4(TIMx, duty);

	//	Włączenie kanału, na który podawany jest sygnał z?????
	//LL_TIM_CC_EnableChannel(TIMx, Channel);
}

void LED_LINE_TOGGLE_init(void)
{
	TIM_basic_init_parameters(TIM2, LL_TIM_COUNTERMODE_UP, 10, 1000);
	TIM_basic_init_parameters(TIM2, LL_TIM_COUNTERMODE_UP, 10, 1000);

	TIM_OutputCompareMode(TIM2, LL_TIM_CHANNEL_CH1, 0);
	TIM_OutputCompareMode(TIM2, LL_TIM_CHANNEL_CH2, 62);
	TIM_OutputCompareMode(TIM2, LL_TIM_CHANNEL_CH3, 124);
	TIM_OutputCompareMode(TIM2, LL_TIM_CHANNEL_CH4, 186);

	TIM_OutputCompareMode(TIM2, LL_TIM_CHANNEL_CH1, 248);
	TIM_OutputCompareMode(TIM2, LL_TIM_CHANNEL_CH2, 310);
	TIM_OutputCompareMode(TIM2, LL_TIM_CHANNEL_CH3, 372);
	TIM_OutputCompareMode(TIM2, LL_TIM_CHANNEL_CH4, 434);
}

//	////////////////////////////////////////////////////////////////////////////
void TIM_enable_clock(TIM_TypeDef *TIMx)
{
	#if defined (STM32G4)
		if(TIMx == TIM2)		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
		else if(TIMx == TIM3)	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);
		else if(TIMx == TIM4)	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM4);
		else if(TIMx == TIM6)	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM6);
		else if(TIMx == TIM7)	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM7);

		else if(TIMx == TIM1)	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);
		else if(TIMx == TIM8)	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM8);
		else if(TIMx == TIM15)	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM15);
		else if(TIMx == TIM16)	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM16);
		else if(TIMx == TIM17)	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM17);
		else if(TIMx == TIM20)	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM20);
	#endif

#if defined (STM32L4)
	if(TIMx == TIM2)		LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
	else if(TIMx == TIM6)	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM6);
	else if(TIMx == TIM7)	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM7);

	else if(TIMx == TIM1)	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);
	else if(TIMx == TIM15)	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM15);
	else if(TIMx == TIM16)	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM16);
#endif
}

void TIM_freeze_clock(TIM_TypeDef *TIMx)
{
	#if defined (STM32G4)
		if(TIMx == TIM2)		LL_DBGMCU_APB1_GRP1_FreezePeriph(LL_DBGMCU_APB1_GRP1_TIM2_STOP);
		else if(TIMx == TIM3)	LL_DBGMCU_APB1_GRP1_FreezePeriph(LL_DBGMCU_APB1_GRP1_TIM3_STOP);
		else if(TIMx == TIM4)	LL_DBGMCU_APB1_GRP1_FreezePeriph(LL_DBGMCU_APB1_GRP1_TIM4_STOP);
		else if(TIMx == TIM6)	LL_DBGMCU_APB1_GRP1_FreezePeriph(LL_DBGMCU_APB1_GRP1_TIM6_STOP);
		else if(TIMx == TIM7)	LL_DBGMCU_APB1_GRP1_FreezePeriph(LL_DBGMCU_APB1_GRP1_TIM7_STOP);

		else if(TIMx == TIM1)	LL_DBGMCU_APB2_GRP1_FreezePeriph(LL_DBGMCU_APB2_GRP1_TIM1_STOP);
		else if(TIMx == TIM8)	LL_DBGMCU_APB2_GRP1_FreezePeriph(LL_DBGMCU_APB2_GRP1_TIM8_STOP);
		else if(TIMx == TIM15)	LL_DBGMCU_APB2_GRP1_FreezePeriph(LL_DBGMCU_APB2_GRP1_TIM15_STOP);
		else if(TIMx == TIM16)	LL_DBGMCU_APB2_GRP1_FreezePeriph(LL_DBGMCU_APB2_GRP1_TIM16_STOP);
		else if(TIMx == TIM17)	LL_DBGMCU_APB2_GRP1_FreezePeriph(LL_DBGMCU_APB2_GRP1_TIM17_STOP);
		else if(TIMx == TIM20)	LL_DBGMCU_APB2_GRP1_FreezePeriph(LL_DBGMCU_APB2_GRP1_TIM20_STOP);
	#endif
}


void TIM_enable_channels(void)
{
#ifdef STM32G4
	TIM_TypeDef *pTIM = TIM3;
	LL_TIM_CC_EnableChannel(pTIM, LL_TIM_CHANNEL_CH1);
	LL_TIM_CC_EnableChannel(pTIM, LL_TIM_CHANNEL_CH2);
	LL_TIM_CC_EnableChannel(pTIM, LL_TIM_CHANNEL_CH3);
	LL_TIM_CC_EnableChannel(pTIM, LL_TIM_CHANNEL_CH4);

	pTIM = TIM2;
	LL_TIM_CC_EnableChannel(pTIM, LL_TIM_CHANNEL_CH1);
	LL_TIM_CC_EnableChannel(pTIM, LL_TIM_CHANNEL_CH2);
	LL_TIM_CC_EnableChannel(pTIM, LL_TIM_CHANNEL_CH3);
	LL_TIM_CC_EnableChannel(pTIM, LL_TIM_CHANNEL_CH4);

	pTIM = TIM16;
	//LL_TIM_CC_EnableChannel(pTIM, LL_TIM_CHANNEL_CH1);
#endif
}

void TIM_disable_channels(void)
{
#ifdef STM32G4
	TIM_TypeDef *pTIM = TIM3;
	LL_TIM_CC_DisableChannel(pTIM, LL_TIM_CHANNEL_CH1);
	LL_TIM_CC_DisableChannel(pTIM, LL_TIM_CHANNEL_CH2);
	LL_TIM_CC_DisableChannel(pTIM, LL_TIM_CHANNEL_CH3);
	LL_TIM_CC_DisableChannel(pTIM, LL_TIM_CHANNEL_CH4);

	pTIM = TIM2;
	LL_TIM_CC_DisableChannel(pTIM, LL_TIM_CHANNEL_CH1);
	LL_TIM_CC_DisableChannel(pTIM, LL_TIM_CHANNEL_CH2);
	LL_TIM_CC_DisableChannel(pTIM, LL_TIM_CHANNEL_CH3);
	LL_TIM_CC_DisableChannel(pTIM, LL_TIM_CHANNEL_CH4);
#endif
}

void TIM_set_compare(uint16_t *data)
{
#ifdef STM32G4
	TIM_TypeDef *pTIM = TIM3;
	LL_TIM_OC_SetCompareCH1(pTIM, data[0]);
	LL_TIM_OC_SetCompareCH2(pTIM, data[1]);
	LL_TIM_OC_SetCompareCH3(pTIM, data[2]);
	LL_TIM_OC_SetCompareCH4(pTIM, data[3]);

	pTIM = TIM2;
	LL_TIM_OC_SetCompareCH1(pTIM, data[4]);
	LL_TIM_OC_SetCompareCH2(pTIM, data[5]);
	LL_TIM_OC_SetCompareCH3(pTIM, data[6]);
	LL_TIM_OC_SetCompareCH4(pTIM, data[7]);
#endif
}

void TIM_set_output_compare_mode(TIM_TypeDef *TIMx, uint32_t Channel, uint32_t Mode)
{
	//	Generowanie sygnału na wyjściu TIMx_CHy,
	//	Licznik CNT oparty na timerze TIMx zlicza od wartości 0 do ARR,
	//	W międzyczasie wartość CNT zrównuje się z wartością rejestru CCR,
	//	W wyniku zgodnego porównania następuje zmiana stanu na kanale TIMx_CHy
	//	Kanał jest podłączony do pinu P__ na którym generowany jest stan,
	//	W funkcji występuje obsługa przerwania od zgodnego porównania,
	//	Można ustawić czy dla CNT<CCR na wyjściu panuje stan wysoki lub niski,
	//	Generowany jest sygnał prostokątny o stałej częstotliwości i wypełnieniu 50%,
	//	Zmieniając wartość rejestru CCR generujemy "przesunięcie" jakie następuje w
	//	zmianach stanu,

	//	Funkcja służy do sterowania przebiegiem wyjściowym lub wskazywania kiedy upłynął
	//	określony okres czasu.
	//	Kiedy nastąpi dopasowanie pomiędzy CCR a CNT to:
	//	-Przypisuje odpowiedni pin wyjściowy do programowalnej wartości zdefiniowanej
	//	przez tryb porównania wyjścia (bity OCxM w rejestrze TIMx_CCMRx) i polaryzację
	//	wyjściową (bit CCxP w rejestrze TIMx_CCER). Pin wyjściowy może zachować swój
	//	poziom (OCXM=000), być ustawiony jako aktywny (OCxM=001), być ustawiony jako
	//	nieaktywny (OCxM=010) lub może przełączać się (OCxM=011) po dopasowaniu.
	//	-ustawiana jest flaga zajścia przerwania (bit CCxIF bit w rejestrze TIMx_SR).
	//	---flaga ustawiana sprzętowo, zdejmowana przez zapis '0' lub poprzez odczyt CCR

	//	1. Select the counter clock (internal, external, prescaler).
	//TIM_TimerInit(TIMx);

	//	3. Set the CCxIE and/or CCxDE bits if an interrupt and/or a DMA request is to be generated.

	//	4. Select the output mode. For example, one must write OCxM=011, OCxPE=0, CCxP=0
	//	and CCxE=1 to toggle OCx output pin when CNT matches CCRx, CCRx preload is not
	//	used, OCx is enabled and active high.
	//	WYBÓR TRYBU PRACY:
	//LL_TIM_OC_ConfigOutput(TIM3, LL_TIM_CHANNEL_CH1, Configuration);

	LL_TIM_OC_SetMode(TIMx, Channel, Mode);
	LL_TIM_OC_SetPolarity(TIMx, Channel, LL_TIM_OCPOLARITY_HIGH);
	LL_TIM_OC_EnablePreload(TIMx, Channel);
}

void TIM_set_output_compare_mode_all(uint32_t Mode)
{
#ifdef STM32G4
	TIM_TypeDef *pTIM = TIM3;
	TIM_set_output_compare_mode(pTIM, LL_TIM_CHANNEL_CH1, Mode);
	TIM_set_output_compare_mode(pTIM, LL_TIM_CHANNEL_CH2, Mode);
	TIM_set_output_compare_mode(pTIM, LL_TIM_CHANNEL_CH3, Mode);
	TIM_set_output_compare_mode(pTIM, LL_TIM_CHANNEL_CH4, Mode);

	pTIM = TIM2;
	TIM_set_output_compare_mode(pTIM, LL_TIM_CHANNEL_CH1, Mode);
	TIM_set_output_compare_mode(pTIM, LL_TIM_CHANNEL_CH2, Mode);
	TIM_set_output_compare_mode(pTIM, LL_TIM_CHANNEL_CH3, Mode);
	TIM_set_output_compare_mode(pTIM, LL_TIM_CHANNEL_CH4, Mode);
#endif
}

void TIM_force_level(uint8_t *level)
{
	//	-wymuszenie stanu na OCxREF w trybie Output Compare,
	//	-do wykonania tego należy:
	//	***zezwolić na podpięcie zegara z APB1 o znanej częstotliwości do timera,
	//	***zezwolić na podpięcie kanału,
	//	-ustawić pożądany stan na danym kanale,

	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);		//	RCC->APB1ENR1
#ifdef STM32G4
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);		//	RCC->APB1ENR1
#endif

	TIM_enable_channels();
#ifdef STM32G4
	if(level[0] == 0)
		LL_TIM_OC_SetMode(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_FORCED_INACTIVE);
	else
		LL_TIM_OC_SetMode(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_FORCED_ACTIVE);

	if(level[1] == 0)
		LL_TIM_OC_SetMode(TIM3, LL_TIM_CHANNEL_CH2, LL_TIM_OCMODE_FORCED_INACTIVE);
	else
		LL_TIM_OC_SetMode(TIM3, LL_TIM_CHANNEL_CH2, LL_TIM_OCMODE_FORCED_ACTIVE);

	if(level[2] == 0)
		LL_TIM_OC_SetMode(TIM3, LL_TIM_CHANNEL_CH3, LL_TIM_OCMODE_FORCED_INACTIVE);
	else
		LL_TIM_OC_SetMode(TIM3, LL_TIM_CHANNEL_CH3, LL_TIM_OCMODE_FORCED_ACTIVE);

	if(level[3] == 0)
		LL_TIM_OC_SetMode(TIM3, LL_TIM_CHANNEL_CH4, LL_TIM_OCMODE_FORCED_INACTIVE);
	else
		LL_TIM_OC_SetMode(TIM3, LL_TIM_CHANNEL_CH4, LL_TIM_OCMODE_FORCED_ACTIVE);
#endif
	if(level[4] == 0)
		LL_TIM_OC_SetMode(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_FORCED_INACTIVE);
	else
		LL_TIM_OC_SetMode(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_FORCED_ACTIVE);

	if(level[5] == 0)
		LL_TIM_OC_SetMode(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_OCMODE_FORCED_INACTIVE);
	else
		LL_TIM_OC_SetMode(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_OCMODE_FORCED_ACTIVE);

	if(level[6] == 0)
		LL_TIM_OC_SetMode(TIM2, LL_TIM_CHANNEL_CH3, LL_TIM_OCMODE_FORCED_INACTIVE);
	else
		LL_TIM_OC_SetMode(TIM2, LL_TIM_CHANNEL_CH3, LL_TIM_OCMODE_FORCED_ACTIVE);

	if(level[7] == 0)
		LL_TIM_OC_SetMode(TIM2, LL_TIM_CHANNEL_CH4, LL_TIM_OCMODE_FORCED_INACTIVE);
	else
		LL_TIM_OC_SetMode(TIM2, LL_TIM_CHANNEL_CH4, LL_TIM_OCMODE_FORCED_ACTIVE);
}


void TIM_basic_timer(void)
{
	TIM_basic_init_parameters(TIM6, LL_TIM_COUNTERMODE_UP, 1, 10);
}


//	////////////////////////////////////////////////////////////////////////////
void TIM_basic_init(TIM_t *pTIMx)
{
	//	-bazowe ustawienie timera,
	//	-wykorzystywane tylko dla TIM6 o TIM7,
	//	-należy uważać na to jaką wartość wpisujemy do makra '__LL_TIM_CALC_ARR' jako argument '__PSC__',
	//	-jako '__PSC__' należy wpisać wartość preskalera wpisywaną do rejestru czyli obniżoną o jeden!!!,

	//#define PRESKALER ((SYS_CORE_CLOCK / pMOTOR->number_of_steps) / pMOTOR->frequency)

	//VT100_show_data("SYS CORE CLOCK [Hz]: ", SYS_CORE_CLOCK / 1000000);
	//VT100_show_data("pTIMx->number_of_steps: ", pMOTOR->number_of_steps);
	//VT100_show_data("pTIMx->frequency: ", pMOTOR->frequency);
	//VT100_show_data("PRESKALER: ", PRESKALER);

	//	Zezwolenie na podpięcie zegara z APB1/APB2 o znanej częstotliwości do timera:
	if(pTIMx->TIMx == TIM2)			LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
#ifdef STM32G4
	else if(pTIMx->TIMx == TIM3)	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);
#endif
	else if(pTIMx->TIMx == TIM6)	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM6);
	else if(pTIMx->TIMx == TIM7)	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM7);

	else if(pTIMx->TIMx == TIM1)	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);
	else if(pTIMx->TIMx == TIM15)	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM15);
	else if(pTIMx->TIMx == TIM16)	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM16);
#ifdef STM32G4
	else if(pTIMx->TIMx == TIM17)	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM17);
#endif
	/*
	//	Zezwolenie na podpięcie zegara z APB1 o znanej częstotliwości do timera:
	if(pTIMx->TIMx == TIM6 || pTIMx->TIMx == TIM7)
	{
		LL_APB1_GRP1_EnableClock(pTIMx->periphs);		//	RCC->APB1ENR1
	}


	if( (pTIMx->periphs == LL_APB1_GRP1_PERIPH_TIM2) || \
		(pTIMx->periphs == LL_APB1_GRP1_PERIPH_TIM3) || \
		(pTIMx->periphs == LL_APB1_GRP1_PERIPH_TIM6) || \
		(pTIMx->periphs == LL_APB1_GRP1_PERIPH_TIM7) )
	{
		LL_APB1_GRP1_EnableClock(pTIMx->periphs);	//	RCC->APB1ENR1
	}

	if(	(pTIMx->periphs == LL_APB2_GRP1_PERIPH_TIM15) || \
		(pTIMx->periphs == LL_APB2_GRP1_PERIPH_TIM16) || \
		(pTIMx->periphs == LL_APB2_GRP1_PERIPH_TIM17) )
	{
		LL_APB2_GRP1_EnableClock(pTIMx->periphs);	//	RCC->APB2ENR1
	}
*/
	//	Ustawienie czy zawartość ARR ma być uaktualniana natychmiast czy przy UPDATE:
	//	Aktualizacja natychmiast niesie za sobą możliwość wpisania wartości mniejszej
	//	od wartości w liczniku, wtedy licznik zliczy do wartości 16-bitowej
	LL_TIM_EnableARRPreload(pTIMx->TIMx);

	//	Ustawienie typu źródła zegara:
	LL_TIM_SetClockSource(pTIMx->TIMx, pTIMx->clock_source);	//	TIM_SMCR_SMS | TIM_SMCR_ECE

	//	Kierunek zliczania:
	LL_TIM_SetCounterMode(pTIMx->TIMx, pTIMx->counter_mode);	//	TIM_CR1_DIR | TIM_CR1_CMS

	TIM_set_basic_parameters(pTIMx, 0, pTIMx->frequency);
}

void TIM_basic_init_parameters(TIM_TypeDef *TIMx, uint32_t counter_mode, uint32_t preskaler, uint32_t frequency)
{
	//	-bazowe ustawienie timera,
	//	-wykorzystywane tylko dla TIM6 o TIM7,
	//	-należy uważać na to jaką wartość wpisujemy do makra '__LL_TIM_CALC_ARR' jako argument '__PSC__',
	//	-jako '__PSC__' należy wpisać wartość preskalera wpisywaną do rejestru czyli obniżoną o jeden!!!,

	TIM_t TIMER_TMP =
	{
		.TIMx = TIMx,									//	Wybór jednostki używanego timera,
		.clock_source = LL_TIM_CLOCKSOURCE_INTERNAL, 	//	Wybór źródła zegara: INTERNAL, EXT_MODE1, EXT_MODE2,
		.counter_mode = counter_mode,					//	Kierunek zliczania licznika: UP, DOWN, CENTER_UP, CENTER_DOWN, CENTER_UP_DOWN,
		//.preskaler = preskaler,							//	Preskaler częstotliwości SYSCLK,
		.frequency = frequency,							//	Częstotliwość generowania przerwania w [Hz],
	};

	TIM_basic_init(&TIMER_TMP);
}

void TIM_mode_init(CHANNEL_t *pTIM)
{
	//	PWM zapewnia wygenerowanie sygnału o częstotliwości zapisanej w ARR i
	//	współczynniku wypełnienia o wartości CCR,
	//	PWM może być skonfigurowany niezależnie na każdym kanale (jedno PWM na
	//	każde OCx - output compare) poprzez zapis PWM 1 = 110 lub PWM 2 = 111
	//	w bicie OCxM rejestru CCMRx,
	//	Korespondujący preload register musi zostać aktywowany za pomocą OCxPE
	//	w rejestrze CCMRx
	//	Ewentualnie automatyczny reload preload register w bicie ARPE w CR1
	//	(w zliczaniu w górę lub centrowany do środka)
	//	Jako preload register jest przepisywany do rejestru cieni tylko wtedy kiedy
	//	zachodzi zdarzenie uaktualnienia, przed włączeniem licznika, wszystkie
	//	rejestry muszą być zainicjalizowane poprzez zapis bitu UG w EGR


	//	Wybór trybu pracy:
	LL_TIM_OC_SetMode(pTIM->timer->TIMx, pTIM->channel, pTIM->work_mode);

	LL_TIM_OC_SetPolarity(pTIM->timer->TIMx, pTIM->channel, LL_TIM_OCPOLARITY_HIGH);

	LL_TIM_OC_EnablePreload(pTIM->timer->TIMx, pTIM->channel);

	//	2. Write the desired data in the TIMx_ARR and TIMx_CCRx registers.
	if(pTIM->channel == LL_TIM_CHANNEL_CH1)	LL_TIM_OC_SetCompareCH1(pTIM->timer->TIMx, pTIM->duty);
	if(pTIM->channel == LL_TIM_CHANNEL_CH2)	LL_TIM_OC_SetCompareCH2(pTIM->timer->TIMx, pTIM->duty);
	if(pTIM->channel == LL_TIM_CHANNEL_CH3)	LL_TIM_OC_SetCompareCH3(pTIM->timer->TIMx, pTIM->duty);
	if(pTIM->channel == LL_TIM_CHANNEL_CH4)	LL_TIM_OC_SetCompareCH4(pTIM->timer->TIMx, pTIM->duty);

	//	Włączenie kanału, na który podawany jest sygnał z?????
	//LL_TIM_CC_EnableChannel(TIMx, Channel);

	LL_TIM_GenerateEvent_UPDATE(pTIM->timer->TIMx);
	LL_TIM_ClearFlag_UPDATE(pTIM->timer->TIMx);
}

void TIM_mode_init_parameters(TIM_TypeDef *TIMx, uint32_t work_mode, uint32_t channel, uint16_t duty)
{
	//	PWM zapewnia wygenerowanie sygnału o częstotliwości zapisanej w ARR i
	//	współczynniku wypełnienia o wartości CCR,
	//	PWM może być skonfigurowany niezależnie na każdym kanale (jedno PWM na
	//	każde OCx - output compare) poprzez zapis PWM 1 = 110 lub PWM 2 = 111
	//	w bicie OCxM rejestru CCMRx,
	//	Korespondujący preload register musi zostać aktywowany za pomocą OCxPE
	//	w rejestrze CCMRx
	//	Ewentualnie automatyczny reload preload register w bicie ARPE w CR1
	//	(w zliczaniu w górę lub centrowany do środka)
	//	Jako preload register jest przepisywany do rejestru cieni tylko wtedy kiedy
	//	zachodzi zdarzenie uaktualnienia, przed włączeniem licznika, wszystkie
	//	rejestry muszą być zainicjalizowane poprzez zapis bitu UG w EGR

	TIM_t TIMER_BASIC =
	{
		.TIMx = TIMx,
	};

	CHANNEL_t TIMER_TMP =
	{
		.timer = &TIMER_BASIC,
		.channel = channel,		//	Numer kanału, na który będzie podawany sygnał PWM: CH1, CH1N, CH2, CH2N, CH3, CH3N, CH4, CH4N,
		.work_mode = work_mode,	//	Wybór trybu pracy: FROZEN, ACTIVE,
		.duty = duty,
	};

	TIM_mode_init(&TIMER_TMP);

}

void TIM_start_counting(TIM_TypeDef *TIMx)
{
	//	-włączenie generowania sygnału PWM czyli start licznika,
	//	-dodatkowo jeśli używamy timera, który obsługuje 'brake and dead-time' to należy
	//	dokonać włączenia sygnału na wyjście, bez tego sygnał nie zostanie podany na linie,

	if(IS_TIM_BREAK_INSTANCE(TIMx) == 1)
	{
		LL_TIM_EnableAllOutputs(TIMx);
	}

	LL_TIM_EnableCounter(TIMx);
}

void TIM_stop_counting(TIM_TypeDef *TIMx)
{
	LL_TIM_DisableCounter(TIMx);
}

void TIM_set_basic_parameters(TIM_t *pTIMx, const uint32_t preskaler, const uint32_t frequency)
{
	//	-ustawienie parametrów podstawowego zegara,
	//	-dane wejściowe to preskaler i frequency a na ich podstawie wyliczana jest auto_reload,
	//	-auto_reload to wartość wpisywana do rejestru ARR, jest to wypełnienie sygnału PWM,
	//	-jeżeli chcemy zmienić jeden z parametrów na wartość znaną to drugi ustawiamy na również
	//	znaną wartość a trzeci obliczamy,

	//pTIMx->preskaler = preskaler;
	pTIMx->frequency = frequency;

	uint32_t prescal = __LL_TIM_CALC_PSC(SYS_CORE_CLOCK, pTIMx->NEW_FREQ);

	//	Wartość preskalera, preskaler 16-bitowy!!! wartości od 1..65535:
	LL_TIM_SetPrescaler(pTIMx->TIMx, prescal);		//	TIM_CR1_DIR | TIM_CR1_CMS

	//	Obliczenie wartości rejestru automatycznego przeładowania:
	//uint32_t auto_reload = __LL_TIM_CALC_ARR(SYS_CORE_CLOCK, pTIMx->preskaler - 1, pTIMx->frequency) + 1;
	uint32_t auto_reload = __LL_TIM_CALC_ARR(SYS_CORE_CLOCK, prescal, pTIMx->frequency) + 1;


	//	Zapis do rejestru ARR. Wpisana wartość musi zostać jeszcze załadowana do rejestru cieni:
	LL_TIM_SetAutoReload(pTIMx->TIMx, auto_reload - 1);			//	TIMx->ARR

	LL_TIM_GenerateEvent_UPDATE(pTIMx->TIMx);
	LL_TIM_ClearFlag_UPDATE(pTIMx->TIMx);

	/*
	VT100_text("USTAWIONE PARAMETRY BAZOWE ZEGARA:");
	VT100_EOL();
	VT100_show_data("Preskaler:      ", preskaler);
	VT100_show_data("Frequency [Hz]: ", frequency);
	VT100_show_data("Auto reload:    ", auto_reload);
	VT100_EOL();
	*/
}


//	////////////////////////////////////////////////////////////////////////////
void TIM_one_pulse_mode(void)
{
	/*
	 * WYKONANIE ZADANEJ ILOŚCI CYKLI PWM:
	 * licznik przestaje zliczać w momencie zajścia zdarzenia UPDATE,
	 * LL_TIM_SetOnePulseMode(TIMx, LL_TIM_ONEPULSEMODE_SINGLE);
	 * Licznik nie jest zatrzymywany podczas wystąpienia zdarzenia UPDATE,
	 * LL_TIM_ONEPULSEMODE_REPETITIVE
	 * Wystąpienie zdarzenia UPDATE może być odwleczone poprzez wpisanie wartości do rejestru RCR
	 * LL_TIM_SetRepetitionCounter(TIMx, ILOŚĆ_CYKLI_DO_WYKONANIA - 1);
	 *
	 */

#ifdef STM32G4
	TIM_TypeDef *TIMx    = TIM8;
	uint32_t     channel = LL_TIM_CHANNEL_CH1;


	LL_DBGMCU_APB2_GRP1_FreezePeriph(LL_DBGMCU_APB2_GRP1_TIM8_STOP);

	uint16_t prescal     = 10;
	uint16_t freq        = 1000;
	uint16_t auto_reload = __LL_TIM_CALC_ARR(SYS_CORE_CLOCK, prescal - 1, freq) + 1;

	LL_APB2_GRP1_EnableClock   (LL_APB2_GRP1_PERIPH_TIM8);
	LL_TIM_EnableARRPreload    (TIMx);
	LL_TIM_SetClockSource      (TIMx, LL_TIM_CLOCKSOURCE_INTERNAL);
	LL_TIM_SetCounterMode      (TIMx, LL_TIM_COUNTERMODE_UP);
	LL_TIM_SetPrescaler        (TIMx, prescal - 1);
	LL_TIM_SetAutoReload       (TIMx, auto_reload - 1);
	LL_TIM_SetOnePulseMode     (TIMx, LL_TIM_ONEPULSEMODE_SINGLE);		//	Dla ciągłego zliczania: REPETITIVE,
	LL_TIM_SetRepetitionCounter(TIMx, 100 - 1);
	LL_TIM_SetClockDivision    (TIMx, LL_TIM_CLOCKDIVISION_DIV1);		//	Dzielnik sygnału wejściowego CK_INT
	LL_TIM_OC_SetMode          (TIMx, channel, LL_TIM_OCMODE_PWM2);		//	Wybór trybu pracy,
	LL_TIM_OC_SetPolarity      (TIMx, channel, LL_TIM_OCPOLARITY_HIGH);	//	Polaryzacja,
	LL_TIM_OC_EnablePreload    (TIMx, channel);
	LL_TIM_OC_SetCompareCH1    (TIMx, (auto_reload>>2) );				//	Współczynnik wypełnienia,
	LL_TIM_CC_EnableChannel    (TIMx, channel);							//	Podanie sygnału na pin wyjściowy,
	LL_TIM_EnableAllOutputs    (TIMx);	//	!!!!!!!!!!!!!!!!!!!!
	LL_TIM_GenerateEvent_UPDATE(TIMx);
	LL_TIM_ClearFlag_UPDATE    (TIMx);
	LL_TIM_EnableCounter       (TIMx);
#endif
}


void TIM_one_pulse_mode_3(void)
{
	/*
	 * WYKONANIE ZADANEJ ILOŚCI CYKLI PWM:
	 * licznik przestaje zliczać w momencie zajścia zdarzenia UPDATE,
	 * LL_TIM_SetOnePulseMode(TIMx, LL_TIM_ONEPULSEMODE_SINGLE);
	 * Licznik nie jest zatrzymywany podczas wystąpienia zdarzenia UPDATE,
	 * LL_TIM_ONEPULSEMODE_REPETITIVE
	 * Wystąpienie zdarzenia UPDATE może być odwleczone poprzez wpisanie wartości do rejestru RCR
	 * LL_TIM_SetRepetitionCounter(TIMx, ILOŚĆ_CYKLI_DO_WYKONANIA - 1);
	 *
	 */

#ifdef STM32G4
	TIM_TypeDef *TIMx    = TIM8;
	uint32_t     channel = LL_TIM_CHANNEL_CH3;


	LL_DBGMCU_APB2_GRP1_FreezePeriph(LL_DBGMCU_APB2_GRP1_TIM8_STOP);

	uint16_t prescal     = 10;
	uint16_t freq        = 1000;
	uint16_t auto_reload = __LL_TIM_CALC_ARR(SYS_CORE_CLOCK, prescal - 1, freq) + 1;

	LL_APB2_GRP1_EnableClock   (LL_APB2_GRP1_PERIPH_TIM8);
	LL_TIM_EnableARRPreload    (TIMx);
	LL_TIM_SetClockSource      (TIMx, LL_TIM_CLOCKSOURCE_INTERNAL);
	LL_TIM_SetCounterMode      (TIMx, LL_TIM_COUNTERMODE_UP);
	LL_TIM_SetPrescaler        (TIMx, prescal - 1);
	LL_TIM_SetAutoReload       (TIMx, auto_reload - 1);
	LL_TIM_SetOnePulseMode     (TIMx, LL_TIM_ONEPULSEMODE_SINGLE);		//	Dla ciągłego zliczania: REPETITIVE,
	//LL_TIM_SetRepetitionCounter(TIMx, 10 - 1);
	LL_TIM_SetClockDivision    (TIMx, LL_TIM_CLOCKDIVISION_DIV1);		//	Dzielnik sygnału wejściowego CK_INT
	LL_TIM_OC_SetMode          (TIMx, channel, LL_TIM_OCMODE_PWM2);		//	Wybór trybu pracy,
	LL_TIM_OC_SetPolarity      (TIMx, channel, LL_TIM_OCPOLARITY_HIGH);	//	Polaryzacja,
	LL_TIM_OC_EnablePreload    (TIMx, channel);
	LL_TIM_OC_SetCompareCH3    (TIMx, (auto_reload>>3) );				//	Współczynnik wypełnienia,
	LL_TIM_CC_EnableChannel    (TIMx, channel);							//	Podanie sygnału na pin wyjściowy,
	LL_TIM_EnableAllOutputs    (TIMx);	//	!!!!!!!!!!!!!!!!!!!!
	LL_TIM_GenerateEvent_UPDATE(TIMx);
	LL_TIM_ClearFlag_UPDATE    (TIMx);
	LL_TIM_EnableCounter       (TIMx);
#endif
}







