/*
 * RC5.c
 *
 *  Created on: 7 lut 2025
 *      Author: asmnop
 */


#include "../../LIB/Inc/RC5.h"


/*
 * UWAGI:
 * -zwrócić uwagę na to kiedy przyjmujemy, że odebraliśmy bit wyskoki a kiedy bit niski,
 */



void RC5_init(void)
{
	/*
	 * Ustawienie timera TIM15_CH2 do wykonywania przechwytywania wartości zegara,
	 * Na początku wykonano bazową konfigurację zegara TIM15:
	 * SYS_CORE_CLOCK = 10 000 000
	 * preskal = 10 000
	 * ARR = 0xFFffFFff
	 * KONFIGURACJA DODATKOWA:
	 * -ustawienie żródła sygnału generującego przechwycenie,
	 * -ustawienie zbocza sygnału, które powoduje przechwycenie,
	 * -preskal oznaczający co które przechwycenie będzie zapisywane do CCRx,
	 * -ustawienie filtra wejściowego, czyli ilości taktów zegara ze stanem stabilnym,
	 * po których zajściu jest generowany stan na wejściu,
	 * -liczba taktów związana jest z bitem CKD w CR1
	 *
	 */

	LL_DBGMCU_APB2_GRP1_FreezePeriph(LL_DBGMCU_APB2_GRP1_TIM15_STOP);

	LL_APB2_GRP1_EnableClock    (LL_APB2_GRP1_PERIPH_TIM15);
	LL_TIM_EnableARRPreload     (TIM15);
	LL_TIM_SetClockSource       (TIM15, LL_TIM_CLOCKSOURCE_INTERNAL);
	LL_TIM_SetCounterMode       (TIM15, LL_TIM_COUNTERMODE_UP);
	LL_TIM_SetPrescaler         (TIM15, 20000 - 1);
	LL_TIM_SetAutoReload        (TIM15, UINT32_MAX);


	LL_TIM_SetClockDivision     (TIM15, LL_TIM_CLOCKDIVISION_DIV1);	//	Dzielnik sygnału wejściowego CK_INT

	LL_TIM_IC_SetFilter         (TIM15, LL_TIM_CHANNEL_CH2, LL_TIM_IC_FILTER_FDIV1_N8);		//	Filtr,
	LL_TIM_IC_SetActiveInput    (TIM15, LL_TIM_CHANNEL_CH2, LL_TIM_ACTIVEINPUT_DIRECTTI);	//	Źródło sygnału sterującego,
	LL_TIM_IC_SetPolarity       (TIM15, LL_TIM_CHANNEL_CH2, LL_TIM_IC_POLARITY_FALLING);	//	Zbocze aktywujące,
	LL_TIM_IC_SetPrescaler      (TIM15, LL_TIM_CHANNEL_CH2, LL_TIM_ICPSC_DIV1);	//	Co która zgodność zbocza aktywującego powoduje przechwycenie,
	LL_TIM_CC_EnableChannel     (TIM15, LL_TIM_CHANNEL_CH2);	//	Zgoda na przechwycenie,

	LL_TIM_EnableAllOutputs     (TIM15);	//	!!!!!!!!!!!!!!!!!!!!

	LL_TIM_GenerateEvent_UPDATE (TIM15);
	LL_TIM_ClearFlag_UPDATE     (TIM15);
	//LL_TIM_EnableIT_UPDATE      (TIM15);
	LL_TIM_EnableIT_CC2         (TIM15);	//	Włączenie przerwania od aktualizacji CCRx
	LL_TIM_EnableCounter        (TIM15);

	/*
	 *
	 *
	 * 	if(LL_TIM_IsActiveFlag_CC2(TIM15) == 1)
		{
			LL_TIM_ClearFlag_CC2(TIM15);
			push_diff_time_ms = LL_TIM_IC_GetCaptureCH2(TIM15);
			LL_TIM_SetCounter(TIM15, 0UL);
			VT100_number(push_diff_time_ms);
			VT100_EOL();
		}
	 */

}

void RC5_init_2(void)
{
	/*
	 * Ustawienie timera TIM15_CH2 do wykonywania przechwytywania wartości zegara,
	 * Na początku wykonano bazową konfigurację zegara TIM15:
	 * SYS_CORE_CLOCK = 10 000 000
	 * preskal = 10 000
	 * ARR = 0xFFffFFff
	 * KONFIGURACJA DODATKOWA:
	 * -ustawienie żródła sygnału generującego przechwycenie,
	 * -ustawienie zbocza sygnału, które powoduje przechwycenie,
	 * -preskal oznaczający co które przechwycenie będzie zapisywane do CCRx,
	 * -ustawienie filtra wejściowego, czyli ilości taktów zegara ze stanem stabilnym,
	 * po których zajściu jest generowany stan na wejściu,
	 * -liczba taktów związana jest z bitem CKD w CR1
	 *
	 * INFORMACJE DODATKOWE:
	 * -w tej konfiguracji występuje niebezpośrednie podłączenie CH1 tzn. sygnał
	 * który jest dostarczany do CH2 jest dopiero wewnętrznie podłączony do CH1
	 * za pomocą funkcji 'LL_TIM_IC_SetActiveInput(TIM15, LL_TIM_CHANNEL_CH1, LL_TIM_ACTIVEINPUT_INDIRECTTI);'
	 *
	 */

	LL_DBGMCU_APB2_GRP1_FreezePeriph(LL_DBGMCU_APB2_GRP1_TIM15_STOP);

	LL_APB2_GRP1_EnableClock    (LL_APB2_GRP1_PERIPH_TIM15);
	LL_TIM_EnableARRPreload     (TIM15);
	LL_TIM_SetClockSource       (TIM15, LL_TIM_CLOCKSOURCE_INTERNAL);
	LL_TIM_SetCounterMode       (TIM15, LL_TIM_COUNTERMODE_UP);
	LL_TIM_SetPrescaler         (TIM15, 20000 - 1);
	LL_TIM_SetAutoReload        (TIM15, UINT16_MAX);


	LL_TIM_SetClockDivision     (TIM15, LL_TIM_CLOCKDIVISION_DIV1);	//	Dzielnik sygnału wejściowego CK_INT

	LL_TIM_IC_SetFilter         (TIM15, LL_TIM_CHANNEL_CH1, LL_TIM_IC_FILTER_FDIV1);		//	Filtr,
	LL_TIM_IC_SetActiveInput    (TIM15, LL_TIM_CHANNEL_CH1, LL_TIM_ACTIVEINPUT_INDIRECTTI);	//	Źródło sygnału sterującego,
	LL_TIM_IC_SetPolarity       (TIM15, LL_TIM_CHANNEL_CH1, LL_TIM_IC_POLARITY_RISING);	//	Zbocze aktywujące,
	LL_TIM_IC_SetPrescaler      (TIM15, LL_TIM_CHANNEL_CH1, LL_TIM_ICPSC_DIV1);	//	Co która zgodność zbocza aktywującego powoduje przechwycenie,
	LL_TIM_CC_EnableChannel     (TIM15, LL_TIM_CHANNEL_CH1);	//	Zgoda na przechwycenie,

	LL_TIM_IC_SetFilter         (TIM15, LL_TIM_CHANNEL_CH2, LL_TIM_IC_FILTER_FDIV1);		//	Filtr,
	LL_TIM_IC_SetActiveInput    (TIM15, LL_TIM_CHANNEL_CH2, LL_TIM_ACTIVEINPUT_DIRECTTI);	//	Źródło sygnału sterującego,
	LL_TIM_IC_SetPolarity       (TIM15, LL_TIM_CHANNEL_CH2, LL_TIM_IC_POLARITY_FALLING);	//	Zbocze aktywujące,
	LL_TIM_IC_SetPrescaler      (TIM15, LL_TIM_CHANNEL_CH2, LL_TIM_ICPSC_DIV1);	//	Co która zgodność zbocza aktywującego powoduje przechwycenie,
	LL_TIM_CC_EnableChannel     (TIM15, LL_TIM_CHANNEL_CH2);	//	Zgoda na przechwycenie,

	//LL_TIM_SetTriggerInput      (TIM15, LL_TIM_TS_TI2FP2);
	//LL_TIM_SetSlaveMode         (TIM15, LL_TIM_SLAVEMODE_RESET);

	LL_TIM_EnableAllOutputs     (TIM15);	//	!!!!!!!!!!!!!!!!!!!!

	LL_TIM_GenerateEvent_UPDATE (TIM15);
	LL_TIM_ClearFlag_UPDATE     (TIM15);
	//LL_TIM_EnableIT_UPDATE      (TIM15);
	LL_TIM_EnableIT_CC1         (TIM15);	//	Włączenie przerwania od aktualizacji CCR1
	LL_TIM_EnableIT_CC2         (TIM15);	//	Włączenie przerwania od aktualizacji CCR2
	LL_TIM_EnableCounter        (TIM15);

	/*
	 *
	 *
	 * 	if(LL_TIM_IsActiveFlag_CC2(TIM15) == 1)
		{
			LL_TIM_ClearFlag_CC2(TIM15);
			push_diff_time_ms = LL_TIM_IC_GetCaptureCH2(TIM15);
			LL_TIM_SetCounter(TIM15, 0UL);
			VT100_number(push_diff_time_ms);
			VT100_EOL();
		}
	 */

}

void RC5_init_3(void)
{
	/*
	 * Ustawienie timera TIM15_CH2 do wykonywania przechwytywania wartości zegara,
	 * Na początku wykonano bazową konfigurację zegara TIM15:
	 * SYS_CORE_CLOCK = 10 000 000
	 * preskal = 100
	 * ARR = 0xFFff
	 * KONFIGURACJA DODATKOWA:
	 * -ustawienie żródła sygnału generującego przechwycenie,
	 * -ustawienie zbocza sygnału, które powoduje przechwycenie,
	 * -preskal oznaczający co które przechwycenie będzie zapisywane do CCRx,
	 * -ustawienie filtra wejściowego, czyli ilości taktów zegara ze stanem stabilnym,
	 * po których zajściu jest generowany stan na wejściu,
	 * -liczba taktów związana jest z bitem CKD w CR1
	 *
	 * INFORMACJE DODATKOWE:
	 * -w tej konfiguracji występuje niebezpośrednie podłączenie CH1 tzn. sygnał
	 * który jest dostarczany do CH2 jest dopiero wewnętrznie podłączony do CH1
	 * za pomocą funkcji 'LL_TIM_IC_SetActiveInput(TIM15, LL_TIM_CHANNEL_CH1, LL_TIM_ACTIVEINPUT_INDIRECTTI);'
	 *
	 */

	LL_DBGMCU_APB2_GRP1_FreezePeriph(LL_DBGMCU_APB2_GRP1_TIM15_STOP);

	LL_APB2_GRP1_EnableClock    (LL_APB2_GRP1_PERIPH_TIM15);
	LL_TIM_EnableARRPreload     (TIM15);
	LL_TIM_SetClockSource       (TIM15, LL_TIM_CLOCKSOURCE_INTERNAL);
	LL_TIM_SetCounterMode       (TIM15, LL_TIM_COUNTERMODE_UP);
	LL_TIM_SetPrescaler         (TIM15, 200 - 1);
	LL_TIM_SetAutoReload        (TIM15, UINT16_MAX);


	LL_TIM_SetClockDivision     (TIM15, LL_TIM_CLOCKDIVISION_DIV1);	//	Dzielnik sygnału wejściowego CK_INT

	LL_TIM_IC_SetFilter         (TIM15, LL_TIM_CHANNEL_CH1, LL_TIM_IC_FILTER_FDIV1);		//	Filtr,
	LL_TIM_IC_SetActiveInput    (TIM15, LL_TIM_CHANNEL_CH1, LL_TIM_ACTIVEINPUT_INDIRECTTI);	//	Źródło sygnału sterującego,
	LL_TIM_IC_SetPolarity       (TIM15, LL_TIM_CHANNEL_CH1, LL_TIM_IC_POLARITY_RISING);		//	Zbocze aktywujące,
	LL_TIM_IC_SetPrescaler      (TIM15, LL_TIM_CHANNEL_CH1, LL_TIM_ICPSC_DIV1);				//	Co która zgodność zbocza aktywującego powoduje przechwycenie,
	LL_TIM_CC_EnableChannel     (TIM15, LL_TIM_CHANNEL_CH1);								//	Zgoda na przechwycenie,

	LL_TIM_IC_SetFilter         (TIM15, LL_TIM_CHANNEL_CH2, LL_TIM_IC_FILTER_FDIV1);		//	Filtr,
	LL_TIM_IC_SetActiveInput    (TIM15, LL_TIM_CHANNEL_CH2, LL_TIM_ACTIVEINPUT_DIRECTTI);	//	Źródło sygnału sterującego,
	LL_TIM_IC_SetPolarity       (TIM15, LL_TIM_CHANNEL_CH2, LL_TIM_IC_POLARITY_FALLING);	//	Zbocze aktywujące,
	LL_TIM_IC_SetPrescaler      (TIM15, LL_TIM_CHANNEL_CH2, LL_TIM_ICPSC_DIV1);				//	Co która zgodność zbocza aktywującego powoduje przechwycenie,
	LL_TIM_CC_EnableChannel     (TIM15, LL_TIM_CHANNEL_CH2);								//	Zgoda na przechwycenie,

	//LL_TIM_SetTriggerInput      (TIM15, LL_TIM_TS_TI2FP2);
	//LL_TIM_SetSlaveMode         (TIM15, LL_TIM_SLAVEMODE_RESET);

	LL_TIM_EnableAllOutputs     (TIM15);	//	!!!!!!!!!!!!!!!!!!!!

	LL_TIM_GenerateEvent_UPDATE (TIM15);
	LL_TIM_ClearFlag_UPDATE     (TIM15);
	//LL_TIM_EnableIT_UPDATE      (TIM15);
	LL_TIM_EnableIT_CC1         (TIM15);	//	Włączenie przerwania od aktualizacji CCR1
	LL_TIM_EnableIT_CC2         (TIM15);	//	Włączenie przerwania od aktualizacji CCR2
	LL_TIM_EnableCounter        (TIM15);

	/*
	 *
	 *
	 * 	if(LL_TIM_IsActiveFlag_CC2(TIM15) == 1)
		{
			LL_TIM_ClearFlag_CC2(TIM15);
			push_diff_time_ms = LL_TIM_IC_GetCaptureCH2(TIM15);
			LL_TIM_SetCounter(TIM15, 0UL);
			VT100_number(push_diff_time_ms);
			VT100_EOL();
		}
	 */

}


void RC5_new_data(RC5_t *pRC5, const uint16_t data_frame)
{
	//	-zapis nowo odebranej ramki danych do tablicy z odebranymi ramkami,
	//	-nowa ramka danych jest zapisywana do komórki o największym numerze,
	//	-podczas zapisu w tablicy dane są przesuwane w kierunku do niższej komórki,

	//	Przepisanie danych o jedną komórkę w dół dla wszystkich danych oprócz jednej:
	for(uint8_t i=0; i<(pRC5->buf-1); i++)
	{
		pRC5->RC5_data_frame[i] = pRC5->RC5_data_frame[i+1];
	}

	pRC5->RC5_data_frame[pRC5->buf-1] = data_frame;
}

uint16_t RC5_get_data(const RC5_t *pRC5)
{
	//	-funkcja zwraca najnowszą odebraną ramkę danych w standardzie RC5,
	//	-ramka ta znajduje się w komórce o najwyższym numerze w tablicy odebranych danych,

	return pRC5->RC5_data_frame[pRC5->buf - 1];
}


void RC5_set_flag_new_data(RC5_t *pRC5)
{
	pRC5->flag = 1;
}

void RC5_reset_flag_new_data(RC5_t *pRC5)
{
	pRC5->flag = 0;
}

uint8_t RC5_get_flag_new_data(const RC5_t *pRC5)
{
	return pRC5->flag;
}


void RC5_control_panel(RC5_t *pRC5, void (*foo_1)(void), void (*foo_2)(void), void (*foo_3)(void), void (*foo_4)(void))
{
	//	====================================================
	//	OBSŁUGA NOWO ODEBRANEJ RAMKI DANYCH Z PILOTA PO RC5:
	//	====================================================
	if(RC5_get_flag_new_data(pRC5) == 1)
	{
		RC5_reset_flag_new_data(pRC5);
		//uint16_t frame_read = RC5_get_data(&RC5_FRAME);

		if(pRC5->address == NINTENDO)
		{
			switch(pRC5->command)
			{
				case NIN_UP:
				{
					foo_1();
					break;
				}
				case NIN_RIGHT:
				{
					foo_2();
					break;
				}
				case NIN_DOWN:
				{
					foo_3();
					break;
				}
				case NIN_LEFT:
				{
					foo_4();
					break;
				}
			}
		}
	}
}


void RC5_terminal_info(TERMINAL_t *pTERMINAL, RC5_t *pRC5, const uint8_t line, const uint8_t row_1, const uint8_t row_2)
{
	TERMINAL_line(pTERMINAL, line + 1, row_1, "RC5 - ADDRESS");
	TERMINAL_number(pTERMINAL, line + 1, row_2, pRC5->address, 3);

	TERMINAL_line(pTERMINAL, line + 2, row_1, "RC5 - COMMAND");
	TERMINAL_number(pTERMINAL, line + 2, row_2, pRC5->command, 3);

	TERMINAL_line(pTERMINAL, line + 3, row_1, "RC5 - TOGGLE");
	TERMINAL_number(pTERMINAL, line + 3, row_2, pRC5->toggle, 1);
}

void RC5_terminal_info_2(TERMINAL_t *pTERMINAL, RC5_t *pRC5, const uint8_t line, const uint8_t row_1, const uint8_t row_2)
{
	TERMINAL_line(pTERMINAL, line, row_1, "RC5 - ADD-COM-TOG-FRAME");

	TERMINAL_number(pTERMINAL, line, row_2, pRC5->address, 3);
	TERMINAL_number(pTERMINAL, line, row_2 + 3, pRC5->command, 3);
	TERMINAL_number(pTERMINAL, line, row_2 + 6, pRC5->toggle, 1);
	TERMINAL_number(pTERMINAL, line, row_2 + 8, RC5_get_data(pRC5), 5);
}




