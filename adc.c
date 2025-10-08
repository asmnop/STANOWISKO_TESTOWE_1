/*
 * adc.c
 *
 *  Created on: 17 paź 2024
 *      Author: asmnop
 */


#include "../../LIB/Inc/adc.h"
#include "../../LIB/Inc/dma.h"
#include "../../LIB/Inc/universal_functions.h"


//	INFO:
//	-zakres możliwych wartość f dla ADC: można w CUBEMX w zakładce clock congiguration ustawić wartość z poza
//	zakresu. Wtedy pole wyświetli się na czerwono wraz z informacją o zakresie akceptowalnych f.
//	-zmieniając instancję ADC i jeżeli wykorzystujemy DMA to w strukturze z parametrami musimy
//	zmienić 'periph_request' oraz ewentualnie 'number_of_data',

//	PRZEZNACZENIE:
//	-odczyt temperatury z wewnętrznego termometru,
//	-odczyt wskazań potencjometru,
//	-odczyt wskazań z joysticka, na który składają się dwa potencjometry, odczyt jednoczesny,
//	-odczyt kilku czujników analogowych podłącznych do ADC, odczyt poprzez wymuszenie lub ciągły,

//	ŹRÓDŁA NA TEMAT ADC:
//	https://forbot.pl/blog/kurs-stm32l4-przetworniki-analogowo-cyfrowe-adc-id46587
//	https://www.stm32wrobotyce.pl/2022/12/29/kurs-stm32-ll-cz-13-wstep-do-konwertera-adc/
//	https://ep.com.pl/kursy/tutoriale/10316-stm32-dla-poczatkujacych-i-nie-tylko.-przetwornik-analogowo-cyfrowy
//	https://deepbluembedded.com/stm32-adc-multi-channel-scan-dma-poll-single-conversion/

//	MOŻLIWE KONFIGURACJE:
//	-pojedyncza konwersja na wybranym kanale, blokujące oczekiwanie na koniec konwersji,
//	-ciągła konwersja na wybranym kanale, blokujące oczekiwanie na koniec konwersji, występowanie
//	nadpisywania odebranych danych,
//	-ciągła konwersja na wybranym kanale, odczyt nowej danej w przerwaniu od zakończenia konwersji,
//	-ciągła konwersja na kilku kanałach, odczyt nowej danej w przerwaniu od zakończenia pojedynczej konwersji,
//	-ciągła konwersja na kilku kanałach, odczyt za pomocą DMA,
//	-ciągła konwersja na kilku kanałach, odczyt nowej danej w sposób dowolny, konwersje wstrzykiwane,


//	////////////////////////////////////////////////////////////////////////////
void ADC_init_G4(ADC_t *pADCx)
{
	//	ADC z użyciem LL:

	//	Wybór zegara i jego ustawienie:
	//LL_ADC_SetCommonClock(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_CLOCK_SYNC_PCLK_DIV2);

#ifdef STM32L4
	LL_RCC_SetADCClockSource(LL_RCC_ADC_CLKSOURCE_PLLSAI1);	//	NONE, PLLSAI1, PLLSAI2, SYSCLK,
#endif

#ifdef STM32G4

#endif

	//	PODPIĘCIA ZEGARÓW:
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);

	//	Konfiguracja pracy linii w trybie analogowym:
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_6, LL_GPIO_MODE_ANALOG);
	LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_6, LL_GPIO_PULL_NO);

	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_7, LL_GPIO_MODE_ANALOG);
	LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_7, LL_GPIO_PULL_NO);

	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_11, LL_GPIO_MODE_ANALOG);
	LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_11, LL_GPIO_PULL_NO);

	LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_15, LL_GPIO_MODE_ANALOG);
	LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_15, LL_GPIO_PULL_NO);


	//	Wybór zegara i jego ustawienie:
	//LL_ADC_SetCommonClock(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_CLOCK_SYNC_PCLK_DIV2);
	LL_RCC_SetADCClockSource(LL_RCC_ADC12_CLKSOURCE_SYSCLK);	//	ADC12, ADC345 ___ NONE, PLL, SYSCLK,

	//	PODPIĘCE I WYBÓR ZEGARÓW DO OBSŁUGI ADC:
	//	Podłączenie zegara i jego taktowania do ADC:
	if((pADCx->pADCx == ADC1) || (pADCx->pADCx == ADC2))
	{
		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC12);
	}
	else if(pADCx->pADCx == ADC3)
	{
		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC345);
	}


	//LL_ADC_SetCommonClock(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_CLOCK_SYNC_PCLK_DIV4);

	//	Rozdzielczość:
	LL_ADC_SetResolution(pADCx->pADCx, pADCx->resolution);	//	str. 589

	//	Sposób wyrównywania danych, do prawej czy do lewej:
	LL_ADC_SetDataAlignment(pADCx->pADCx, pADCx->data_alignment);	//	str. 589

	//	Wybór sposobu wyzwalania konwersji:
	LL_ADC_REG_SetTriggerSource(pADCx->pADCx, pADCx->trigger_source);

	if(pADCx->trigger_source != LL_ADC_REG_TRIG_SOFTWARE)
	{
		//	Wybór zbocza wyzwalającego konwersję jeśli została ustawiona na tryb różny od programowego:
		//	RISING, FALLING, RISINGFALLING,
		LL_ADC_REG_SetTriggerEdge(pADCx->pADCx, LL_ADC_REG_TRIG_EXT_RISING);
	}

	//	Ustawienie trybu konwersji: pojedyncza lub ciągła:
	LL_ADC_REG_SetContinuousMode(pADCx->pADCx, pADCx->mode);	//	SINGLE, CONTINUOUS, str. 588,

	//	Jeśli nie zdążymy odczytać odbieranych danych to:
	//	LL_ADC_REG_OVR_DATA_OVERWRITTEN - nadpisanie danych,
	//	LL_ADC_REG_OVR_DATA_PRESERVED - zachowanie ostatnio nieodebranych,
	LL_ADC_REG_SetOverrun(pADCx->pADCx, pADCx->overrun);

	//	LL_ADC_OVS_REG_CONT - jeden triger wyzwala wszystkie pomiary,
	LL_ADC_SetOverSamplingDiscont(pADCx->pADCx, LL_ADC_OVS_REG_CONT);



	//	Ustawienie ilości konwersji w pojedynczej sekwencji:
	LL_ADC_REG_SetSequencerLength(pADCx->pADCx, pADCx->sequencer_length);	//	str. 597,

	//	########################################################################
	for(uint8_t i=0; i<pADCx->sequencer_length + 1; i++)
	{
		//	Wybór kanału do próbkowania oraz jego kolejności:
		LL_ADC_REG_SetSequencerRanks(pADCx->pADCx, pADCx->rank[i], pADCx->channel[i]);

		//	Ustawienie czasu próbkowania niezależnie dla każdego kanału:
		LL_ADC_SetChannelSamplingTime(pADCx->pADCx, pADCx->channel[i], pADCx->sampling_time[i]);	//	str. 583,

		LL_ADC_SetChannelSingleDiff(pADCx->pADCx, pADCx->channel[i], LL_ADC_SINGLE_ENDED);
	}
	//	########################################################################

	//	Wybór trybu: niezależny czyli każdy ADCx wykonuje konwersję osobno,
	//	Tryb podwójny: master i slave, jednoczesne konwersje na dwóch instancjach ADC,
	LL_ADC_SetMultimode(__LL_ADC_COMMON_INSTANCE(pADCx->pADCx), LL_ADC_MULTI_INDEPENDENT);

	//	Działanie jakie zachodzi podczas aktywacji wstrzykiwania,
	LL_ADC_SetOverSamplingScope(pADCx->pADCx, LL_ADC_OVS_DISABLE);

	//	Wyłączenie DPD, aby mogła zajść kalibracja:
	LL_ADC_DisableDeepPowerDown(pADCx->pADCx);

	//	Włączenie wewnętrznego regulatora napięcia:
	LL_ADC_EnableInternalRegulator(pADCx->pADCx);
	//	Odczekanie czasu tADCVREG_STUP = 20 [us], DS, str. 173,
	delay_ms(1);

	//	Włączenie kalibracji, powiązane z wyborem trybu kalibracji:
	LL_ADC_StartCalibration(pADCx->pADCx, LL_ADC_SINGLE_ENDED);
	while(LL_ADC_IsCalibrationOnGoing(pADCx->pADCx) != 0);

	//	Włączenie ADC bez rozpoczęcia jakichkolwiek konwersji:
	LL_ADC_Enable(pADCx->pADCx);
	//	Po włączeniu ADC ustawiana jest flaga ADRDY:
	while(LL_ADC_IsActiveFlag_ADRDY(pADCx->pADCx) == 0);

	//	Aktywacja transferu danych z ADC poprzez DMA w wybranym trybie:
	LL_ADC_REG_SetDMATransfer(pADCx->pADCx, LL_ADC_REG_DMA_TRANSFER_UNLIMITED);	//	NONE, LIMITED, UNLIMITED,

	//	Rozpoczęcie konwersji:
	LL_ADC_REG_StartConversion(pADCx->pADCx);
}

void ADC_init_L4(ADC_t *pADCx)
{
	//	ADC z użyciem LL:

	//	Podłączenie zegara i jego taktowania do ADC:
	LL_AHB2_GRP1_EnableClock(pADCx->enable_periph);

	//	Wybór zegara i jego ustawienie:
	//LL_ADC_SetCommonClock(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_CLOCK_SYNC_PCLK_DIV2);

#ifdef STM32L4
	LL_RCC_SetADCClockSource(LL_RCC_ADC_CLKSOURCE_PLLSAI1);	//	NONE, PLLSAI1, PLLSAI2, SYSCLK,
#endif

#ifdef STM32G4

#endif

	LL_ADC_SetCommonClock(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_CLOCK_SYNC_PCLK_DIV4);

	//	Rozdzielczość:
	LL_ADC_SetResolution(pADCx->pADCx, pADCx->resolution);	//	str. 589

	//	Sposób wyrównywania danych, do prawej czy do lewej:
	LL_ADC_SetDataAlignment(pADCx->pADCx, pADCx->data_alignment);	//	str. 589

	//	Wybór sposobu wyzwalania konwersji:
	LL_ADC_REG_SetTriggerSource(pADCx->pADCx, pADCx->trigger_source);

	//	Ustawienie trybu konwersji: pojedyncza lub ciągła:
	LL_ADC_REG_SetContinuousMode(pADCx->pADCx, pADCx->mode);	//	SINGLE, CONTINUOUS, str. 588,

	//LL_ADC_REG_SetOverrun(pADCx->pADCx, LL_ADC_REG_OVR_DATA_OVERWRITTEN);

	//	Ustawienie ilości konwersji w pojedynczej sekwencji:
	LL_ADC_REG_SetSequencerLength(pADCx->pADCx, pADCx->sequencer_length);	//	str. 597,

	for(uint8_t i=0; i<pADCx->sequencer_length + 1; i++)
	{
		//	Wybór kanału do próbkowania oraz jego kolejności:
		LL_ADC_REG_SetSequencerRanks(pADCx->pADCx, pADCx->rank[i], pADCx->channel[i]);

		//	Ustawienie czasu próbkowania niezależnie dla każdego kanału:
		LL_ADC_SetChannelSamplingTime(pADCx->pADCx, pADCx->channel[i], pADCx->sampling_time[i]);	//	str. 583,

		LL_ADC_SetChannelSingleDiff(pADCx->pADCx, pADCx->channel[i], LL_ADC_SINGLE_ENDED);
	}

	//	Wyłączenie DPD, aby mogła zajść kalibracja:
	LL_ADC_DisableDeepPowerDown(pADCx->pADCx);

	//	Włączenie wewnętrznego regulatora napięcia:
	LL_ADC_EnableInternalRegulator(pADCx->pADCx);
	//	Odczekanie czasu tADCVREG_STUP = 20 [us], DS, str. 173,
	delay_ms(1);

	//	Włączenie kalibracji, powiązane z wyborem trybu kalibracji:
	LL_ADC_StartCalibration(pADCx->pADCx, LL_ADC_SINGLE_ENDED);
	while(LL_ADC_IsCalibrationOnGoing(pADCx->pADCx) != 0);

	//	Włączenie ADC bez rozpoczęcia jakichkolwiek konwersji:
	LL_ADC_Enable(pADCx->pADCx);
	//	Po włączeniu ADC ustawiana jest flaga ADRDY:
	while(LL_ADC_IsActiveFlag_ADRDY(pADCx->pADCx) == 0);
}

void ADC_start(ADC_t *pADCx)
{
	LL_ADC_REG_StartConversion(pADCx->pADCx);
}

void ADC_read_data(ADC_t *pADCx)
{
	for(uint8_t i=0; i<pADCx->sequencer_length + 1; i++)
	{
		while(LL_ADC_IsActiveFlag_EOC(pADCx->pADCx) == 0);
		//	Odczyt danych, wybór funkcji zależy od wybranej rodzielczości
		pADCx->data_received[i] = LL_ADC_REG_ReadConversionData8(pADCx->pADCx);	//	str. 600,
	}
}

void ADC_IT_init(ADC_t *pADCx)
{
	//	-zezwolenie na obsługę przerwań lokalnych,

	//	Przerwanie od zakończenia konwersji:
	if(pADCx->enable_IT_EOC == 1)
	{
		LL_ADC_EnableIT_EOC(pADCx->pADCx);
	}
	//	Przerwanie od zakończczenienia sekwencji, czyli zakończenia wszystkich konwersji:
	if(pADCx->enable_IT_EOS == 1)
	{
		LL_ADC_EnableIT_EOS(pADCx->pADCx);
	}
	//	Przerwanie od nadpisania danych:
	if(pADCx->enable_IT_OVR == 1)
	{
		LL_ADC_EnableIT_OVR(pADCx->pADCx);
	}
}

void ADC_stop_conversion(ADC_t *pADCx)
{
	LL_ADC_REG_SetDMATransfer(pADCx->pADCx, LL_ADC_REG_DMA_TRANSFER_NONE);
	LL_ADC_REG_StopConversion(pADCx->pADCx);
}



//	////////////////////////////////////////////////////////////////////////////
void ADC_LL_get_value(void)
{

}

void ADC_LL_TEMP(void)
{
	//	########################################################################
	//	POBRANE Z: void Configure_ADC(void)
	//	########################################################################

	////__IO uint32_t wait_loop_index = 0;

	/*## Configuration of GPIO used by ADC channels ############################*/

	/* Note: On this STM32 device, ADC1 internal channel temperature sensor is mapped on GPIO pin PA.04 */

	/* Enable GPIO Clock */
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);

	/* Configure GPIO in analog mode to be used as ADC input */
	////LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_4, LL_GPIO_MODE_ANALOG);

	/* Connect GPIO analog switch to ADC input */
	////LL_GPIO_EnablePinAnalogControl(GPIOA, LL_GPIO_PIN_4);

	/*## Configuration of NVIC #################################################*/
	/* Configure NVIC to enable ADC1 interruptions */
	////NVIC_SetPriority(ADC1_2_IRQn, 0);
	////NVIC_EnableIRQ(ADC1_2_IRQn);

	/*## Configuration of ADC ##################################################*/

	/*## Configuration of ADC hierarchical scope: common to several ADC ########*/

	/* Enable ADC clock (core clock) */
#ifdef STM32L4
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC);
#endif
	/* Note: Hardware constraint (refer to description of the functions         */
	/*       below):                                                            */
	/*       On this STM32 serie, setting of these features is conditioned to   */
	/*       ADC state:                                                         */
	/*       All ADC instances of the ADC common group must be disabled.        */
	/* Note: In this example, all these checks are not necessary but are        */
	/*       implemented anyway to show the best practice usages                */
	/*       corresponding to reference manual procedure.                       */
	/*       Software can be optimized by removing some of these checks, if     */
	/*       they are not relevant considering previous settings and actions    */
	/*       in user application.                                               */
	/////////??????????if(__LL_ADC_IS_ENABLED_ALL_COMMON_INSTANCE() == 0)
	{
		/* Note: Call of the functions below are commented because they are       */
		/*       useless in this example:                                         */
		/*       setting corresponding to default configuration from reset state. */

		/* Set ADC clock (conversion clock) common to several ADC instances */
		LL_ADC_SetCommonClock(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_CLOCK_SYNC_PCLK_DIV2);

		/* Set ADC measurement path to internal channels */
		LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_PATH_INTERNAL_TEMPSENSOR);

		/* Delay for ADC temperature sensor stabilization time.                   */
		/* Compute number of CPU cycles to wait for, from delay in us.            */
		/* Note: Variable divided by 2 to compensate partially                    */
		/*       CPU processing cycles (depends on compilation optimization).     */
		/* Note: If system core clock frequency is below 200kHz, wait time        */
		/*       is only a few CPU processing cycles.                             */
		/* Note: This delay is implemented here for the purpose in this example.  */
		/*       It can be optimized if merged with other delays                  */
		/*       during ADC activation or if other actions are performed          */
		/*       in the meantime.                                                 */

		/*
		wait_loop_index = ((LL_ADC_DELAY_TEMPSENSOR_STAB_US * (SystemCoreClock / (100000 * 2))) / 10);
		while(wait_loop_index != 0)
		{
			wait_loop_index--;
		}
		*/

		/*## Configuration of ADC hierarchical scope: multimode ####################*/

		/* Set ADC multimode configuration */
		// LL_ADC_SetMultimode(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_MULTI_INDEPENDENT);

		/* Set ADC multimode DMA transfer */
		// LL_ADC_SetMultiDMATransfer(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_MULTI_REG_DMA_EACH_ADC);

		/* Set ADC multimode: delay between 2 sampling phases */
		// LL_ADC_SetMultiTwoSamplingDelay(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_MULTI_TWOSMP_DELAY_1CYCLE);

	}


	/*## Configuration of ADC hierarchical scope: ADC instance #################*/

	/* Note: Hardware constraint (refer to description of the functions         */
	/*       below):                                                            */
	/*       On this STM32 serie, setting of these features is conditioned to   */
	/*       ADC state:                                                         */
	/*       ADC must be disabled.                                              */
	if (LL_ADC_IsEnabled(ADC1) == 0)
	{
		/* Note: Call of the functions below are commented because they are       */
		/*       useless in this example:                                         */
		/*       setting corresponding to default configuration from reset state. */

		/* Set ADC data resolution */
		LL_ADC_SetResolution(ADC1, LL_ADC_RESOLUTION_12B);

		/* Set ADC conversion data alignment */
		LL_ADC_SetResolution(ADC1, LL_ADC_DATA_ALIGN_RIGHT);

		/* Set ADC low power mode */
		// LL_ADC_SetLowPowerMode(ADC1, LL_ADC_LP_MODE_NONE);

		/* Set ADC selected offset number: channel and offset level */
		// LL_ADC_SetOffset(ADC1, LL_ADC_OFFSET_1, LL_ADC_CHANNEL_TEMPSENSOR, 0x000);

	}


	/*## Configuration of ADC hierarchical scope: ADC group regular ############*/

	/* Note: Hardware constraint (refer to description of the functions         */
	/*       below):                                                            */
	/*       On this STM32 serie, setting of these features is conditioned to   */
	/*       ADC state:                                                         */
	/*       ADC must be disabled or enabled without conversion on going        */
	/*       on group regular.                                                  */
	if ((LL_ADC_IsEnabled(ADC1) == 0) || (LL_ADC_REG_IsConversionOngoing(ADC1) == 0)   )
	{
		/* Set ADC group regular trigger source */
		LL_ADC_REG_SetTriggerSource(ADC1, LL_ADC_REG_TRIG_SOFTWARE);

		/* Set ADC group regular trigger polarity */
		// LL_ADC_REG_SetTriggerEdge(ADC1, LL_ADC_REG_TRIG_EXT_RISING);

		/* Set ADC group regular continuous mode */
		LL_ADC_REG_SetContinuousMode(ADC1, LL_ADC_REG_CONV_SINGLE);

		/* Set ADC group regular conversion data transfer */
		// LL_ADC_REG_SetDMATransfer(ADC1, LL_ADC_REG_DMA_TRANSFER_NONE);

		/* Set ADC group regular overrun behavior */
		////LL_ADC_REG_SetOverrun(ADC1, LL_ADC_REG_OVR_DATA_OVERWRITTEN);

		/* Set ADC group regular sequencer */
		/* Note: On this STM32 serie, ADC group regular sequencer is              */
		/*       fully configurable: sequencer length and each rank               */
		/*       affectation to a channel are configurable.                       */
		/*       Refer to description of function                                 */
		/*       "LL_ADC_REG_SetSequencerLength()".                               */

		/* Set ADC group regular sequencer length and scan direction */
		LL_ADC_REG_SetSequencerLength(ADC1, LL_ADC_REG_SEQ_SCAN_DISABLE);

		/* Set ADC group regular sequencer discontinuous mode */
		// LL_ADC_REG_SetSequencerDiscont(ADC1, LL_ADC_REG_SEQ_DISCONT_DISABLE);

		/* Set ADC group regular sequence: channel on the selected sequence rank. */
#ifdef STM32L4
		LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_TEMPSENSOR);
#endif
	}


	/*## Configuration of ADC hierarchical scope: ADC group injected ###########*/

	/* Note: Hardware constraint (refer to description of the functions         */
	/*       below):                                                            */
	/*       On this STM32 serie, setting of these features is conditioned to   */
	/*       ADC state:                                                         */
	/*       ADC must be disabled or enabled without conversion on going        */
	/*       on group injected.                                                 */
	if ((LL_ADC_IsEnabled(ADC1) == 0) || (LL_ADC_INJ_IsConversionOngoing(ADC1) == 0)   )
	{
		/* Note: Call of the functions below are commented because they are       */
		/*       useless in this example:                                         */
		/*       setting corresponding to default configuration from reset state. */

		/* Set ADC group injected trigger source */
		// LL_ADC_INJ_SetTriggerSource(ADC1, LL_ADC_INJ_TRIG_SOFTWARE);

		/* Set ADC group injected trigger polarity */
		// LL_ADC_INJ_SetTriggerEdge(ADC1, LL_ADC_INJ_TRIG_EXT_RISING);

		/* Set ADC group injected conversion trigger  */
		// LL_ADC_INJ_SetTrigAuto(ADC1, LL_ADC_INJ_TRIG_INDEPENDENT);

		/* Set ADC group injected contexts queue mode */
		/* Note: If ADC group injected contexts queue are enabled, configure      */
		/*       contexts using function "LL_ADC_INJ_ConfigQueueContext()".       */
		// LL_ADC_INJ_SetQueueMode(ADC1, LL_ADC_INJ_QUEUE_DISABLE);

		/* Set ADC group injected sequencer */
		/* Note: On this STM32 serie, ADC group injected sequencer is             */
		/*       fully configurable: sequencer length and each rank               */
		/*       affectation to a channel are configurable.                       */
		/*       Refer to description of function                                 */
		/*       "LL_ADC_INJ_SetSequencerLength()".                               */

		/* Set ADC group injected sequencer length and scan direction */
		// LL_ADC_INJ_SetSequencerLength(ADC1, LL_ADC_INJ_SEQ_SCAN_DISABLE);

		/* Set ADC group injected sequencer discontinuous mode */
		// LL_ADC_INJ_SetSequencerDiscont(ADC1, LL_ADC_INJ_SEQ_DISCONT_DISABLE);

		/* Set ADC group injected sequence: channel on the selected sequence rank. */
		// LL_ADC_INJ_SetSequencerRanks(ADC1, LL_ADC_INJ_RANK_1, LL_ADC_CHANNEL_TEMPSENSOR);
	}


	/*## Configuration of ADC hierarchical scope: channels #####################*/

	/* Note: Hardware constraint (refer to description of the functions         */
	/*       below):                                                            */
	/*       On this STM32 serie, setting of these features is conditioned to   */
	/*       ADC state:                                                         */
	/*       ADC must be disabled or enabled without conversion on going        */
	/*       on either groups regular or injected.                              */
	if ((LL_ADC_IsEnabled(ADC1) == 0) || ((LL_ADC_REG_IsConversionOngoing(ADC1) == 0) && (LL_ADC_INJ_IsConversionOngoing(ADC1) == 0)   )   )
	{
		/* Set ADC channels sampling time */
		/* Note: Set long sampling time due to internal channels (VrefInt,        */
		/*       temperature sensor) constraints.                                 */
		/*       Refer to description of function                                 */
		/*       "LL_ADC_SetChannelSamplingTime()".                               */
#ifdef STM32L4
		LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_TEMPSENSOR, LL_ADC_SAMPLINGTIME_247CYCLES_5);
#endif
		/* Set mode single-ended or differential input of the selected            */
		/* ADC channel.                                                           */
		// LL_ADC_SetChannelSingleDiff(ADC1, LL_ADC_CHANNEL_TEMPSENSOR, LL_ADC_SINGLE_ENDED);
	}


	/*## Configuration of ADC transversal scope: analog watchdog ###############*/

	/* Set ADC analog watchdog channels to be monitored */
	// LL_ADC_SetAnalogWDMonitChannels(ADC1, LL_ADC_AWD1, LL_ADC_AWD_DISABLE);

	/* Set ADC analog watchdog thresholds */
	// LL_ADC_ConfigAnalogWDThresholds(ADC1, LL_ADC_AWD1, 0xFFF, 0x000);


	/*## Configuration of ADC transversal scope: oversampling ##################*/

	/* Set ADC oversampling scope */
	// LL_ADC_SetOverSamplingScope(ADC1, LL_ADC_OVS_DISABLE);

	/* Set ADC oversampling parameters */
	// LL_ADC_ConfigOverSamplingRatioShift(ADC1, LL_ADC_OVS_RATIO_2, LL_ADC_OVS_SHIFT_NONE);


	/*## Configuration of ADC interruptions ####################################*/
	/* Enable interruption ADC group regular overrun */
	////LL_ADC_EnableIT_OVR(ADC1);

	/* Note: In this example, end of ADC conversions are awaited by polling     */
	/*       (not by interruption).                                             */


	//	########################################################################
	//	POBRANE Z: void Activate_ADC(void)
	//	########################################################################

	////__IO uint32_t wait_loop_index = 0;
	#if (USE_TIMEOUT == 1)
	uint32_t Timeout = 0; /* Variable used for timeout management */
	#endif /* USE_TIMEOUT */

	/*## Operation on ADC hierarchical scope: ADC instance #####################*/

	/* Note: Hardware constraint (refer to description of the functions         */
	/*       below):                                                            */
	/*       On this STM32 serie, setting of these features is conditioned to   */
	/*       ADC state:                                                         */
	/*       ADC must be disabled.                                              */
	/* Note: In this example, all these checks are not necessary but are        */
	/*       implemented anyway to show the best practice usages                */
	/*       corresponding to reference manual procedure.                       */
	/*       Software can be optimized by removing some of these checks, if     */
	/*       they are not relevant considering previous settings and actions    */
	/*       in user application.                                               */
	if (LL_ADC_IsEnabled(ADC1) == 0)
	{
		/* Disable ADC deep power down (enabled by default after reset state) */
		LL_ADC_DisableDeepPowerDown(ADC1);

		/* Enable ADC internal voltage regulator */
		LL_ADC_EnableInternalRegulator(ADC1);

		/* Delay for ADC internal voltage regulator stabilization.                */
		/* Compute number of CPU cycles to wait for, from delay in us.            */
		/* Note: Variable divided by 2 to compensate partially                    */
		/*       CPU processing cycles (depends on compilation optimization).     */
		/* Note: If system core clock frequency is below 200kHz, wait time        */
		/*       is only a few CPU processing cycles.                             */

		/*
		wait_loop_index = ((LL_ADC_DELAY_INTERNAL_REGUL_STAB_US * (SystemCoreClock / (100000 * 2))) / 10);
		while(wait_loop_index != 0)
		{
		wait_loop_index--;
		}
		*/

		/* Run ADC self calibration */
		LL_ADC_StartCalibration(ADC1, LL_ADC_SINGLE_ENDED);

		/* Poll for ADC effectively calibrated */
		#if (USE_TIMEOUT == 1)
		Timeout = ADC_CALIBRATION_TIMEOUT_MS;
		#endif /* USE_TIMEOUT */

		while (LL_ADC_IsCalibrationOnGoing(ADC1) != 0)
		{
			#if (USE_TIMEOUT == 1)
			/* Check Systick counter flag to decrement the time-out value */
			if (LL_SYSTICK_IsActiveCounterFlag())
			{
				if(Timeout-- == 0)
				{
				/* Time-out occurred. Set LED to blinking mode */
				LED_Blinking(LED_BLINK_ERROR);
				}
			}
			#endif /* USE_TIMEOUT */
		}

		/* Delay between ADC end of calibration and ADC enable.                   */
		/* Note: Variable divided by 2 to compensate partially                    */
		/*       CPU processing cycles (depends on compilation optimization).     */

		/*
		wait_loop_index = (ADC_DELAY_CALIB_ENABLE_CPU_CYCLES >> 1);

		while(wait_loop_index != 0)
		{
			wait_loop_index--;
		}
		*/

		/* Enable ADC */
		LL_ADC_Enable(ADC1);

		/* Poll for ADC ready to convert */
		#if (USE_TIMEOUT == 1)
		Timeout = ADC_ENABLE_TIMEOUT_MS;
		#endif /* USE_TIMEOUT */

		while (LL_ADC_IsActiveFlag_ADRDY(ADC1) == 0)
		{
			#if (USE_TIMEOUT == 1)
			/* Check Systick counter flag to decrement the time-out value */
			if (LL_SYSTICK_IsActiveCounterFlag())
			{
			if(Timeout-- == 0)
			{
			/* Time-out occurred. Set LED to blinking mode */
			LED_Blinking(LED_BLINK_ERROR);
			}
			}
			#endif /* USE_TIMEOUT */
		}

		/* Note: ADC flag ADRDY is not cleared here to be able to check ADC       */
		/*       status afterwards.                                               */
		/*       This flag should be cleared at ADC Deactivation, before a new    */
		/*       ADC activation, using function "LL_ADC_ClearFlag_ADRDY()".       */
	}

	/*## Operation on ADC hierarchical scope: ADC group regular ################*/
	/* Note: No operation on ADC group regular performed here.                  */
	/*       ADC group regular conversions to be performed after this function  */
	/*       using function:                                                    */
	/*       "LL_ADC_REG_StartConversion();"                                    */

	/*## Operation on ADC hierarchical scope: ADC group injected ###############*/
	/* Note: No operation on ADC group injected performed here.                 */
	/*       ADC group injected conversions to be performed after this function */
	/*       using function:                                                    */
	/*       "LL_ADC_INJ_StartConversion();"                                    */






}

uint32_t ADC_LL_TEMP_COMPUTE(void)
{
	 #define VDDA_APPLI                       ((uint32_t)3300)
	//	########################################################################
	//	POBRANE Z: void ConversionStartPoll_ADC_GrpRegular(void)
	//	########################################################################

	#if (USE_TIMEOUT == 1)
	uint32_t Timeout = 0; /* Variable used for timeout management */
	#endif /* USE_TIMEOUT */

	/* Start ADC group regular conversion */
	/* Note: Hardware constraint (refer to description of the function          */
	/*       below):                                                            */
	/*       On this STM32 serie, setting of this feature is conditioned to     */
	/*       ADC state:                                                         */
	/*       ADC must be enabled without conversion on going on group regular,  */
	/*       without ADC disable command on going.                              */
	/* Note: In this example, all these checks are not necessary but are        */
	/*       implemented anyway to show the best practice usages                */
	/*       corresponding to reference manual procedure.                       */
	/*       Software can be optimized by removing some of these checks, if     */
	/*       they are not relevant considering previous settings and actions    */
	/*       in user application.                                               */

	if ((LL_ADC_IsEnabled(ADC1) == 1) && (LL_ADC_IsDisableOngoing(ADC1) == 0) && (LL_ADC_REG_IsConversionOngoing(ADC1) == 0)   )
	{
		LL_ADC_REG_StartConversion(ADC1);
	}
	else
	{
		/* Error: ADC conversion start could not be performed */
		////LED_Blinking(LED_BLINK_ERROR);
	}

	#if (USE_TIMEOUT == 1)
	Timeout = ADC_UNITARY_CONVERSION_TIMEOUT_MS;
	#endif /* USE_TIMEOUT */

	while (LL_ADC_IsActiveFlag_EOC(ADC1) == 0)
	{
		#if (USE_TIMEOUT == 1)
		/* Check Systick counter flag to decrement the time-out value */
		if (LL_SYSTICK_IsActiveCounterFlag())
		{
		if(Timeout-- == 0)
		{
		/* Time-out occurred. Set LED to blinking mode */
		LED_Blinking(LED_BLINK_SLOW);
	}
	}
	#endif /* USE_TIMEOUT */
	}

	/* Clear flag ADC group regular end of unitary conversion */
	/* Note: This action is not needed here, because flag ADC group regular   */
	/*       end of unitary conversion is cleared automatically when          */
	/*       software reads conversion data from ADC data register.           */
	/*       Nevertheless, this action is done anyway to show how to clear    */
	/*       this flag, needed if conversion data is not always read          */
	/*       or if group injected end of unitary conversion is used (for      */
	/*       devices with group injected available).                          */

	//LL_ADC_ClearFlag_EOC(ADC1);

	uint16_t data = LL_ADC_REG_ReadConversionData12(ADC1);
	uint32_t final_temp;
	//uint32_t TS_CAL_1 = (int32_t)(int32_t)*((uint16_t*) (0x1FFF75A8UL));
	//uint32_t TS_CAL_1_ = (int32_t)(int32_t)*((uint16_t*) (0x1FFF75A9UL));
	return final_temp = __LL_ADC_CALC_TEMPERATURE(VDDA_APPLI, data, LL_ADC_RESOLUTION_12B);


}



//	////////////////////////////////////////////////////////////////////////////
void ADC_LL_POT(uint32_t Channel)
{
	//	########################################################################
	//	POBRANE Z: void Configure_ADC(void)
	//	########################################################################

	////__IO uint32_t wait_loop_index = 0;

	/*## Configuration of GPIO used by ADC channels ############################*/

	/* Note: On this STM32 device, ADC1 internal channel temperature sensor is mapped on GPIO pin PA.04 */

	/* Enable GPIO Clock */
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);

	/* Configure GPIO in analog mode to be used as ADC input */
	LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_0, LL_GPIO_MODE_ANALOG);

	/* Connect GPIO analog switch to ADC input */
	LL_GPIO_EnablePinAnalogControl(GPIOC, LL_GPIO_PIN_0);

	/*## Configuration of NVIC #################################################*/
	/* Configure NVIC to enable ADC1 interruptions */
	////NVIC_SetPriority(ADC1_2_IRQn, 0);
	////NVIC_EnableIRQ(ADC1_2_IRQn);

	/*## Configuration of ADC ##################################################*/

	/*## Configuration of ADC hierarchical scope: common to several ADC ########*/

	/* Enable ADC clock (core clock) */
#ifdef STM32L4
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC);
#endif
	/* Note: Hardware constraint (refer to description of the functions         */
	/*       below):                                                            */
	/*       On this STM32 serie, setting of these features is conditioned to   */
	/*       ADC state:                                                         */
	/*       All ADC instances of the ADC common group must be disabled.        */
	/* Note: In this example, all these checks are not necessary but are        */
	/*       implemented anyway to show the best practice usages                */
	/*       corresponding to reference manual procedure.                       */
	/*       Software can be optimized by removing some of these checks, if     */
	/*       they are not relevant considering previous settings and actions    */
	/*       in user application.                                               */
	///////////????????????if(__LL_ADC_IS_ENABLED_ALL_COMMON_INSTANCE() == 0)
	{
		/* Note: Call of the functions below are commented because they are       */
		/*       useless in this example:                                         */
		/*       setting corresponding to default configuration from reset state. */

		/* Set ADC clock (conversion clock) common to several ADC instances */
		LL_ADC_SetCommonClock(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_CLOCK_SYNC_PCLK_DIV2);

		/* Set ADC measurement path to internal channels */
		LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_PATH_INTERNAL_TEMPSENSOR);

		/* Delay for ADC temperature sensor stabilization time.                   */
		/* Compute number of CPU cycles to wait for, from delay in us.            */
		/* Note: Variable divided by 2 to compensate partially                    */
		/*       CPU processing cycles (depends on compilation optimization).     */
		/* Note: If system core clock frequency is below 200kHz, wait time        */
		/*       is only a few CPU processing cycles.                             */
		/* Note: This delay is implemented here for the purpose in this example.  */
		/*       It can be optimized if merged with other delays                  */
		/*       during ADC activation or if other actions are performed          */
		/*       in the meantime.                                                 */

		/*
		wait_loop_index = ((LL_ADC_DELAY_TEMPSENSOR_STAB_US * (SystemCoreClock / (100000 * 2))) / 10);
		while(wait_loop_index != 0)
		{
			wait_loop_index--;
		}
		*/

		/*## Configuration of ADC hierarchical scope: multimode ####################*/

		/* Set ADC multimode configuration */
		// LL_ADC_SetMultimode(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_MULTI_INDEPENDENT);

		/* Set ADC multimode DMA transfer */
		// LL_ADC_SetMultiDMATransfer(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_MULTI_REG_DMA_EACH_ADC);

		/* Set ADC multimode: delay between 2 sampling phases */
		// LL_ADC_SetMultiTwoSamplingDelay(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_MULTI_TWOSMP_DELAY_1CYCLE);

	}


	/*## Configuration of ADC hierarchical scope: ADC instance #################*/

	/* Note: Hardware constraint (refer to description of the functions         */
	/*       below):                                                            */
	/*       On this STM32 serie, setting of these features is conditioned to   */
	/*       ADC state:                                                         */
	/*       ADC must be disabled.                                              */
	if (LL_ADC_IsEnabled(ADC1) == 0)
	{
		/* Note: Call of the functions below are commented because they are       */
		/*       useless in this example:                                         */
		/*       setting corresponding to default configuration from reset state. */

		/* Set ADC data resolution */
		LL_ADC_SetResolution(ADC1, LL_ADC_RESOLUTION_12B);

		/* Set ADC conversion data alignment */
		LL_ADC_SetDataAlignment(ADC1, LL_ADC_DATA_ALIGN_RIGHT);

		/* Set ADC low power mode */
		// LL_ADC_SetLowPowerMode(ADC1, LL_ADC_LP_MODE_NONE);

		/* Set ADC selected offset number: channel and offset level */
		// LL_ADC_SetOffset(ADC1, LL_ADC_OFFSET_1, LL_ADC_CHANNEL_TEMPSENSOR, 0x000);

	}


	/*## Configuration of ADC hierarchical scope: ADC group regular ############*/

	/* Note: Hardware constraint (refer to description of the functions         */
	/*       below):                                                            */
	/*       On this STM32 serie, setting of these features is conditioned to   */
	/*       ADC state:                                                         */
	/*       ADC must be disabled or enabled without conversion on going        */
	/*       on group regular.                                                  */
	if ((LL_ADC_IsEnabled(ADC1) == 0) || (LL_ADC_REG_IsConversionOngoing(ADC1) == 0)   )
	{
		/* Set ADC group regular trigger source */
		LL_ADC_REG_SetTriggerSource(ADC1, LL_ADC_REG_TRIG_SOFTWARE);

		/* Set ADC group regular trigger polarity */
		// LL_ADC_REG_SetTriggerEdge(ADC1, LL_ADC_REG_TRIG_EXT_RISING);

		/* Set ADC group regular continuous mode */
		LL_ADC_REG_SetContinuousMode(ADC1, LL_ADC_REG_CONV_SINGLE);

		/* Set ADC group regular conversion data transfer */
		// LL_ADC_REG_SetDMATransfer(ADC1, LL_ADC_REG_DMA_TRANSFER_NONE);

		/* Set ADC group regular overrun behavior */
		////LL_ADC_REG_SetOverrun(ADC1, LL_ADC_REG_OVR_DATA_OVERWRITTEN);

		/* Set ADC group regular sequencer */
		/* Note: On this STM32 serie, ADC group regular sequencer is              */
		/*       fully configurable: sequencer length and each rank               */
		/*       affectation to a channel are configurable.                       */
		/*       Refer to description of function                                 */
		/*       "LL_ADC_REG_SetSequencerLength()".                               */

		/* Set ADC group regular sequencer length and scan direction */
		//LL_ADC_REG_SetSequencerLength(ADC1, LL_ADC_REG_SEQ_SCAN_DISABLE);
		LL_ADC_REG_SetSequencerLength(ADC1, LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS);

		/* Set ADC group regular sequencer discontinuous mode */
		// LL_ADC_REG_SetSequencerDiscont(ADC1, LL_ADC_REG_SEQ_DISCONT_DISABLE);

		/* Set ADC group regular sequence: channel on the selected sequence rank. */
		LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, Channel);
#ifdef STM32L4
		LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_2, LL_ADC_CHANNEL_TEMPSENSOR);
#endif
	}


	/*## Configuration of ADC hierarchical scope: ADC group injected ###########*/

	/* Note: Hardware constraint (refer to description of the functions         */
	/*       below):                                                            */
	/*       On this STM32 serie, setting of these features is conditioned to   */
	/*       ADC state:                                                         */
	/*       ADC must be disabled or enabled without conversion on going        */
	/*       on group injected.                                                 */
	if ((LL_ADC_IsEnabled(ADC1) == 0) || (LL_ADC_INJ_IsConversionOngoing(ADC1) == 0)   )
	{
		/* Note: Call of the functions below are commented because they are       */
		/*       useless in this example:                                         */
		/*       setting corresponding to default configuration from reset state. */

		/* Set ADC group injected trigger source */
		// LL_ADC_INJ_SetTriggerSource(ADC1, LL_ADC_INJ_TRIG_SOFTWARE);

		/* Set ADC group injected trigger polarity */
		// LL_ADC_INJ_SetTriggerEdge(ADC1, LL_ADC_INJ_TRIG_EXT_RISING);

		/* Set ADC group injected conversion trigger  */
		// LL_ADC_INJ_SetTrigAuto(ADC1, LL_ADC_INJ_TRIG_INDEPENDENT);

		/* Set ADC group injected contexts queue mode */
		/* Note: If ADC group injected contexts queue are enabled, configure      */
		/*       contexts using function "LL_ADC_INJ_ConfigQueueContext()".       */
		// LL_ADC_INJ_SetQueueMode(ADC1, LL_ADC_INJ_QUEUE_DISABLE);

		/* Set ADC group injected sequencer */
		/* Note: On this STM32 serie, ADC group injected sequencer is             */
		/*       fully configurable: sequencer length and each rank               */
		/*       affectation to a channel are configurable.                       */
		/*       Refer to description of function                                 */
		/*       "LL_ADC_INJ_SetSequencerLength()".                               */

		/* Set ADC group injected sequencer length and scan direction */
		// LL_ADC_INJ_SetSequencerLength(ADC1, LL_ADC_INJ_SEQ_SCAN_DISABLE);

		/* Set ADC group injected sequencer discontinuous mode */
		// LL_ADC_INJ_SetSequencerDiscont(ADC1, LL_ADC_INJ_SEQ_DISCONT_DISABLE);

		/* Set ADC group injected sequence: channel on the selected sequence rank. */
		// LL_ADC_INJ_SetSequencerRanks(ADC1, LL_ADC_INJ_RANK_1, LL_ADC_CHANNEL_TEMPSENSOR);
	}


	/*## Configuration of ADC hierarchical scope: channels #####################*/

	/* Note: Hardware constraint (refer to description of the functions         */
	/*       below):                                                            */
	/*       On this STM32 serie, setting of these features is conditioned to   */
	/*       ADC state:                                                         */
	/*       ADC must be disabled or enabled without conversion on going        */
	/*       on either groups regular or injected.                              */
	if ((LL_ADC_IsEnabled(ADC1) == 0) || ((LL_ADC_REG_IsConversionOngoing(ADC1) == 0) && (LL_ADC_INJ_IsConversionOngoing(ADC1) == 0)   )   )
	{
		/* Set ADC channels sampling time */
		/* Note: Set long sampling time due to internal channels (VrefInt,        */
		/*       temperature sensor) constraints.                                 */
		/*       Refer to description of function                                 */
		/*       "LL_ADC_SetChannelSamplingTime()".                               */
		LL_ADC_SetChannelSamplingTime(ADC1, Channel, LL_ADC_SAMPLINGTIME_640CYCLES_5);

		/* Set mode single-ended or differential input of the selected            */
		/* ADC channel.                                                           */
		// LL_ADC_SetChannelSingleDiff(ADC1, LL_ADC_CHANNEL_TEMPSENSOR, LL_ADC_SINGLE_ENDED);
	}


	/*## Configuration of ADC transversal scope: analog watchdog ###############*/

	/* Set ADC analog watchdog channels to be monitored */
	// LL_ADC_SetAnalogWDMonitChannels(ADC1, LL_ADC_AWD1, LL_ADC_AWD_DISABLE);

	/* Set ADC analog watchdog thresholds */
	// LL_ADC_ConfigAnalogWDThresholds(ADC1, LL_ADC_AWD1, 0xFFF, 0x000);


	/*## Configuration of ADC transversal scope: oversampling ##################*/

	/* Set ADC oversampling scope */
	// LL_ADC_SetOverSamplingScope(ADC1, LL_ADC_OVS_DISABLE);

	/* Set ADC oversampling parameters */
	// LL_ADC_ConfigOverSamplingRatioShift(ADC1, LL_ADC_OVS_RATIO_2, LL_ADC_OVS_SHIFT_NONE);


	/*## Configuration of ADC interruptions ####################################*/
	/* Enable interruption ADC group regular overrun */
	////LL_ADC_EnableIT_OVR(ADC1);

	/* Note: In this example, end of ADC conversions are awaited by polling     */
	/*       (not by interruption).                                             */


	//	########################################################################
	//	POBRANE Z: void Activate_ADC(void)
	//	########################################################################

	////__IO uint32_t wait_loop_index = 0;
	#if (USE_TIMEOUT == 1)
	uint32_t Timeout = 0; /* Variable used for timeout management */
	#endif /* USE_TIMEOUT */

	/*## Operation on ADC hierarchical scope: ADC instance #####################*/

	/* Note: Hardware constraint (refer to description of the functions         */
	/*       below):                                                            */
	/*       On this STM32 serie, setting of these features is conditioned to   */
	/*       ADC state:                                                         */
	/*       ADC must be disabled.                                              */
	/* Note: In this example, all these checks are not necessary but are        */
	/*       implemented anyway to show the best practice usages                */
	/*       corresponding to reference manual procedure.                       */
	/*       Software can be optimized by removing some of these checks, if     */
	/*       they are not relevant considering previous settings and actions    */
	/*       in user application.                                               */
	if (LL_ADC_IsEnabled(ADC1) == 0)
	{
		/* Disable ADC deep power down (enabled by default after reset state) */
		LL_ADC_DisableDeepPowerDown(ADC1);

		/* Enable ADC internal voltage regulator */
		LL_ADC_EnableInternalRegulator(ADC1);

		/* Delay for ADC internal voltage regulator stabilization.                */
		/* Compute number of CPU cycles to wait for, from delay in us.            */
		/* Note: Variable divided by 2 to compensate partially                    */
		/*       CPU processing cycles (depends on compilation optimization).     */
		/* Note: If system core clock frequency is below 200kHz, wait time        */
		/*       is only a few CPU processing cycles.                             */

		/*
		wait_loop_index = ((LL_ADC_DELAY_INTERNAL_REGUL_STAB_US * (SystemCoreClock / (100000 * 2))) / 10);
		while(wait_loop_index != 0)
		{
		wait_loop_index--;
		}
		*/


		delay_ms(300);
		/* Run ADC self calibration */
		LL_ADC_StartCalibration(ADC1, LL_ADC_SINGLE_ENDED);

		/* Poll for ADC effectively calibrated */
		#if (USE_TIMEOUT == 1)
		Timeout = ADC_CALIBRATION_TIMEOUT_MS;
		#endif /* USE_TIMEOUT */

		while (LL_ADC_IsCalibrationOnGoing(ADC1) != 0)
		{
			#if (USE_TIMEOUT == 1)
			/* Check Systick counter flag to decrement the time-out value */
			if (LL_SYSTICK_IsActiveCounterFlag())
			{
				if(Timeout-- == 0)
				{
				/* Time-out occurred. Set LED to blinking mode */
				LED_Blinking(LED_BLINK_ERROR);
				}
			}
			#endif /* USE_TIMEOUT */
		}

		/* Delay between ADC end of calibration and ADC enable.                   */
		/* Note: Variable divided by 2 to compensate partially                    */
		/*       CPU processing cycles (depends on compilation optimization).     */

		/*
		wait_loop_index = (ADC_DELAY_CALIB_ENABLE_CPU_CYCLES >> 1);

		while(wait_loop_index != 0)
		{
			wait_loop_index--;
		}
		*/

		/* Enable ADC */
		LL_ADC_Enable(ADC1);

		/* Poll for ADC ready to convert */
		#if (USE_TIMEOUT == 1)
		Timeout = ADC_ENABLE_TIMEOUT_MS;
		#endif /* USE_TIMEOUT */

		while (LL_ADC_IsActiveFlag_ADRDY(ADC1) == 0)
		{
			#if (USE_TIMEOUT == 1)
			/* Check Systick counter flag to decrement the time-out value */
			if (LL_SYSTICK_IsActiveCounterFlag())
			{
			if(Timeout-- == 0)
			{
			/* Time-out occurred. Set LED to blinking mode */
			LED_Blinking(LED_BLINK_ERROR);
			}
			}
			#endif /* USE_TIMEOUT */
		}

		/* Note: ADC flag ADRDY is not cleared here to be able to check ADC       */
		/*       status afterwards.                                               */
		/*       This flag should be cleared at ADC Deactivation, before a new    */
		/*       ADC activation, using function "LL_ADC_ClearFlag_ADRDY()".       */
	}

	/*## Operation on ADC hierarchical scope: ADC group regular ################*/
	/* Note: No operation on ADC group regular performed here.                  */
	/*       ADC group regular conversions to be performed after this function  */
	/*       using function:                                                    */
	/*       "LL_ADC_REG_StartConversion();"                                    */

	/*## Operation on ADC hierarchical scope: ADC group injected ###############*/
	/* Note: No operation on ADC group injected performed here.                 */
	/*       ADC group injected conversions to be performed after this function */
	/*       using function:                                                    */
	/*       "LL_ADC_INJ_StartConversion();"                                    */






}

uint32_t ADC_LL_POT_COMPUTE(void)
{
	//	########################################################################
	//	POBRANE Z: void ConversionStartPoll_ADC_GrpRegular(void)
	//	########################################################################

	#if (USE_TIMEOUT == 1)
	uint32_t Timeout = 0; /* Variable used for timeout management */
	#endif /* USE_TIMEOUT */

	/* Start ADC group regular conversion */
	/* Note: Hardware constraint (refer to description of the function          */
	/*       below):                                                            */
	/*       On this STM32 serie, setting of this feature is conditioned to     */
	/*       ADC state:                                                         */
	/*       ADC must be enabled without conversion on going on group regular,  */
	/*       without ADC disable command on going.                              */
	/* Note: In this example, all these checks are not necessary but are        */
	/*       implemented anyway to show the best practice usages                */
	/*       corresponding to reference manual procedure.                       */
	/*       Software can be optimized by removing some of these checks, if     */
	/*       they are not relevant considering previous settings and actions    */
	/*       in user application.                                               */

	if ((LL_ADC_IsEnabled(ADC1) == 1) && (LL_ADC_IsDisableOngoing(ADC1) == 0) && (LL_ADC_REG_IsConversionOngoing(ADC1) == 0)   )
	{
		LL_ADC_REG_StartConversion(ADC1);
	}
	else
	{
		/* Error: ADC conversion start could not be performed */
		////LED_Blinking(LED_BLINK_ERROR);
	}

	#if (USE_TIMEOUT == 1)
	Timeout = ADC_UNITARY_CONVERSION_TIMEOUT_MS;
	#endif /* USE_TIMEOUT */

	while (LL_ADC_IsActiveFlag_EOC(ADC1) == 0)
	{
		#if (USE_TIMEOUT == 1)
		/* Check Systick counter flag to decrement the time-out value */
		if (LL_SYSTICK_IsActiveCounterFlag())
		{
		if(Timeout-- == 0)
		{
		/* Time-out occurred. Set LED to blinking mode */
		LED_Blinking(LED_BLINK_SLOW);
	}
	}
	#endif /* USE_TIMEOUT */
	}

	/* Clear flag ADC group regular end of unitary conversion */
	/* Note: This action is not needed here, because flag ADC group regular   */
	/*       end of unitary conversion is cleared automatically when          */
	/*       software reads conversion data from ADC data register.           */
	/*       Nevertheless, this action is done anyway to show how to clear    */
	/*       this flag, needed if conversion data is not always read          */
	/*       or if group injected end of unitary conversion is used (for      */
	/*       devices with group injected available).                          */

	//LL_ADC_ClearFlag_EOC(ADC1);

	uint16_t data = LL_ADC_REG_ReadConversionData12(ADC1);
	uint32_t final_temp;
	return final_temp = __LL_ADC_CALC_DATA_TO_VOLTAGE(VDDA_APPLI, data, LL_ADC_RESOLUTION_12B);





	//uint16_t data = LL_ADC_REG_ReadConversionData12(ADC1);
	return data;
}


//	////////////////////////////////////////////////////////////////////////////
uint16_t ADC_LL_SINGLE_G4(void)
{
	//	-obsługa modułu ADC z użyciem LL,
	//	-podłączony jeden potencjometr do pinu PA0,
	//	-funkcja zawiera pełną konfigurację linii

	//	ADC12_IN1 - PA0
	//	ADC12_IN2 - PA1
	//	ADC12_IN3 - PA2
	//	ADC12_IN4 - PA3
	//	ADC1_IN10 - PF0

	static uint8_t init = 0;

	if(init == 0)
	{
		//	PODPIĘCIA ZEGARÓW:
		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);

		//	Konfiguracja pracy linii w trybie analogowym:
		LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_0, LL_GPIO_MODE_ANALOG);

		//	Podpięcie linii do modułu ADC:
		//LL_GPIO_EnablePinAnalogControl(GPIOA, LL_GPIO_PIN_0);



		//	Wybór zegara i jego ustawienie:
		//LL_ADC_SetCommonClock(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_CLOCK_SYNC_PCLK_DIV2);
		LL_RCC_SetADCClockSource(LL_RCC_ADC12_CLKSOURCE_SYSCLK);	//	ADC12, ADC345 ___ NONE, PLL, SYSCLK,

		//	PODPIĘCE I WYBÓR ZEGARÓW DO OBSŁUGI ADC:
		//	Podłączenie zegara i jego taktowania do ADC:
		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC12);


		//LL_VREFBUF_SetVoltageScaling

		//	Rozdzielczość:
		LL_ADC_SetResolution(ADC1, LL_ADC_RESOLUTION_6B);	//	str. 589

		//	Sposób wyrównywania danych, do prawej czy do lewej:
		LL_ADC_SetDataAlignment(ADC1, LL_ADC_DATA_ALIGN_RIGHT);	//	str. 589

		//	Wybór sposobu wyzwalania konwersji: programowa lub sygnałem zewnętrznym,
		LL_ADC_REG_SetTriggerSource(ADC1, LL_ADC_REG_TRIG_SOFTWARE);

		//	Wybór zbocza wyzwalającego konwersję jeśli została ustawiona na tryb różny od programowego:
		//LL_ADC_REG_SetTriggerEdge(ADC1, LL_ADC_REG_TRIG_EXT_RISING);

		//	Ustawienie trybu konwersji: pojedyncza lub ciągła:
		LL_ADC_REG_SetContinuousMode(ADC1, LL_ADC_REG_CONV_CONTINUOUS);	//	SINGLE, CONTINUOUS, str. 687,

		LL_ADC_REG_SetOverrun(ADC1, LL_ADC_REG_OVR_DATA_OVERWRITTEN);

		//	Ustawienie ilości konwersji w pojedynczej sekwencji:
		LL_ADC_REG_SetSequencerLength(ADC1, LL_ADC_REG_SEQ_SCAN_DISABLE);	//	str. 597,

		//	Wybór kanału do próbkowania oraz jego kolejności:
		LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_1);

		//	Ustawienie czasu próbkowania niezależnie dla każdego kanału:
		LL_ADC_SetChannelSamplingTime(ADC1, ADC_CHANNEL_1_NUMBER, LL_ADC_SAMPLINGTIME_640CYCLES_5);	//	str. 583,

		//	Wyłączenie DPD, aby mogła zajść kalibracja:
		LL_ADC_DisableDeepPowerDown(ADC1);

		//	Włączenie kalibracji, powiązane z wyborem trybu kalibracji:
		LL_ADC_StartCalibration(ADC1, LL_ADC_SINGLE_ENDED);
		while(LL_ADC_IsCalibrationOnGoing(ADC1) != 0);

		//	Włączenie wewnętrznego regulatora napięcia:
		LL_ADC_EnableInternalRegulator(ADC1);
		//	Odczekanie czasu tADCVREG_STUP = 20 [us], DS, str. 173,
		delay_ms(1);

		//	Włączenie ADC bez rozpoczęcia jakichkolwiek konwersji:
		LL_ADC_Enable(ADC1);
		//	Po włączeniu ADC ustawiana jest flaga ADRDY:
		while(LL_ADC_IsActiveFlag_ADRDY(ADC1) == 0);

		init = 1;

	}

	//	Rozpoczęcie konwersji:
	LL_ADC_REG_StartConversion(ADC1);

	//	Po zakończeniu ustawiana jest flaga EOC:
	while(LL_ADC_IsActiveFlag_EOC(ADC1) == 0);

	//	Odczyt danych, wybór funkcji zależy od wybranej rodzielczości:
	uint16_t data = LL_ADC_REG_ReadConversionData32(ADC1);	//	str. 600,
	return data;
}

uint32_t ADC_joystick_G4(void)
{
	//	-obsługa modułu ADC z użyciem LL,
	//	-podłączony jeden potencjometr do pinu PA0,
	//	-funkcja zawiera pełną konfigurację linii

	//	ADC12_IN1 - PA0
	//	ADC12_IN2 - PA1
	//	ADC12_IN3 - PA2
	//	ADC12_IN4 - PA3
	//	ADC1_IN10 - PF0

	static uint8_t init = 0;

	if(init == 0)
	{
		//	PODPIĘCIA ZEGARÓW:
		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);

		//	Konfiguracja pracy linii w trybie analogowym:
		LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_0, LL_GPIO_MODE_ANALOG);
		LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_0, LL_GPIO_PULL_NO);

		LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_12, LL_GPIO_MODE_ANALOG);
		LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_12, LL_GPIO_PULL_NO);

		//	Podpięcie linii do modułu ADC:
		//LL_GPIO_EnablePinAnalogControl(GPIOA, LL_GPIO_PIN_0);



		//	Wybór zegara i jego ustawienie:
		//LL_ADC_SetCommonClock(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_CLOCK_SYNC_PCLK_DIV2);
		LL_RCC_SetADCClockSource(LL_RCC_ADC12_CLKSOURCE_SYSCLK);	//	ADC12, ADC345 ___ NONE, PLL, SYSCLK,

		//	PODPIĘCE I WYBÓR ZEGARÓW DO OBSŁUGI ADC:
		//	Podłączenie zegara i jego taktowania do ADC:
		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC12);


		//LL_VREFBUF_SetVoltageScaling

		//	Rozdzielczość:
		LL_ADC_SetResolution(ADC1, LL_ADC_RESOLUTION_8B);	//	str. 589

		//	Sposób wyrównywania danych, do prawej czy do lewej:
		LL_ADC_SetDataAlignment(ADC1, LL_ADC_DATA_ALIGN_RIGHT);	//	str. 589

		//	Wybór sposobu wyzwalania konwersji: programowa lub sygnałem zewnętrznym,
		LL_ADC_REG_SetTriggerSource(ADC1, LL_ADC_REG_TRIG_SOFTWARE);

		//	Wybór zbocza wyzwalającego konwersję jeśli została ustawiona na tryb różny od programowego:
		//LL_ADC_REG_SetTriggerEdge(ADC1, LL_ADC_REG_TRIG_EXT_RISING);

		//	Ustawienie trybu konwersji: pojedyncza lub ciągła:
		LL_ADC_REG_SetContinuousMode(ADC1, LL_ADC_REG_CONV_SINGLE);	//	SINGLE, CONTINUOUS, str. 687,

		//	Jeśli nie zdążymy odczytać odbieranych danych to:
		//	LL_ADC_REG_OVR_DATA_OVERWRITTEN - nadpisanie danych,
		//	LL_ADC_REG_OVR_DATA_PRESERVED - zachowanie ostatnio nieodebranych,

		LL_ADC_REG_SetOverrun(ADC1, LL_ADC_REG_OVR_DATA_OVERWRITTEN);

		//	LL_ADC_OVS_REG_CONT - jeden triger wyzwala wszystkie pomiary,
		LL_ADC_SetOverSamplingDiscont(ADC1, LL_ADC_OVS_REG_CONT);

		//	####################### KONFIGURACJA KANAŁÓW #######################
		//	Ustawienie ilości konwersji w pojedynczej sekwencji:
		LL_ADC_REG_SetSequencerLength(ADC1, LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS);	//	str. 597,

		//	Wybór kanału do próbkowania oraz jego kolejności:
		LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_1);
		LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_2, LL_ADC_CHANNEL_11);

		//	Ustawienie czasu próbkowania niezależnie dla każdego kanału:
		LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_1, LL_ADC_SAMPLINGTIME_640CYCLES_5);	//	str. 583,
		LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_11, LL_ADC_SAMPLINGTIME_640CYCLES_5);	//	str. 583,

		LL_ADC_SetChannelSingleDiff(ADC1, LL_ADC_CHANNEL_1, LL_ADC_SINGLE_ENDED);
		LL_ADC_SetChannelSingleDiff(ADC1, LL_ADC_CHANNEL_11, LL_ADC_SINGLE_ENDED);
		//	####################################################################

		//	Wybór trybu: niezależny czyli każdy ADCx wykonuje konwersję osobno,
		//	Tryb podwójny: master i slave, jednoczesne konwersje na dwóch instancjach ADC,
		LL_ADC_SetMultimode(ADC12_COMMON, LL_ADC_MULTI_INDEPENDENT);

		//	Działanie jakie zachodzi podczas aktywacji wstrzykiwania,
		LL_ADC_SetOverSamplingScope(ADC1, LL_ADC_OVS_DISABLE);

		//	Wyłączenie DPD, aby mogła zajść kalibracja:
		LL_ADC_DisableDeepPowerDown(ADC1);

		//	Włączenie wewnętrznego regulatora napięcia:
		LL_ADC_EnableInternalRegulator(ADC1);
		//	Odczekanie czasu tADCVREG_STUP = 20 [us], DS, str. 173,
		delay_ms(10);

		//	Włączenie kalibracji, powiązane z wyborem trybu kalibracji:
		LL_ADC_StartCalibration(ADC1, LL_ADC_SINGLE_ENDED);
		while(LL_ADC_IsCalibrationOnGoing(ADC1) != 0);

		//	Włączenie ADC bez rozpoczęcia jakichkolwiek konwersji:
		LL_ADC_Enable(ADC1);
		//	Po włączeniu ADC ustawiana jest flaga ADRDY:
		while(LL_ADC_IsActiveFlag_ADRDY(ADC1) == 0);

		init = 1;
		LL_ADC_ClearFlag_EOC(ADC1);
		LL_ADC_ClearFlag_EOS(ADC1);
	}

	//	Rozpoczęcie konwersji:
	LL_ADC_REG_StartConversion(ADC1);
	//delay_ms(50);

	//	Po zakończeniu ustawiana jest flaga EOC:
	while(LL_ADC_IsActiveFlag_EOC(ADC1) == 0);

	//	Odczyt danych, wybór funkcji zależy od wybranej rodzielczości:
	uint16_t data = LL_ADC_REG_ReadConversionData8(ADC1);	//	str. 600,
	delay_ms(50);

	//LL_ADC_REG_StartConversion(ADC1);
	//LL_ADC_REG_StartConversion(ADC1);
	//	Po zakończeniu ustawiana jest flaga EOC:
	//while(LL_ADC_IsActiveFlag_EOC(ADC1) == 0);

	//	Odczyt danych, wybór funkcji zależy od wybranej rodzielczości:
	uint16_t data_2 = LL_ADC_REG_ReadConversionData8(ADC1);	//	str. 600,

	return data | (data_2<<16);
}

uint32_t ADC_joystick_G4_DMA(void)
{
	//	-obsługa joysticka analogowego czyli dwóch potencjometrów, które zmieniają
	//	rezystację wyjściową w zależności od wychylenia dźwigni joysticka,
	//	-tryb konwersji: ciągły, ilość kanałów: 2, odczytane dane przekazywane poprzez DMA do tablicy,


	static uint8_t init = 0;

	if(init == 0)
	{
		//	PODPIĘCIA ZEGARÓW:
		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);

		//	Konfiguracja pracy linii w trybie analogowym:
		LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_11, LL_GPIO_MODE_ANALOG);
		LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_11, LL_GPIO_PULL_NO);

		LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_12, LL_GPIO_MODE_ANALOG);
		LL_GPIO_SetPinPull(GPIOB, LL_GPIO_PIN_12, LL_GPIO_PULL_NO);

		//	Podpięcie linii do modułu ADC:
		//LL_GPIO_EnablePinAnalogControl(GPIOA, LL_GPIO_PIN_0);



		//	Wybór zegara i jego ustawienie:
		//LL_ADC_SetCommonClock(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_CLOCK_SYNC_PCLK_DIV2);
		LL_RCC_SetADCClockSource(LL_RCC_ADC12_CLKSOURCE_SYSCLK);	//	ADC12, ADC345 ___ NONE, PLL, SYSCLK,

		//	PODPIĘCE I WYBÓR ZEGARÓW DO OBSŁUGI ADC:
		//	Podłączenie zegara i jego taktowania do ADC:
		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC12);


		//LL_VREFBUF_SetVoltageScaling

		//	Rozdzielczość:
		LL_ADC_SetResolution(ADC1, LL_ADC_RESOLUTION_8B);	//	str. 589

		//	Sposób wyrównywania danych, do prawej czy do lewej:
		LL_ADC_SetDataAlignment(ADC1, LL_ADC_DATA_ALIGN_RIGHT);	//	str. 589

		//	Wybór sposobu wyzwalania konwersji: programowa lub sygnałem zewnętrznym,
		LL_ADC_REG_SetTriggerSource(ADC1, LL_ADC_REG_TRIG_SOFTWARE);

		//	Wybór zbocza wyzwalającego konwersję jeśli została ustawiona na tryb różny od programowego:
		//LL_ADC_REG_SetTriggerEdge(ADC1, LL_ADC_REG_TRIG_EXT_RISING);

		//	Ustawienie trybu konwersji: pojedyncza lub ciągła:
		LL_ADC_REG_SetContinuousMode(ADC1, LL_ADC_REG_CONV_CONTINUOUS);	//	SINGLE, CONTINUOUS, str. 687,

		//	Jeśli nie zdążymy odczytać odbieranych danych to:
		//	LL_ADC_REG_OVR_DATA_OVERWRITTEN - nadpisanie danych,
		//	LL_ADC_REG_OVR_DATA_PRESERVED - zachowanie ostatnio nieodebranych,

		LL_ADC_REG_SetOverrun(ADC1, LL_ADC_REG_OVR_DATA_OVERWRITTEN);

		//	LL_ADC_OVS_REG_CONT - jeden triger wyzwala wszystkie pomiary,
		LL_ADC_SetOverSamplingDiscont(ADC1, LL_ADC_OVS_REG_CONT);

		//	####################### KONFIGURACJA KANAŁÓW #######################
		//	Ustawienie ilości konwersji w pojedynczej sekwencji:
		LL_ADC_REG_SetSequencerLength(ADC1, LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS);	//	str. 597,

		//	Wybór kanału do próbkowania oraz jego kolejności:
		LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_14);
		LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_2, LL_ADC_CHANNEL_11);

		//	Ustawienie czasu próbkowania niezależnie dla każdego kanału:
		LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_14, LL_ADC_SAMPLINGTIME_640CYCLES_5);	//	str. 583,
		LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_11, LL_ADC_SAMPLINGTIME_640CYCLES_5);	//	str. 583,

		LL_ADC_SetChannelSingleDiff(ADC1, LL_ADC_CHANNEL_14, LL_ADC_SINGLE_ENDED);
		LL_ADC_SetChannelSingleDiff(ADC1, LL_ADC_CHANNEL_11, LL_ADC_SINGLE_ENDED);
		//	####################################################################

		//	Wybór trybu: niezależny czyli każdy ADCx wykonuje konwersję osobno,
		//	Tryb podwójny: master i slave, jednoczesne konwersje na dwóch instancjach ADC,
		LL_ADC_SetMultimode(ADC12_COMMON, LL_ADC_MULTI_INDEPENDENT);

		//	Działanie jakie zachodzi podczas aktywacji wstrzykiwania,
		LL_ADC_SetOverSamplingScope(ADC1, LL_ADC_OVS_DISABLE);

		//	Wyłączenie DPD, aby mogła zajść kalibracja:
		LL_ADC_DisableDeepPowerDown(ADC1);

		//	Włączenie wewnętrznego regulatora napięcia:
		LL_ADC_EnableInternalRegulator(ADC1);
		//	Odczekanie czasu tADCVREG_STUP = 20 [us], DS, str. 173,
		delay_ms(10);

		//	Włączenie kalibracji, powiązane z wyborem trybu kalibracji:
		LL_ADC_StartCalibration(ADC1, LL_ADC_SINGLE_ENDED);
		while(LL_ADC_IsCalibrationOnGoing(ADC1) != 0);

		//	Włączenie ADC bez rozpoczęcia jakichkolwiek konwersji:
		LL_ADC_Enable(ADC1);
		//	Po włączeniu ADC ustawiana jest flaga ADRDY:
		while(LL_ADC_IsActiveFlag_ADRDY(ADC1) == 0);

		init = 1;
		LL_ADC_ClearFlag_EOC(ADC1);
		LL_ADC_ClearFlag_EOS(ADC1);
	}

	//	Aktywacja transferu danych z ADC poprzez DMA w wybranym trybie:
	LL_ADC_REG_SetDMATransfer(ADC1, LL_ADC_REG_DMA_TRANSFER_UNLIMITED);	//	NONE, LIMITED, UNLIMITED,

	//	Rozpoczęcie konwersji:
	LL_ADC_REG_StartConversion(ADC1);



	return 0;
}

uint16_t ADC_compute_voltage(ADC_t *pADC)
{
	//	-obliczenie wartości napięcia na pinie,

	uint16_t res = 0;


	switch(pADC->resolution)
	{
		case LL_ADC_RESOLUTION_6B:
		{
			res = 64;
			break;
		}
		case LL_ADC_RESOLUTION_8B:
		{
			res = 256;
			break;
		}
		case LL_ADC_RESOLUTION_10B:
		{
			res = 1024;
			break;
		}
		case LL_ADC_RESOLUTION_12B:
		{
			res = 4096;
			break;
		}
	}

	return (pADC->data_received[0] * pADC->V_DDA) / res;
}


//	OLD:

//void ADC_DMA(void);
//uint16_t read_ADC_data[10];


  //#define ADC_DELAY_CALIB_ENABLE_CPU_CYCLES  (LL_ADC_DELAY_CALIB_ENABLE_ADC_CYCLES * 32)
 // #define ADC_UNITARY_CONVERSION_TIMEOUT_MS ((uint32_t)   1)
 // #define VDDA_APPLI                       ((uint32_t)3300)
  /* Init variable out of expected ADC conversion data range */
  //#define VAR_CONVERTED_DATA_INIT_VALUE    (__LL_ADC_DIGITAL_SCALE(LL_ADC_RESOLUTION_12B) + 1)


//__IO uint32_t ubUserButtonPressed = 0;

/* Variables for ADC conversion data */
//__IO uint16_t uhADCxConvertedData = VAR_CONVERTED_DATA_INIT_VALUE; /* ADC group regular conversion data */

/* Variables for ADC conversion data computation to physical values */
//__IO uint16_t hADCxConvertedData_Temperature_DegreeCelsius = 0;  /* Value of temperature calculated from ADC conversion data (unit: degree Celcius) */

//__IO uint8_t ubAdcGrpRegularUnitaryConvStatus = 2; /* Variable set into ADC interruption callback */


/*
void ADC_asm(void)
{
	//	1. Wybór zegara do podpięcia do ADC:
	//LL_RCC_SetADCClockSource(LL_RCC_ADC_CLKSOURCE_SYSCLK);	//	NONE, PLLSAI1, PLLSAI2, SYSCLK,
#ifdef STM32L4
	RCC->CCIPR |= RCC_CCIPR_ADCSEL_1 | RCC_CCIPR_ADCSEL_0; 	//	str 270
#endif
	//	2. Podpięcie zegara do ADC:
#ifdef STM32L4
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;	//	str 251
#endif
	//	3. Włączenie dedykowanego regulatora napięcia ADC: ADC_CR
	ADC1->CR |= ADC_CR_ADVREGEN;			//	str 583

	//	4. Ustawienie rozdzielczości:
	ADC1->CFGR &= ~(ADC_CFGR_RES_0 | ADC_CFGR_RES_1);

	//	5. Ustawienie sposobu wyrównywania danych:
	ADC1->CFGR &= ~(ADC_CFGR_ALIGN);

	//	6. Ustawienie trybu konwersji:
	//LL_ADC_REG_SetContinuousMode(ADC1, LL_ADC_REG_CONV_SINGLE);
	ADC1->CFGR &= ~(ADC_CFGR_CONT);

	//	7. Ustawienie czasu próbkowania dla wybranego kanału:
	ADC1->SMPR1 |= ADC_SMPR1_SMP4_0 | ADC_SMPR1_SMP4_1 | ADC_SMPR1_SMP4_2;


	//	włączenie autokalibracji ADC, ustawiany programowo, bit znajduje się w stanie wysokim dopuki trwa kalibracja,
	//	bit jest zerowany po zakończeniu kalibracji,
	//	program zezwala na odpalenie kalibracji poprzez ustawienie ADCAL tylko jeśli ADEN = 0,
	ADC1->CR |= ADC_CR_ADCAL;
	while(ADC1->CR & ADC_CR_ADCAL);




	//	9. Włączenie kontroli nad ADC:
	ADC1->CR |= ADC_CR_ADEN;	//	str 585,

	//	10. Wybór kanału:
	//	Należy sprawdzić w datasheecie np str. 74, funkcja dodatkowa: PA4 --- ADC12_IN9,
	ADC1->SQR1 |= ADC_SQR1_SQ1_2;





}

*/


/*
uint16_t ADC_LL_1_SINGLE(void)
{
	//	-obsługa modułu ADC z użyciem LL,
	//	-podłączony jeden potencjometr do pinu PC0,

	static uint8_t init = 0;

	if(init == 0)
	{
		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);

		LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_0, LL_GPIO_MODE_ANALOG);

		//	Podpięcie linii do modułu ADC:
		LL_GPIO_EnablePinAnalogControl(GPIOC, LL_GPIO_PIN_0);

		//	Podłączenie zegara i jego taktowania do ADC:
#ifdef STM32L4
		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC);
#endif
		//	Wybór zegara i jego ustawienie:
		//LL_ADC_SetCommonClock(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_CLOCK_SYNC_PCLK_DIV2);
#ifdef STM32L4
		LL_RCC_SetADCClockSource(LL_RCC_ADC_CLKSOURCE_SYSCLK);	//	NONE, PLLSAI1, PLLSAI2, SYSCLK,
#endif
		//	Rozdzielczość:
		LL_ADC_SetResolution(ADC1, LL_ADC_RESOLUTION_12B);	//	str. 589

		//	Sposób wyrównywania danych, do prawej czy do lewej:
		LL_ADC_SetDataAlignment(ADC1, LL_ADC_DATA_ALIGN_RIGHT);	//	str. 589

		//	Wybór sposobu wyzwalania konwersji:
		LL_ADC_REG_SetTriggerSource(ADC1, LL_ADC_REG_TRIG_SOFTWARE);

		//	Ustawienie trybu konwersji: pojedyncza lub ciągła:
		LL_ADC_REG_SetContinuousMode(ADC1, LL_ADC_REG_CONV_SINGLE);	//	SINGLE, CONTINUOUS, str. 588,

		LL_ADC_REG_SetOverrun(ADC1, LL_ADC_REG_OVR_DATA_OVERWRITTEN);

		//	Ustawienie ilości konwersji w pojedynczej sekwencji:
		LL_ADC_REG_SetSequencerLength(ADC1, LL_ADC_REG_SEQ_SCAN_DISABLE);	//	str. 597,

		//	Wybór kanału do próbkowania oraz jego kolejności:
		LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_1);

		//	Ustawienie czasu próbkowania niezależnie dla każdego kanału:
		LL_ADC_SetChannelSamplingTime(ADC1, ADC_CHANNEL_1_NUMBER, LL_ADC_SAMPLINGTIME_640CYCLES_5);	//	str. 583,

		//	Wyłączenie DPD, aby mogła zajść kalibracja:
		LL_ADC_DisableDeepPowerDown(ADC1);

		//	Włączenie kalibracji, powiązane z wyborem trybu kalibracji:
		LL_ADC_StartCalibration(ADC1, LL_ADC_SINGLE_ENDED);
		while(LL_ADC_IsCalibrationOnGoing(ADC1) != 0);

		//	Włączenie wewnętrznego regulatora napięcia:
		LL_ADC_EnableInternalRegulator(ADC1);
		//	Odczekanie czasu tADCVREG_STUP = 20 [us], DS, str. 173,
		delay_ms(1);

		//	Włączenie ADC bez rozpoczęcia jakichkolwiek konwersji:
		LL_ADC_Enable(ADC1);
		//	Po włączeniu ADC ustawiana jest flaga ADRDY:
		while(LL_ADC_IsActiveFlag_ADRDY(ADC1) == 0);

		init = 1;

	}

	//	Rozpoczęcie konwersji:
	LL_ADC_REG_StartConversion(ADC1);

	//	Po zakończeniu ustawiana jest flaga EOC:
	while(LL_ADC_IsActiveFlag_EOC(ADC1) == 0);

	//	Odczyt danych, wybór funkcji zależy od wybranej rodzielczości:
	uint16_t data = LL_ADC_REG_ReadConversionData12(ADC1);	//	str. 600,
	return data;
}

*/

/*
uint16_t ADC_LL_1_continuous(void)
{
	//	ADC z użyciem LL:

	static uint8_t init = 0;

	if(init == 0)
	{

		//	Podłączenie zegara i jego taktowania do ADC:
#ifdef STM32L4
		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC);
#endif
		//	Wybór zegara i jego ustawienie:
		//LL_ADC_SetCommonClock(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_CLOCK_SYNC_PCLK_DIV2);
#ifdef STM32L4
		LL_RCC_SetADCClockSource(LL_RCC_ADC_CLKSOURCE_PLLSAI1);	//	NONE, PLLSAI1, PLLSAI2, SYSCLK,
#endif
		LL_ADC_SetCommonClock(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_CLOCK_SYNC_PCLK_DIV4);

		//	Rozdzielczość:
		LL_ADC_SetResolution(ADC1, LL_ADC_RESOLUTION_8B);	//	str. 589

		//	Sposób wyrównywania danych, do prawej czy do lewej:
		LL_ADC_SetDataAlignment(ADC1, LL_ADC_DATA_ALIGN_RIGHT);	//	str. 589

		//	Wybór sposobu wyzwalania konwersji:
		LL_ADC_REG_SetTriggerSource(ADC1, LL_ADC_REG_TRIG_SOFTWARE);

		//	Ustawienie trybu konwersji: pojedyncza lub ciągła:
		LL_ADC_REG_SetContinuousMode(ADC1, LL_ADC_REG_CONV_CONTINUOUS);	//	SINGLE, CONTINUOUS, str. 588,

		LL_ADC_REG_SetOverrun(ADC1, LL_ADC_REG_OVR_DATA_OVERWRITTEN);

		//	Ustawienie ilości konwersji w pojedynczej sekwencji:
		LL_ADC_REG_SetSequencerLength(ADC1, LL_ADC_REG_SEQ_SCAN_DISABLE);	//	str. 597,

		//	Wybór kanału do próbkowania oraz jego kolejności:
		LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_1);

		//	Ustawienie czasu próbkowania niezależnie dla każdego kanału:
		//LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_1, LL_ADC_SAMPLINGTIME_640CYCLES_5);	//	str. 583,
		LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_1, LL_ADC_SAMPLINGTIME_640CYCLES_5);	//	str. 583,

		//	Wyłączenie DPD, aby mogła zajść kalibracja:
		LL_ADC_DisableDeepPowerDown(ADC1);

		//	Włączenie wewnętrznego regulatora napięcia:
		LL_ADC_EnableInternalRegulator(ADC1);
		//	Odczekanie czasu tADCVREG_STUP = 20 [us], DS, str. 173,
		delay_ms(1);

		//	Włączenie kalibracji, powiązane z wyborem trybu kalibracji:
		LL_ADC_StartCalibration(ADC1, LL_ADC_SINGLE_ENDED);
		while(LL_ADC_IsCalibrationOnGoing(ADC1) != 0);

		//	Włączenie ADC bez rozpoczęcia jakichkolwiek konwersji:
		LL_ADC_Enable(ADC1);
		//	Po włączeniu ADC ustawiana jest flaga ADRDY:
		while(LL_ADC_IsActiveFlag_ADRDY(ADC1) == 0);

		NVIC_SetPriority(ADC1_2_IRQn, 1);
		NVIC_EnableIRQ(ADC1_2_IRQn);

		LL_ADC_EnableIT_EOS(ADC1);
		LL_ADC_EnableIT_EOC(ADC1);
		LL_ADC_EnableIT_OVR(ADC1);

		init = 1;

		LL_ADC_REG_StartConversion(ADC1);
	}

	//	Rozpoczęcie konwersji:
	//LL_ADC_REG_StartConversion(ADC1);
	//	Po zakończeniu ustawiana jest flaga EOC:




	return 0;
}

*/

/*
ADC_t ADC_11 =
{
	.pADCx = ADC1,
#ifdef STM32L4
	.enable_periph = LL_AHB2_GRP1_PERIPH_ADC,
#endif
	.resolution = LL_ADC_RESOLUTION_8B,			//	12B, 10B, 8B, 6B,
	.data_alignment = LL_ADC_DATA_ALIGN_RIGHT,	//	RIGHT, LEFT
	.trigger_source = LL_ADC_REG_TRIG_SOFTWARE,	//	SOFTWARE, ...
	.mode = LL_ADC_REG_CONV_CONTINUOUS,			//	SINGLE, CONTINUOUS,

	.sequencer_length = LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS,	//	DISABLE, ENABLE_xRANKS - x=<2, 16>,

	.rank[0] = LL_ADC_REG_RANK_1,
	.channel[0] = LL_ADC_CHANNEL_1,				//	0...18, VREFINT, TEMPSENSOR, VBAT,
	.sampling_time[0] = LL_ADC_SAMPLINGTIME_640CYCLES_5,	//	2, 6, 12, 24, 47, 92, 247, 940,

	.rank[1] = LL_ADC_REG_RANK_2,
	.channel[1] = LL_ADC_CHANNEL_2,				//	0...18, VREFINT, TEMPSENSOR, VBAT,
	.sampling_time[1] = LL_ADC_SAMPLINGTIME_640CYCLES_5,	//	2, 6, 12, 24, 47, 92, 247, 940,

	.enable_IT_EOC = 1,
	.enable_IT_EOS = 0,
	.enable_IT_OVR = 0,
};

*/

void ADC_LL_2_SINGLE(uint16_t data[])
{
	//	ADC z użyciem LL:

	static uint8_t init = 0;

	if(init == 0)
	{
		/*
		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);

		// Configure GPIO in analog mode to be used as ADC input
		LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_0, LL_GPIO_MODE_ANALOG);
		LL_GPIO_SetPinMode(GPIOC, LL_GPIO_PIN_1, LL_GPIO_MODE_ANALOG);

		//	Podpięcie linii do modułu ADC:
		LL_GPIO_EnablePinAnalogControl(GPIOC, LL_GPIO_PIN_0);
		LL_GPIO_EnablePinAnalogControl(GPIOC, LL_GPIO_PIN_1);
*/
		//	Podłączenie zegara i jego taktowania do ADC:
#ifdef STM32L4
		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC);
#endif
		//	Wybór zegara i jego ustawienie:
		//LL_ADC_SetCommonClock(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_CLOCK_SYNC_PCLK_DIV2);
#ifdef STM32L4
		LL_RCC_SetADCClockSource(LL_RCC_ADC_CLKSOURCE_SYSCLK);	//	NONE, PLLSAI1, PLLSAI2, SYSCLK,
#endif
		//	Rozdzielczość:
		LL_ADC_SetResolution(ADC1, LL_ADC_RESOLUTION_8B);	//	str. 589

		//	Sposób wyrównywania danych, do prawej czy do lewej:
		LL_ADC_SetDataAlignment(ADC1, LL_ADC_DATA_ALIGN_RIGHT);	//	str. 589

		//	Wybór sposobu wyzwalania konwersji:
		LL_ADC_REG_SetTriggerSource(ADC1, LL_ADC_REG_TRIG_SOFTWARE);

		//	Ustawienie trybu konwersji: pojedyncza lub ciągła:
		LL_ADC_REG_SetContinuousMode(ADC1, LL_ADC_REG_CONV_SINGLE);	//	SINGLE, CONTINUOUS, str. 588,

		//	Ustawienie ilości konwersji w pojedynczej sekwencji:
		LL_ADC_REG_SetSequencerLength(ADC1, LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS);	//	str. 597,

		//	Wybór kanału do próbkowania oraz jego kolejności:
		LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_1);
		LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_2, LL_ADC_CHANNEL_2);

		//	Ustawienie czasu próbkowania niezależnie dla każdego kanału:
		LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_1, LL_ADC_SAMPLINGTIME_247CYCLES_5);	//	str. 583,
		LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_2, LL_ADC_SAMPLINGTIME_247CYCLES_5);	//	str. 583,

		//	Wyłączenie DPD, aby mogła zajść kalibracja:
		LL_ADC_DisableDeepPowerDown(ADC1);

		//	Włączenie kalibracji, powiązane z wyborem trybu kalibracji:
		LL_ADC_StartCalibration(ADC1, LL_ADC_SINGLE_ENDED);
		while(LL_ADC_IsCalibrationOnGoing(ADC1) != 0);

		//	Włączenie wewnętrznego regulatora napięcia:
		LL_ADC_EnableInternalRegulator(ADC1);
		//	Odczekanie czasu tADCVREG_STUP = 20 [us], DS, str. 173,
		delay_ms(1);

		//	Włączenie ADC bez rozpoczęcia jakichkolwiek konwersji:
		LL_ADC_Enable(ADC1);
		//	Po włączeniu ADC ustawiana jest flaga ADRDY:
		while(LL_ADC_IsActiveFlag_ADRDY(ADC1) == 0);


		NVIC_SetPriority(ADC1_2_IRQn, 1);
		NVIC_EnableIRQ(ADC1_2_IRQn);

		//LL_ADC_EnableIT_EOS(ADC1);
		//LL_ADC_EnableIT_EOC(ADC1);
		//LL_ADC_EnableIT_OVR(ADC1);

		init = 1;

		//LL_ADC_REG_StartConversion(ADC1);
	}

	//	Rozpoczęcie konwersji:
	LL_ADC_REG_StartConversion(ADC1);

	///*
	//	Po zakończeniu ustawiana jest flaga EOC:
	while(LL_ADC_IsActiveFlag_EOC(ADC1) == 0);
	//	Odczyt danych, wybór funkcji zależy od wybranej rodzielczości
	data[0] = LL_ADC_REG_ReadConversionData8(ADC1);	//	str. 600,

	while(LL_ADC_IsActiveFlag_EOC(ADC1) == 0);
	data[1] = LL_ADC_REG_ReadConversionData8(ADC1);	//	str. 600,
	//*/
}

void ADC_LL_2_continuous(uint16_t data[])
{
	//	ADC z użyciem LL:

	static uint8_t init = 0;

	if(init == 0)
	{
		//	Podłączenie zegara i jego taktowania do ADC:
#ifdef STM32L4
		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC);
#endif
		//	Wybór zegara i jego ustawienie:
		//LL_ADC_SetCommonClock(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_CLOCK_SYNC_PCLK_DIV2);
#ifdef STM32L4
		LL_RCC_SetADCClockSource(LL_RCC_ADC_CLKSOURCE_PLLSAI1);	//	NONE, PLLSAI1, PLLSAI2, SYSCLK,
#endif
		LL_ADC_SetCommonClock(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_CLOCK_SYNC_PCLK_DIV4);

		//	Rozdzielczość:
		LL_ADC_SetResolution(ADC1, LL_ADC_RESOLUTION_8B);	//	str. 589

		//	Sposób wyrównywania danych, do prawej czy do lewej:
		LL_ADC_SetDataAlignment(ADC1, LL_ADC_DATA_ALIGN_RIGHT);	//	str. 589

		//	Wybór sposobu wyzwalania konwersji:
		LL_ADC_REG_SetTriggerSource(ADC1, LL_ADC_REG_TRIG_SOFTWARE);

		//	Ustawienie trybu konwersji: pojedyncza lub ciągła:
		LL_ADC_REG_SetContinuousMode(ADC1, LL_ADC_REG_CONV_CONTINUOUS);	//	SINGLE, CONTINUOUS, str. 588,
		//LL_ADC_REG_SetContinuousMode(ADC1, LL_ADC_REG_CONV_SINGLE);	//	SINGLE, CONTINUOUS, str. 588,

		//LL_ADC_REG_SetOverrun(ADC1, LL_ADC_REG_OVR_DATA_OVERWRITTEN);

		//	Ustawienie ilości konwersji w pojedynczej sekwencji:
		//LL_ADC_REG_SetSequencerLength(ADC1, LL_ADC_REG_SEQ_SCAN_DISABLE);	//	str. 597,
		LL_ADC_REG_SetSequencerLength(ADC1, LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS);	//	str. 597,

		//	Wybór kanału do próbkowania oraz jego kolejności:
		LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_1);
		LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_2, LL_ADC_CHANNEL_2);

		//	Ustawienie czasu próbkowania niezależnie dla każdego kanału:
		LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_1, LL_ADC_SAMPLINGTIME_640CYCLES_5);	//	str. 583,
		LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_2, LL_ADC_SAMPLINGTIME_640CYCLES_5);	//	str. 583,

		LL_ADC_SetChannelSingleDiff(ADC1, LL_ADC_CHANNEL_1, LL_ADC_SINGLE_ENDED);
		LL_ADC_SetChannelSingleDiff(ADC1, LL_ADC_CHANNEL_2, LL_ADC_SINGLE_ENDED);

		//LL_ADC_SetChannelSamplingTime(ADC1, ADC_CHANNEL_2_NUMBER, LL_ADC_SAMPLINGTIME_92CYCLES_5);	//	str. 583,

		//	Wyłączenie DPD, aby mogła zajść kalibracja:
		LL_ADC_DisableDeepPowerDown(ADC1);

		//	Włączenie wewnętrznego regulatora napięcia:
		LL_ADC_EnableInternalRegulator(ADC1);
		//	Odczekanie czasu tADCVREG_STUP = 20 [us], DS, str. 173,
		delay_ms(1);

		//	Włączenie kalibracji, powiązane z wyborem trybu kalibracji:
		LL_ADC_StartCalibration(ADC1, LL_ADC_SINGLE_ENDED);
		while(LL_ADC_IsCalibrationOnGoing(ADC1) != 0);

		//	Włączenie ADC bez rozpoczęcia jakichkolwiek konwersji:
		LL_ADC_Enable(ADC1);
		//	Po włączeniu ADC ustawiana jest flaga ADRDY:
		while(LL_ADC_IsActiveFlag_ADRDY(ADC1) == 0);

		init = 1;
/*
		NVIC_SetPriority(ADC1_2_IRQn, 1);
		NVIC_EnableIRQ(ADC1_2_IRQn);

		LL_ADC_EnableIT_EOS(ADC1);
		LL_ADC_EnableIT_EOC(ADC1);
		LL_ADC_EnableIT_OVR(ADC1);
*/
		//DMA_first();
		////////////////////////////////////////////////DMA_ADC1_init(&DMA_ADC_1);

		LL_ADC_REG_StartConversion(ADC1);
	}

	//	Rozpoczęcie konwersji:
	//LL_ADC_REG_StartConversion(ADC1);
	//	Po zakończeniu ustawiana jest flaga EOC:



/*
	data[0] = LL_ADC_REG_ReadConversionData8(ADC1);	//	str. 600,
	data[0] = LL_ADC_REG_ReadConversionData8(ADC1);	//	str. 600,
	while(LL_ADC_IsActiveFlag_OVR(ADC1) == 0);
	LL_ADC_ClearFlag_OVR(ADC1);
	*/

	/*
	while(LL_ADC_IsActiveFlag_EOS(ADC1) == 0);
	LL_ADC_ClearFlag_EOS(ADC1);
	while(LL_ADC_IsActiveFlag_EOS(ADC1) == 0);
	LL_ADC_ClearFlag_EOS(ADC1);

	data[0] = LL_ADC_REG_ReadConversionData8(ADC1);	//	str. 600,
	while(LL_ADC_IsActiveFlag_OVR(ADC1) == 0);
	LL_ADC_ClearFlag_OVR(ADC1);


	while(LL_ADC_IsActiveFlag_EOC(ADC1) == 0);
	//	Odczyt danych, wybór funkcji zależy od wybranej rodzielczości
	data[0] = LL_ADC_REG_ReadConversionData8(ADC1);	//	str. 600,

	while(LL_ADC_IsActiveFlag_EOC(ADC1) == 0);
	data[1] = LL_ADC_REG_ReadConversionData8(ADC1);	//	str. 600,
	*/
}



