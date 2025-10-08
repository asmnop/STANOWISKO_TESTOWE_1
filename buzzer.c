/*
 * buzzer.c
 *
 *  Created on: 12 mar 2023
 *      Author: mm
 */


#include "../../LIB/Inc/buzzer.h"
//#include "definitions.h"
//#include "driver/ledc.h"	//	Obsługa PWM dla LED,
#include "..\..\LIB\Inc\pwm.h"
#include "..\..\LIB\Inc\notes.h"
#include "..\..\LIB\Inc\songs.h"
#include "..\..\LIB\Inc\universal_functions.h"
#include "..\..\LIB\Inc\VT100.h"
#include "../../LIB/Inc/DS18B20.h"
//#include "info.h"

extern DS18B20_t DS18B20_1;




//	NOTE_MIN:
//	-najkrótszy dźwięk jaki można wydać z buzzera, przyjmujemy że najkrótszy możliwy to 1/64,
//	-oznacza to, że jeżeli chcemy wydać dźwięk:
//	-1/32 to musimy go wydawać przez 2*(1/64) = 2/64 = 1/32,
//	-1/16 to musimy go wydawać przez 4*(1/64) = 4/64 = 1/16,
//	-1/8 to musimy go wydawać przez 8*(1/64) = 8/64 = 1/8,
//	-1/4 to musimy go wydawać przez 16*(1/64) = 16/64 = 1/4,
//	-1/2 (półnuta) to musimy go wydawać przez 32*(1/64) = 32/64 = 1/2,
//	-1 (cała nuta) to musimy go wydawać przez 64*(1/64) = 64/64 = 1,

//#define TIME_NOTE_MIN	( ( ( ( 1000000 ) / ( NOTE_MIN ) ) * ( 240 ) ) / ( TEMPO ) )	//	Czas trwania najkrótszego dźwięku [us],


//	SPOSOBY GENEROWANIA DŹWIĘKÓW:
//	-odpalamy funkcję, która rozpoczyna odgrywanie melodii,
//	-zmiana stanu na linii --> wywołanie przerwania od zmiany stanu na linii,
//	-w obsłudze przerwania odpytanie czy to początek nowej piosenki,
//	-jeśli tak to odpalenie nadawania sygnału pwm,
//	-sygnał jest nadawany, zbocza mają miejsce, są zliczane, jeśli zajdzie odpowiednia ilość zboczy to reakcja w przerwaniu,
//	-reakcja polega na zmianie częstotliwości nadawania kolejnego dźwięku,
//	-w obsłudze przerwania jest wskaźnik który 'wędruje' po odpowiedniej tablicy z danymi o odczytuje parametry utworu,


//	UWAGI:
//	-należy zwracać uwagę na argumenty jakie przyjmuje funkcja tzn czy przyjmuje adres pierwszego elementu w tablicy gdzie pierwszym
//	elementem jest wartość tempa a kolejne to nuta i czas trwania,
//	-CZY może zapis danych utworu zawiera się w strukturze o polach nazwa, tempo, nuty i wtedy następuje problem z pobraniem wartości
//	tempa jeśli jako argument wejściowy podajemy tablice z nutami np. play_song(&NYAN_CAT_song.notes[0]);


/*
void BUZZER_init(void)
{
	//	-procedura inicjalizacyjna SM,

	uint32_t auto_reload = __LL_TIM_CALC_ARR(SYS_CORE_CLOCK, 1000 - 1, 100) + 1;

	////////////////////////////////////////////////////////////////////////////
	//??LL_APB1_GRP1_EnableClock   (LL_APB1_GRP1_PERIPH_TIM3);
	//??LL_TIM_EnableARRPreload    (TIM3);
	LL_TIM_SetClockSource      (TIM3, LL_TIM_CLOCKSOURCE_INTERNAL);
	LL_TIM_SetCounterMode      (TIM3, LL_TIM_COUNTERMODE_UP);
	LL_TIM_SetPrescaler        (TIM3, 1000 - 1);
	LL_TIM_SetAutoReload       (TIM3, auto_reload - 1);
	LL_TIM_OC_SetMode          (TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_PWM1);
	LL_TIM_OC_SetPolarity      (TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_OCPOLARITY_HIGH);
	LL_TIM_OC_EnablePreload    (TIM3, LL_TIM_CHANNEL_CH1);
	LL_TIM_OC_SetCompareCH1    (TIM3, 0);
	LL_TIM_CC_EnableChannel    (TIM3, LL_TIM_CHANNEL_CH1);
	LL_TIM_EnableIT_CC1        (TIM3);
	LL_TIM_GenerateEvent_UPDATE(TIM3);
	LL_TIM_ClearFlag_UPDATE    (TIM3);
}
*/

/*
void TIM_synchro_init(void)
{

	 * Synchronizacja timerów TIM4 oraz TIM3,
	 * TIM4 pracuje jako master, TIM3 jako slave,
	 * Sygnał PWM z TIM4 nie jest wyprowadzany na zewnątrz, jest ustawiony jako
	 * wyzwalacz,
	 * TIM3 jest ustawione w tryb GATED czyli poczas występowania stanu wysokiego
	 * jego timer działa,
	 * Źródło wyzwalania TIM3 jest ustawione jako sygnał z TIM4,



	//	########################################################################
	//	KONFIGURACJA MASTER:
	uint16_t prescal = 10000;
	uint16_t freq = 1;
	uint32_t auto_reload = __LL_TIM_CALC_ARR(SYS_CORE_CLOCK, prescal - 1, freq) + 1;

	LL_APB1_GRP1_EnableClock    (LL_APB1_GRP1_PERIPH_TIM4);
	LL_TIM_EnableARRPreload     (TIM4);
	LL_TIM_SetClockSource       (TIM4, LL_TIM_CLOCKSOURCE_INTERNAL);
	LL_TIM_SetCounterMode       (TIM4, LL_TIM_COUNTERMODE_UP);
	LL_TIM_SetPrescaler         (TIM4, prescal - 1);
	LL_TIM_SetAutoReload        (TIM4, auto_reload - 1);

	LL_TIM_OC_SetMode           (TIM4, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_PWM2);
	LL_TIM_OC_SetPolarity       (TIM4, LL_TIM_CHANNEL_CH1, LL_TIM_OCPOLARITY_HIGH);
	LL_TIM_OC_EnablePreload     (TIM4, LL_TIM_CHANNEL_CH1);
	LL_TIM_OC_SetCompareCH1     (TIM4, (auto_reload>>1) );
	LL_TIM_CC_EnableChannel     (TIM4, LL_TIM_CHANNEL_CH1);

	LL_TIM_SetTriggerOutput     (TIM4, LL_TIM_TRGO_OC1REF);
	LL_TIM_EnableMasterSlaveMode(TIM4);

	LL_TIM_GenerateEvent_UPDATE (TIM4);
	LL_TIM_ClearFlag_UPDATE     (TIM4);

	//	########################################################################
	//	KONFIGURACJA SLAVE:
	prescal = 1000;
	freq = 2;
	auto_reload = __LL_TIM_CALC_ARR(SYS_CORE_CLOCK, prescal - 1, freq) + 1;

	//??LL_APB1_GRP1_EnableClock   (LL_APB1_GRP1_PERIPH_TIM3);
	//??LL_TIM_EnableARRPreload    (TIM3);
	LL_TIM_SetClockSource      (TIM3, LL_TIM_CLOCKSOURCE_INTERNAL);
	LL_TIM_SetCounterMode      (TIM3, LL_TIM_COUNTERMODE_UP);
	LL_TIM_SetPrescaler        (TIM3, prescal - 1);
	LL_TIM_SetAutoReload       (TIM3, auto_reload - 1);

	LL_TIM_OC_SetMode          (TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_PWM1);
	LL_TIM_OC_SetPolarity      (TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_OCPOLARITY_HIGH);
	LL_TIM_OC_EnablePreload    (TIM3, LL_TIM_CHANNEL_CH1);
	LL_TIM_OC_SetCompareCH1    (TIM3, (auto_reload>>1) );
	LL_TIM_CC_EnableChannel    (TIM3, LL_TIM_CHANNEL_CH1);
	LL_TIM_EnableIT_CC1        (TIM3);
	LL_TIM_GenerateEvent_UPDATE(TIM3);
	LL_TIM_ClearFlag_UPDATE    (TIM3);

	LL_TIM_SetSlaveMode        (TIM3, LL_TIM_SLAVEMODE_GATED);
	LL_TIM_SetTriggerInput     (TIM3, LL_TIM_TS_ITR3);	//	Jeżeli chcemy żeby TIM3 był sterowany z TIM4,

	//	########################################################################
	//	START:
	LL_TIM_EnableCounter       (TIM3);
	LL_TIM_EnableCounter       (TIM4);
}
*/

void TIM_song_init_G4(SONG_t *song)
{
	/*
	 * Synchronizacja timerów TIM4 oraz TIM3,
	 * TIM4 pracuje jako master, TIM3 jako slave,
	 * Sygnał PWM z TIM4 nie jest wyprowadzany na zewnątrz, jest ustawiony jako
	 * wyzwalacz,
	 * TIM3 jest ustawione w tryb GATED czyli poczas występowania stanu wysokiego
	 * jego timer działa,
	 * Źródło wyzwalania TIM3 jest ustawione jako sygnał z TIM4,
	 */

	/*
	 * UWAGI:
	 * -uwaga na stosowanie przeładowania w timerze od generowania PWM,
	 * -jeśli ustawimy tam np freq = 1 i wykonamy zapis do rejestru to zapis
	 * zostanie dokonany i kolejna aktualizacja rejestru będzie miała miejsce
	 * w momencie zajścia UPDATE!!! a więc dopiero po 1 sekundzie!!!
	 * -wystąpi blokowanie generowania zmiany f PWM,
	 */

	//	########################################################################
	//	KONFIGURACJA MASTER:
	uint16_t prescal       = 1000;
	uint32_t time_note_min = ( ( ( (SYS_CORE_CLOCK / prescal) / NOTE_MIN ) * 240 ) / (song->tempo) );
	uint16_t freq          = ( (SYS_CORE_CLOCK / prescal) / time_note_min );
	uint32_t auto_reload   = __LL_TIM_CALC_ARR(SYS_CORE_CLOCK, prescal - 1, freq) + 1;

	////////////////////////
	VT100_EOL();
	VT100_text("prescal - ");
	VT100_number(prescal);
	VT100_EOL();
	VT100_text("time_note_min - ");
	VT100_number(time_note_min);
	VT100_EOL();
	VT100_text("freq - ");
	VT100_number(freq);
	VT100_EOL();
	VT100_text("auto_reload - ");
	VT100_number(auto_reload);
	VT100_EOL();
	VT100_EOL();
	////////////////////////////////////////

	LL_DBGMCU_APB2_GRP1_FreezePeriph(LL_DBGMCU_APB2_GRP1_TIM1_STOP);
	LL_DBGMCU_APB1_GRP1_FreezePeriph(LL_DBGMCU_APB1_GRP1_TIM2_STOP);

	LL_APB2_GRP1_EnableClock    (LL_APB2_GRP1_PERIPH_TIM1);
	LL_TIM_EnableARRPreload     (TIM1);
	LL_TIM_SetClockSource       (TIM1, LL_TIM_CLOCKSOURCE_INTERNAL);
	LL_TIM_SetCounterMode       (TIM1, LL_TIM_COUNTERMODE_UP);
	LL_TIM_SetPrescaler         (TIM1, prescal - 1);
	LL_TIM_SetAutoReload        (TIM1, auto_reload - 1);

	//LL_TIM_OC_SetMode           (TIM1, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_PWM1);
	LL_TIM_OC_SetMode           (TIM1, LL_TIM_CHANNEL_CH2N, LL_TIM_OCMODE_TOGGLE);
	LL_TIM_OC_SetPolarity       (TIM1, LL_TIM_CHANNEL_CH2N, LL_TIM_OCPOLARITY_HIGH);
	LL_TIM_OC_EnablePreload     (TIM1, LL_TIM_CHANNEL_CH2N);
	LL_TIM_OC_SetCompareCH2     (TIM1, (auto_reload>>1) );
	LL_TIM_CC_EnableChannel     (TIM1, LL_TIM_CHANNEL_CH2N);

	LL_TIM_GenerateEvent_UPDATE (TIM1);
	LL_TIM_ClearFlag_UPDATE     (TIM1);
	LL_TIM_EnableIT_UPDATE      (TIM1);
	LL_TIM_EnableIT_CC2         (TIM1);

	LL_TIM_SetTriggerOutput     (TIM1, LL_TIM_TRGO_UPDATE);
	LL_TIM_EnableMasterSlaveMode(TIM1);

	//	########################################################################
	//	KONFIGURACJA SLAVE:
	prescal = 1000;
	freq = song->notes[0];
	auto_reload = __LL_TIM_CALC_ARR(SYS_CORE_CLOCK, prescal - 1, freq) + 1;

	LL_APB1_GRP1_EnableClock   (LL_APB1_GRP1_PERIPH_TIM2);
	LL_TIM_EnableARRPreload    (TIM2);
	LL_TIM_SetClockSource      (TIM2, LL_TIM_CLOCKSOURCE_INTERNAL);
	LL_TIM_SetCounterMode      (TIM2, LL_TIM_COUNTERMODE_UP);
	//LL_TIM_SetPrescaler        (TIM2, prescal - 1);
	//LL_TIM_SetAutoReload       (TIM2, auto_reload - 1);

	LL_TIM_OC_SetMode          (TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_PWM1);
	LL_TIM_OC_SetPolarity      (TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_OCPOLARITY_HIGH);
	LL_TIM_OC_EnablePreload    (TIM2, LL_TIM_CHANNEL_CH1);
	//LL_TIM_OC_SetCompareCH1    (TIM2, (auto_reload>>1) );
	LL_TIM_CC_EnableChannel    (TIM2, LL_TIM_CHANNEL_CH1);
	LL_TIM_EnableIT_CC1        (TIM2);
	LL_TIM_GenerateEvent_UPDATE(TIM2);
	LL_TIM_ClearFlag_UPDATE    (TIM2);

	LL_TIM_SetSlaveMode        (TIM2, LL_TIM_SLAVEMODE_TRIGGER);
	LL_TIM_SetTriggerInput     (TIM2, LL_TIM_TS_ITR0);	//	Jeżeli chcemy żeby TIM2 był sterowany z TIM1,

	//	########################################################################
	//	START:
	LL_TIM_EnableCounter       (TIM2);
	LL_TIM_EnableCounter       (TIM1);
}

void TIM_song_init_L4_OLD(SONG_t *song)
{
	/*
	 * Synchronizacja timerów TIM4 oraz TIM3,
	 * TIM4 pracuje jako master, TIM3 jako slave,
	 * Sygnał PWM z TIM4 nie jest wyprowadzany na zewnątrz, jest ustawiony jako
	 * wyzwalacz,
	 * TIM3 jest ustawione w tryb GATED czyli poczas występowania stanu wysokiego
	 * jego timer działa,
	 * Źródło wyzwalania TIM3 jest ustawione jako sygnał z TIM4,
	 */

	/*
	 * UWAGI:
	 * -uwaga na stosowanie przeładowania w timerze od generowania PWM,
	 * -jeśli ustawimy tam np freq = 1 i wykonamy zapis do rejestru to zapis
	 * zostanie dokonany i kolejna aktualizacja rejestru będzie miała miejsce
	 * w momencie zajścia UPDATE!!! a więc dopiero po 1 sekundzie!!!
	 * -wystąpi blokowanie generowania zmiany f PWM,
	 */

	//	####################
	//	KONFIGURACJA MASTER:
	//	####################

	uint16_t prescal       = 1000;
	uint32_t time_note_min = ( ( ( (SYS_CORE_CLOCK / prescal) / NOTE_MIN ) * 240 ) / (song->tempo) );
	uint16_t freq          = ( (SYS_CORE_CLOCK / prescal) / time_note_min );
	uint32_t auto_reload   = __LL_TIM_CALC_ARR(SYS_CORE_CLOCK, prescal - 1, freq) + 1;

	////////////////////////
	VT100_EOL();
	VT100_text("prescal - ");
	VT100_number(prescal);
	VT100_EOL();
	VT100_text("time_note_min - ");
	VT100_number(time_note_min);
	VT100_EOL();
	VT100_text("freq - ");
	VT100_number(freq);
	VT100_EOL();
	VT100_text("auto_reload - ");
	VT100_number(auto_reload);
	VT100_EOL();
	VT100_EOL();
	////////////////////////////////////////

	//LL_DBGMCU_APB2_GRP1_FreezePeriph(LL_DBGMCU_APB2_GRP1_TIM1_STOP);
	//LL_DBGMCU_APB1_GRP1_FreezePeriph(LL_DBGMCU_APB1_GRP1_TIM2_STOP);

	LL_APB2_GRP1_EnableClock    (LL_APB2_GRP1_PERIPH_TIM1);
	LL_TIM_EnableARRPreload     (TIM1);
	LL_TIM_SetClockSource       (TIM1, LL_TIM_CLOCKSOURCE_INTERNAL);
	LL_TIM_SetCounterMode       (TIM1, LL_TIM_COUNTERMODE_UP);
	LL_TIM_SetPrescaler         (TIM1, prescal - 1);
	LL_TIM_SetAutoReload        (TIM1, auto_reload - 1);

	LL_TIM_OC_SetMode           (TIM1, LL_TIM_CHANNEL_CH2, LL_TIM_OCMODE_TOGGLE);
	LL_TIM_OC_SetPolarity       (TIM1, LL_TIM_CHANNEL_CH2N, LL_TIM_OCPOLARITY_HIGH);
	LL_TIM_OC_EnablePreload     (TIM1, LL_TIM_CHANNEL_CH2);
	LL_TIM_OC_SetCompareCH2     (TIM1, (auto_reload>>1) );
	LL_TIM_CC_EnableChannel     (TIM1, LL_TIM_CHANNEL_CH2N);
	LL_TIM_EnableAllOutputs     (TIM1);	//	!!!!!!!!!!!!!!!!!!!!

	LL_TIM_GenerateEvent_UPDATE (TIM1);
	LL_TIM_ClearFlag_UPDATE     (TIM1);
	LL_TIM_EnableIT_UPDATE      (TIM1);

	LL_TIM_SetTriggerOutput     (TIM1, LL_TIM_TRGO_UPDATE);
	LL_TIM_EnableMasterSlaveMode(TIM1);


	//	###################
	//	KONFIGURACJA SLAVE:
	//	###################

	prescal     = 1000;
	freq        = song->notes[0];
	auto_reload = __LL_TIM_CALC_ARR(SYS_CORE_CLOCK, prescal - 1, freq) + 1;

	LL_APB1_GRP1_EnableClock   (LL_APB1_GRP1_PERIPH_TIM2);
	LL_TIM_EnableARRPreload    (TIM2);
	LL_TIM_SetClockSource      (TIM2, LL_TIM_CLOCKSOURCE_INTERNAL);
	LL_TIM_SetCounterMode      (TIM2, LL_TIM_COUNTERMODE_UP);
	LL_TIM_SetPrescaler        (TIM2, prescal - 1);
	LL_TIM_SetAutoReload       (TIM2, auto_reload - 1);

	//	WYJŚCIE SYGNAŁU PWM NA LINIE Z ZIELONĄ DIODĄ:
	LL_TIM_OC_SetMode          (TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_OCMODE_TOGGLE);
	LL_TIM_OC_SetPolarity      (TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_OCPOLARITY_HIGH);
	LL_TIM_OC_EnablePreload    (TIM2, LL_TIM_CHANNEL_CH2);
	LL_TIM_OC_SetCompareCH1    (TIM2, (auto_reload>>1) );
	LL_TIM_CC_EnableChannel    (TIM2, LL_TIM_CHANNEL_CH2);

	//	WYJŚCIE SYGNAŁU PWM NA LINIĘ DOCELOWĄ:
	LL_TIM_OC_SetMode          (TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_PWM1);
	LL_TIM_OC_SetPolarity      (TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_OCPOLARITY_HIGH);
	LL_TIM_OC_EnablePreload    (TIM2, LL_TIM_CHANNEL_CH1);
	LL_TIM_OC_SetCompareCH1    (TIM2, (auto_reload>>1) );
	LL_TIM_CC_EnableChannel    (TIM2, LL_TIM_CHANNEL_CH1);


	LL_TIM_GenerateEvent_UPDATE(TIM2);
	LL_TIM_ClearFlag_UPDATE    (TIM2);

	LL_TIM_SetSlaveMode        (TIM2, LL_TIM_SLAVEMODE_TRIGGER);
	LL_TIM_SetTriggerInput     (TIM2, LL_TIM_TS_ITR0);	//	Jeżeli chcemy żeby TIM2 był sterowany z TIM1,

	//LL_TIM_EnableCounter       (TIM2);
	LL_TIM_EnableCounter       (TIM1);






	//	########################################################################
	//	KONFIGURACJA SLAVE:





	//	########################################################################
	//	START:

}

uint8_t compute_note_duration(uint8_t note_duration)
{
	//	Obliczenie ile najmniejszych stopni dźwięku trwa dana nuta:
	if(note_duration == 1)			note_duration = NOTE_MIN;			//	128,	nuta,
	else if(note_duration == 2)		note_duration = NOTE_MIN>>1;		//	64,		półnuta,
	else if(note_duration == 3)		note_duration = (NOTE_MIN>>2) * 3;	//	96,		półnuta + kropka,
	else if(note_duration == 4)		note_duration = NOTE_MIN>>2;		//	32,		ćwierćnuta,
	else if(note_duration == 6)		note_duration = (NOTE_MIN>>3) * 3;	//	48,		ćwierćnuta + kropka,
	else if(note_duration == 8)		note_duration = NOTE_MIN>>3;		//	16,		ósemka,
	else if(note_duration == 12)	note_duration = (NOTE_MIN>>4) * 3;	//	24,		ósemka + kropka,
	else if(note_duration == 16)	note_duration = NOTE_MIN>>4;		//	8,		szesnastka,
	else if(note_duration == 24)	note_duration = (NOTE_MIN>>5) * 3;	//	12,		szesnastka + kropka,
	else if(note_duration == 32)	note_duration = NOTE_MIN>>5;		//	4		32,
	else if(note_duration == 48)	note_duration = (NOTE_MIN>>6) * 3;	//	6
	else if(note_duration == 64)	note_duration = NOTE_MIN>>6;		//	2		64,
	else if(note_duration == 96)	note_duration = (NOTE_MIN>>7) * 3;	//	3
	else if(note_duration == 128)	note_duration = NOTE_MIN>>7;		//	1		128,
	else if(note_duration == TRI_8)	note_duration = 11;
	else if(note_duration == TRI_16)note_duration = 5;

	return note_duration;
}

uint16_t number_of_chronon(SONG_t *song)
{
	//	-wyliczenie z jakiej liczby niepodzielnych odcinków czasu składa się utwór,
	//	CHRONON, z greckiego 'czas' - niepodzielna jednostka czasu,

	uint16_t i = 0;
	uint16_t chronon_summary = 0;
	uint16_t note_duration = 0;
	uint16_t note_freq = 0;
	uint16_t note_freq_prev = 0;

	while(song->notes[i] != 0)
	{
		note_freq = song->notes[i];
		note_duration = song->notes[i + 1];
		chronon_summary = chronon_summary + compute_note_duration(note_duration);

		if(note_freq == note_freq_prev)
		{
			chronon_summary++;
		}

		i = i + 2;
		note_freq_prev = note_freq;
	};

	return chronon_summary;
}

uint16_t number_of_notes(SONG_t *pSONG)
{
	//	-funkcja zwraca ilość nut w utworze,

	uint16_t i = 0;
	uint16_t note_counter = 0;

	while(pSONG->notes[i] != 0)
	{
		note_counter++;
		i = i + 2;
	};

	return note_counter;
}


void song_duration(SONG_t *song)
{
	//	-wyznaczenie czasu trwania utworu muzycznego,

	uint16_t chronon_freq = (NOTE_MIN * song->tempo) / 240;
	uint16_t number_of_chronons = number_of_chronon(song);
	uint8_t song_duration_s = number_of_chronons / chronon_freq;
	uint16_t song_duration_ms = (1000 * (number_of_chronons % chronon_freq) ) / chronon_freq;

	//	###############################
	VT100_EOL();
	VT100_text("CZAS TRWANIA UTWORU:");
	VT100_EOL();
	VT100_number(song_duration_s);
	VT100_text(" [s], ");
	VT100_number(song_duration_ms);
	VT100_text(" [ms]");
	VT100_EOL();
	//	###############################
}


void TIM_song_play(SONG_t *song)
{
	/*
	 * Synchronizacja timerów TIM4 oraz TIM3,
	 * TIM4 pracuje jako master, TIM3 jako slave,
	 * Sygnał PWM z TIM4 nie jest wyprowadzany na zewnątrz, jest ustawiony jako
	 * wyzwalacz,
	 * TIM3 jest ustawione w tryb GATED czyli poczas występowania stanu wysokiego
	 * jego timer działa,
	 * Źródło wyzwalania TIM3 jest ustawione jako sygnał z TIM4,
	 */

	/*
	 * UWAGI:
	 * -uwaga na stosowanie przeładowania w timerze od generowania PWM,
	 * -jeśli ustawimy tam np freq = 1 i wykonamy zapis do rejestru to zapis
	 * zostanie dokonany i kolejna aktualizacja rejestru będzie miała miejsce
	 * w momencie zajścia UPDATE!!! a więc dopiero po 1 sekundzie!!!
	 * -wystąpi blokowanie generowania zmiany f PWM,
	 */

	//	####################
	//	KONFIGURACJA MASTER:
	//	####################

	uint16_t prescal       = 1000;
	uint16_t freq          = (NOTE_MIN * song->tempo) / 240;
	uint32_t time_note_min = ( (SYS_CORE_CLOCK / prescal) / freq );
	uint32_t auto_reload   = __LL_TIM_CALC_ARR(SYS_CORE_CLOCK, prescal - 1, freq) + 1;

	////////////////////////
	VT100_EOL();
	VT100_text("PRESCAL       - ");
	VT100_number(prescal);
	VT100_EOL();
	VT100_text("freq          - ");
	VT100_number(freq);
	VT100_EOL();
	VT100_text("time_note_min - ");
	VT100_number(time_note_min);
	VT100_EOL();
	VT100_text("auto_reload   - ");
	VT100_number(auto_reload);
	VT100_EOL();
	VT100_EOL();

	VT100_EOL();
	VT100_text("Liczba chronon-ów   - ");
	VT100_number(number_of_chronon(song));
	VT100_EOL();
	song_duration(song);


	////////////////////////////////////////

	LL_TIM_SetPrescaler         (TIM1, prescal - 1);
	LL_TIM_SetAutoReload        (TIM1, auto_reload - 1);
	LL_TIM_OC_SetCompareCH2     (TIM1, (auto_reload>>1) );
	LL_TIM_EnableIT_UPDATE      (TIM1);

	//	###################
	//	KONFIGURACJA SLAVE:
	//	###################

	prescal     = 1000;
	freq        = song->notes[0];
	auto_reload = __LL_TIM_CALC_ARR(SYS_CORE_CLOCK, prescal - 1, freq) + 1;

	LL_TIM_SetPrescaler        (TIM2, prescal - 1);
	LL_TIM_SetAutoReload       (TIM2, auto_reload - 1);
	LL_TIM_OC_SetCompareCH1    (TIM2, (auto_reload>>1) );

	//	WYJŚCIE SYGNAŁU PWM NA LINIĘ DOCELOWĄ:

	LL_TIM_OC_SetCompareCH1    (TIM2, (auto_reload>>1) );

	LL_TIM_GenerateEvent_UPDATE(TIM2);
	LL_TIM_ClearFlag_UPDATE    (TIM2);

	//LL_TIM_EnableCounter       (TIM2);
	LL_TIM_EnableCounter       (TIM1);
}

void TIM_song_play_(SONG_t *song)
{
	/*
	 * Synchronizacja timerów TIM4 oraz TIM3,
	 * TIM4 pracuje jako master, TIM3 jako slave,
	 * Sygnał PWM z TIM4 nie jest wyprowadzany na zewnątrz, jest ustawiony jako
	 * wyzwalacz,
	 * TIM3 jest ustawione w tryb GATED czyli poczas występowania stanu wysokiego
	 * jego timer działa,
	 * Źródło wyzwalania TIM3 jest ustawione jako sygnał z TIM4,
	 */

	/*
	 * UWAGI:
	 * -uwaga na stosowanie przeładowania w timerze od generowania PWM,
	 * -jeśli ustawimy tam np freq = 1 i wykonamy zapis do rejestru to zapis
	 * zostanie dokonany i kolejna aktualizacja rejestru będzie miała miejsce
	 * w momencie zajścia UPDATE!!! a więc dopiero po 1 sekundzie!!!
	 * -wystąpi blokowanie generowania zmiany f PWM,
	 */

	//	####################
	//	KONFIGURACJA MASTER:
	//	####################

	uint16_t prescal       = 1000;
	uint16_t freq          = (NOTE_MIN * song->tempo) / 240;
	uint32_t time_note_min = ( (SYS_CORE_CLOCK / prescal) / freq );
	uint32_t auto_reload   = __LL_TIM_CALC_ARR(SYS_CORE_CLOCK, prescal - 1, freq) + 1;

	////////////////////////
	VT100_EOL();
	VT100_text("PRESCAL       - ");
	VT100_number(prescal);
	VT100_EOL();
	VT100_text("freq          - ");
	VT100_number(freq);
	VT100_EOL();
	VT100_text("time_note_min - ");
	VT100_number(time_note_min);
	VT100_EOL();
	VT100_text("auto_reload   - ");
	VT100_number(auto_reload);
	VT100_EOL();
	VT100_EOL();

	VT100_EOL();
	VT100_text("Liczba chronon-ów   - ");
	VT100_number(number_of_chronon(song));
	VT100_EOL();
	song_duration(song);


	////////////////////////////////////////


	LL_DBGMCU_APB2_GRP1_FreezePeriph(LL_DBGMCU_APB2_GRP1_TIM1_STOP);
	LL_DBGMCU_APB1_GRP1_FreezePeriph(LL_DBGMCU_APB1_GRP1_TIM2_STOP);

	LL_APB2_GRP1_EnableClock    (LL_APB2_GRP1_PERIPH_TIM1);
	LL_TIM_EnableARRPreload     (TIM1);
	LL_TIM_SetClockSource       (TIM1, LL_TIM_CLOCKSOURCE_INTERNAL);
	LL_TIM_SetCounterMode       (TIM1, LL_TIM_COUNTERMODE_UP);
	LL_TIM_SetPrescaler         (TIM1, prescal - 1);
	LL_TIM_SetAutoReload        (TIM1, auto_reload - 1);

	LL_TIM_OC_SetMode           (TIM1, LL_TIM_CHANNEL_CH2, LL_TIM_OCMODE_TOGGLE);
	LL_TIM_OC_SetPolarity       (TIM1, LL_TIM_CHANNEL_CH2N, LL_TIM_OCPOLARITY_HIGH);
	LL_TIM_OC_EnablePreload     (TIM1, LL_TIM_CHANNEL_CH2);
	LL_TIM_OC_SetCompareCH2     (TIM1, (auto_reload>>1) );
	LL_TIM_CC_EnableChannel     (TIM1, LL_TIM_CHANNEL_CH2N);
	LL_TIM_EnableAllOutputs     (TIM1);	//	!!!!!!!!!!!!!!!!!!!!

	LL_TIM_GenerateEvent_UPDATE (TIM1);
	LL_TIM_ClearFlag_UPDATE     (TIM1);
	LL_TIM_EnableIT_UPDATE      (TIM1);

	LL_TIM_SetTriggerOutput     (TIM1, LL_TIM_TRGO_UPDATE);
	LL_TIM_EnableMasterSlaveMode(TIM1);


	//	###################
	//	KONFIGURACJA SLAVE:
	//	###################

	prescal     = 1000;
	freq        = song->notes[0];
	auto_reload = __LL_TIM_CALC_ARR(SYS_CORE_CLOCK, prescal - 1, freq) + 1;

	LL_APB1_GRP1_EnableClock   (LL_APB1_GRP1_PERIPH_TIM2);
	LL_TIM_EnableARRPreload    (TIM2);
	LL_TIM_SetClockSource      (TIM2, LL_TIM_CLOCKSOURCE_INTERNAL);
	LL_TIM_SetCounterMode      (TIM2, LL_TIM_COUNTERMODE_UP);
	LL_TIM_SetPrescaler        (TIM2, prescal - 1);
	LL_TIM_SetAutoReload       (TIM2, auto_reload - 1);

	//	WYJŚCIE SYGNAŁU PWM NA LINIE Z ZIELONĄ DIODĄ:
	LL_TIM_OC_SetMode          (TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_OCMODE_TOGGLE);
	LL_TIM_OC_SetPolarity      (TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_OCPOLARITY_HIGH);
	LL_TIM_OC_EnablePreload    (TIM2, LL_TIM_CHANNEL_CH2);
	LL_TIM_OC_SetCompareCH1    (TIM2, (auto_reload>>1) );
	LL_TIM_CC_EnableChannel    (TIM2, LL_TIM_CHANNEL_CH2);

	//	WYJŚCIE SYGNAŁU PWM NA LINIĘ DOCELOWĄ:
	LL_TIM_OC_SetMode          (TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_PWM1);
	LL_TIM_OC_SetPolarity      (TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_OCPOLARITY_HIGH);
	LL_TIM_OC_EnablePreload    (TIM2, LL_TIM_CHANNEL_CH1);
	LL_TIM_OC_SetCompareCH1    (TIM2, (auto_reload>>1) );
	LL_TIM_CC_EnableChannel    (TIM2, LL_TIM_CHANNEL_CH1);


	LL_TIM_GenerateEvent_UPDATE(TIM2);
	LL_TIM_ClearFlag_UPDATE    (TIM2);

	LL_TIM_SetSlaveMode        (TIM2, LL_TIM_SLAVEMODE_TRIGGER);
	LL_TIM_SetTriggerInput     (TIM2, LL_TIM_TS_ITR0);	//	Jeżeli chcemy żeby TIM2 był sterowany z TIM1,

	//LL_TIM_EnableCounter       (TIM2);
	LL_TIM_EnableCounter       (TIM1);
}


void TIM_song_init(void)
{
	/*
	 * Synchronizacja timerów TIM4 oraz TIM3,
	 * TIM4 pracuje jako master, TIM3 jako slave,
	 * Sygnał PWM z TIM4 nie jest wyprowadzany na zewnątrz, jest ustawiony jako
	 * wyzwalacz,
	 * TIM3 jest ustawione w tryb GATED czyli poczas występowania stanu wysokiego
	 * jego timer działa,
	 * Źródło wyzwalania TIM3 jest ustawione jako sygnał z TIM4,
	 */

	/*
	 * UWAGI:
	 * -uwaga na stosowanie przeładowania w timerze od generowania PWM,
	 * -jeśli ustawimy tam np freq = 1 i wykonamy zapis do rejestru to zapis
	 * zostanie dokonany i kolejna aktualizacja rejestru będzie miała miejsce
	 * w momencie zajścia UPDATE!!! a więc dopiero po 1 sekundzie!!!
	 * -wystąpi blokowanie generowania zmiany f PWM,
	 */

	//	####################
	//	KONFIGURACJA MASTER:
	//	####################

	/*
	uint16_t prescal       = 1000;
	uint16_t freq          = (NOTE_MIN * song->tempo) / 240;
	uint32_t time_note_min = ( (SYS_CORE_CLOCK / prescal) / freq );
	uint32_t auto_reload   = __LL_TIM_CALC_ARR(SYS_CORE_CLOCK, prescal - 1, freq) + 1;

	////////////////////////
	VT100_EOL();
	VT100_text("PRESCAL       - ");
	VT100_number(prescal);
	VT100_EOL();
	VT100_text("freq          - ");
	VT100_number(freq);
	VT100_EOL();
	VT100_text("time_note_min - ");
	VT100_number(time_note_min);
	VT100_EOL();
	VT100_text("auto_reload   - ");
	VT100_number(auto_reload);
	VT100_EOL();
	VT100_EOL();

	VT100_EOL();
	VT100_text("Liczba chronon-ów   - ");
	VT100_number(number_of_chronon(song));
	VT100_EOL();
	song_duration(song);
*/

	////////////////////////////////////////


	LL_DBGMCU_APB2_GRP1_FreezePeriph(LL_DBGMCU_APB2_GRP1_TIM1_STOP);
	LL_DBGMCU_APB1_GRP1_FreezePeriph(LL_DBGMCU_APB1_GRP1_TIM2_STOP);

	LL_APB2_GRP1_EnableClock    (LL_APB2_GRP1_PERIPH_TIM1);
	LL_TIM_EnableARRPreload     (TIM1);
	LL_TIM_SetClockSource       (TIM1, LL_TIM_CLOCKSOURCE_INTERNAL);
	LL_TIM_SetCounterMode       (TIM1, LL_TIM_COUNTERMODE_UP);
	//LL_TIM_SetPrescaler         (TIM1, prescal - 1);
	//LL_TIM_SetAutoReload        (TIM1, auto_reload - 1);

	LL_TIM_OC_SetMode           (TIM1, LL_TIM_CHANNEL_CH2, LL_TIM_OCMODE_TOGGLE);
	LL_TIM_OC_SetPolarity       (TIM1, LL_TIM_CHANNEL_CH2N, LL_TIM_OCPOLARITY_HIGH);
	LL_TIM_OC_EnablePreload     (TIM1, LL_TIM_CHANNEL_CH2);
	//LL_TIM_OC_SetCompareCH2     (TIM1, (auto_reload>>1) );
	LL_TIM_CC_EnableChannel     (TIM1, LL_TIM_CHANNEL_CH2N);
	LL_TIM_EnableAllOutputs     (TIM1);	//	!!!!!!!!!!!!!!!!!!!!

	LL_TIM_GenerateEvent_UPDATE (TIM1);
	LL_TIM_ClearFlag_UPDATE     (TIM1);
	//LL_TIM_EnableIT_UPDATE      (TIM1);

	LL_TIM_SetTriggerOutput     (TIM1, LL_TIM_TRGO_UPDATE);
	LL_TIM_EnableMasterSlaveMode(TIM1);


	//	###################
	//	KONFIGURACJA SLAVE:
	//	###################

	//prescal     = 1000;
	//freq        = song->notes[0];
	//auto_reload = __LL_TIM_CALC_ARR(SYS_CORE_CLOCK, prescal - 1, freq) + 1;

	LL_APB1_GRP1_EnableClock   (LL_APB1_GRP1_PERIPH_TIM2);
	LL_TIM_EnableARRPreload    (TIM2);
	LL_TIM_SetClockSource      (TIM2, LL_TIM_CLOCKSOURCE_INTERNAL);
	LL_TIM_SetCounterMode      (TIM2, LL_TIM_COUNTERMODE_UP);
	//LL_TIM_SetPrescaler        (TIM2, prescal - 1);
	//LL_TIM_SetAutoReload       (TIM2, auto_reload - 1);

	//	WYJŚCIE SYGNAŁU PWM NA LINIE Z ZIELONĄ DIODĄ:
	LL_TIM_OC_SetMode          (TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_OCMODE_TOGGLE);
	LL_TIM_OC_SetPolarity      (TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_OCPOLARITY_HIGH);
	LL_TIM_OC_EnablePreload    (TIM2, LL_TIM_CHANNEL_CH2);
	//LL_TIM_OC_SetCompareCH1    (TIM2, (auto_reload>>1) );
	LL_TIM_CC_EnableChannel    (TIM2, LL_TIM_CHANNEL_CH2);



	//	WYJŚCIE SYGNAŁU PWM NA LINIĘ DOCELOWĄ:
	LL_TIM_OC_SetMode          (TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_PWM1);
	LL_TIM_OC_SetPolarity      (TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_OCPOLARITY_HIGH);
	LL_TIM_OC_EnablePreload    (TIM2, LL_TIM_CHANNEL_CH1);
	//LL_TIM_OC_SetCompareCH1    (TIM2, (auto_reload>>1) );
	LL_TIM_CC_EnableChannel    (TIM2, LL_TIM_CHANNEL_CH1);

	//DS18B20_read_rom_code(&DS18B20_1);

	LL_TIM_GenerateEvent_UPDATE(TIM2);
	LL_TIM_ClearFlag_UPDATE    (TIM2);

	LL_TIM_SetSlaveMode        (TIM2, LL_TIM_SLAVEMODE_TRIGGER);
	LL_TIM_SetTriggerInput     (TIM2, LL_TIM_TS_ITR0);	//	Jeżeli chcemy żeby TIM2 był sterowany z TIM1,

	//LL_TIM_EnableCounter       (TIM2);
	//LL_TIM_EnableCounter       (TIM1);
}



void TIM_song_init_(SONG_t *song)
{
	/*
	 * Synchronizacja timerów TIM4 oraz TIM3,
	 * TIM4 pracuje jako master, TIM3 jako slave,
	 * Sygnał PWM z TIM4 nie jest wyprowadzany na zewnątrz, jest ustawiony jako
	 * wyzwalacz,
	 * TIM3 jest ustawione w tryb GATED czyli poczas występowania stanu wysokiego
	 * jego timer działa,
	 * Źródło wyzwalania TIM3 jest ustawione jako sygnał z TIM4,
	 */

	/*
	 * UWAGI:
	 * -uwaga na stosowanie przeładowania w timerze od generowania PWM,
	 * -jeśli ustawimy tam np freq = 1 i wykonamy zapis do rejestru to zapis
	 * zostanie dokonany i kolejna aktualizacja rejestru będzie miała miejsce
	 * w momencie zajścia UPDATE!!! a więc dopiero po 1 sekundzie!!!
	 * -wystąpi blokowanie generowania zmiany f PWM,
	 */

	//	########################################################################
	//	KONFIGURACJA MASTER:
	uint16_t prescal       = 1000;
	uint32_t time_note_min = ( ( ( (SYS_CORE_CLOCK / prescal) / NOTE_MIN ) * 240 ) / (song->tempo) );
	uint16_t freq          = ( (SYS_CORE_CLOCK / prescal) / time_note_min );
	uint32_t auto_reload   = __LL_TIM_CALC_ARR(SYS_CORE_CLOCK, prescal - 1, freq) + 1;

	////////////////////////
	VT100_EOL();
	VT100_text("prescal - ");
	VT100_number(prescal);
	VT100_EOL();
	VT100_text("time_note_min - ");
	VT100_number(time_note_min);
	VT100_EOL();
	VT100_text("freq - ");
	VT100_number(freq);
	VT100_EOL();
	VT100_text("auto_reload - ");
	VT100_number(auto_reload);
	VT100_EOL();
	VT100_EOL();
	////////////////////////////////////////

	LL_DBGMCU_APB2_GRP1_FreezePeriph(LL_DBGMCU_APB2_GRP1_TIM1_STOP);
	LL_DBGMCU_APB1_GRP1_FreezePeriph(LL_DBGMCU_APB1_GRP1_TIM2_STOP);

	LL_APB2_GRP1_EnableClock    (LL_APB2_GRP1_PERIPH_TIM1);
	LL_TIM_EnableARRPreload     (TIM1);
	LL_TIM_SetClockSource       (TIM1, LL_TIM_CLOCKSOURCE_INTERNAL);
	LL_TIM_SetCounterMode       (TIM1, LL_TIM_COUNTERMODE_UP);
	LL_TIM_SetPrescaler         (TIM1, prescal - 1);
	LL_TIM_SetAutoReload        (TIM1, auto_reload - 1);

	/*
	LL_TIM_OC_SetMode           (TIM1, LL_TIM_CHANNEL_CH2N, LL_TIM_OCMODE_TOGGLE);
	LL_TIM_OC_SetPolarity       (TIM1, LL_TIM_CHANNEL_CH2N, LL_TIM_OCPOLARITY_HIGH);
	LL_TIM_OC_EnablePreload     (TIM1, LL_TIM_CHANNEL_CH2N);
	LL_TIM_OC_SetCompareCH2     (TIM1, (auto_reload>>1) );
	LL_TIM_CC_EnableChannel     (TIM1, LL_TIM_CHANNEL_CH2N);
*/
	LL_TIM_OC_SetMode           (TIM1, LL_TIM_CHANNEL_CH2, LL_TIM_OCMODE_TOGGLE);
	LL_TIM_OC_SetPolarity       (TIM1, LL_TIM_CHANNEL_CH2, LL_TIM_OCPOLARITY_HIGH);
	LL_TIM_OC_EnablePreload     (TIM1, LL_TIM_CHANNEL_CH2);
	LL_TIM_OC_SetCompareCH2     (TIM1, (auto_reload>>1) );
	LL_TIM_CC_EnableChannel     (TIM1, LL_TIM_CHANNEL_CH2);

	LL_TIM_GenerateEvent_UPDATE (TIM1);
	LL_TIM_ClearFlag_UPDATE     (TIM1);
	LL_TIM_EnableIT_UPDATE      (TIM1);
	LL_TIM_EnableIT_CC2         (TIM1);

	LL_TIM_EnableCounter       (TIM1);
	while(1);

	//LL_TIM_SetTriggerOutput     (TIM1, LL_TIM_TRGO_UPDATE);
	//LL_TIM_EnableMasterSlaveMode(TIM1);

	//	########################################################################
	//	KONFIGURACJA SLAVE:
	prescal = 1000;
	freq = song->notes[0];
	auto_reload = __LL_TIM_CALC_ARR(SYS_CORE_CLOCK, prescal - 1, freq) + 1;

	LL_APB1_GRP1_EnableClock   (LL_APB1_GRP1_PERIPH_TIM2);
	LL_TIM_EnableARRPreload    (TIM2);
	LL_TIM_SetClockSource      (TIM2, LL_TIM_CLOCKSOURCE_INTERNAL);
	LL_TIM_SetCounterMode      (TIM2, LL_TIM_COUNTERMODE_UP);
	//LL_TIM_SetPrescaler        (TIM2, prescal - 1);
	//LL_TIM_SetAutoReload       (TIM2, auto_reload - 1);

	LL_TIM_OC_SetMode          (TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_PWM1);
	LL_TIM_OC_SetPolarity      (TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_OCPOLARITY_HIGH);
	LL_TIM_OC_EnablePreload    (TIM2, LL_TIM_CHANNEL_CH1);
	//LL_TIM_OC_SetCompareCH1    (TIM2, (auto_reload>>1) );
	LL_TIM_CC_EnableChannel    (TIM2, LL_TIM_CHANNEL_CH1);
	LL_TIM_EnableIT_CC1        (TIM2);
	LL_TIM_GenerateEvent_UPDATE(TIM2);
	LL_TIM_ClearFlag_UPDATE    (TIM2);

	LL_TIM_SetSlaveMode        (TIM2, LL_TIM_SLAVEMODE_TRIGGER);
	LL_TIM_SetTriggerInput     (TIM2, LL_TIM_TS_ITR0);	//	Jeżeli chcemy żeby TIM2 był sterowany z TIM1,

	//	########################################################################
	//	START:
	LL_TIM_EnableCounter       (TIM2);
	LL_TIM_EnableCounter       (TIM1);
	while(1);
}




/*
//	##############################################################################################################################
void play_note(const uint16_t freq, const uint32_t duration)
{
	//	-funkcja generuje dźwięk o częstotliwości 'freq' i czasie trwania 'duration',
	//	-częstotliwość drgań decyduje o wysokości dźwięku, zachodzi zależność, że im wyższa częstotliwość tym wyższy dźwięk,
	//	-'freq' - częstotliwość dźwięku muzycznego,
	//	-'duration' - czas trwania dźwięku, jednostki zależą od konfiguracji zegara odmierzającego czas oraz tempa utworu,
	//	-funkcja dostosowana do odpalania w obsłudze utworu muzycznego,
	//	-jako argumenty wpisujemy np. A4, C5, D4x, F5b,

	static uint16_t last_note = 0;	//	Ostatnio generowana nuta,

	//	$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	//	Sprawdzenie czy ostatnio wygenerowana nuta nie jest taka sama jak obecnie oczekująca na zagranie,
	//	Jest to sprawdzane, aby w razie zgodności nut wykonać krótką przerwę pomiędzy dźwiękami,
	if(freq == last_note)
	{
		LL_TIM_DisableCounter(TIM3);
		LL_TIM_SetCounter(TIM3, 0);
		delay_ms(2);
		LL_TIM_EnableCounter(TIM3);
	}

	//	$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	//	Jeśli nie musimy wykonać pauzy to gramy dźwięk,
	if(freq != PAUSE_SONG)
	{
		uint32_t auto_reload = __LL_TIM_CALC_ARR(SYS_CORE_CLOCK, 1000 - 1, freq) + 1;
		LL_TIM_SetAutoReload(TIM3, auto_reload - 1);
		LL_TIM_OC_SetCompareCH1(TIM3, (auto_reload>>1) - 1);
		LL_TIM_EnableCounter(TIM3);
		delay_ms(duration);
	}
	//	$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	//	Generowanie pauzy o wyznaczonym czasie trwania w [us],
	else
	{
		LL_TIM_DisableCounter(TIM3);
		LL_TIM_SetCounter(TIM3, 0);
		delay_ms(duration);
		LL_TIM_EnableCounter(TIM3);
	}

	last_note = freq;	//	Zapis wygenerowanej nuty, jako ostatnio zagranej,
}

void play_song(SONG_t *song)
{
	//	-argumentem funkcji jest adres pierwszego pola w tablicy z
	//	-odgrywanie utworu,
	//	-utwór odgrywany jest w całości bez możliwości przerwania go,

	///////////ledc_timer_resume(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0);	//	Wznowienie pracy zegara,

	uint16_t i = 0;

	//	Obliczenie czasu trwania najkrótszej nuty, wartość tempa pobierana jest z pierwszej komórki tablicy z nutami danej piosenki,
	//const uint32_t time_note_min = 	( ( ( 1000000 / NOTE_MIN ) * 240 ) / (*song) );
	//	Czas trwania najkrótszej nuty w [ms],
	const uint32_t time_note_min = 	( ( ( 1000 / NOTE_MIN ) * 240 ) / (song->tempo) );
	//	Czas generowania dźwięku w [us] o długości zadanej w wywołaniu funkcji,
	//const uint32_t time_note = TIME_NOTE_MIN * ( NOTE_MIN / duration );
	//const uint32_t time_note = time_note_min * ( NOTE_MIN / duration );
	uint32_t time_note = 0;

	do
	{
		time_note = time_note_min * ( NOTE_MIN / ((song->notes[i+1])) );
		play_note((song->notes[i]), time_note);
		i++;
		i++;
	}while((song->notes[i]) != END_SONG);

	LL_TIM_DisableCounter(TIM3);
	LL_TIM_SetCounter(TIM3, 0);

	///////////ledc_timer_pause(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0);	//	Zatrzymanie pracy zegara,

}

*/

/*
void play_BEEP(const uint16_t freq)
{
	//	-wydanie dźwięku tzw. "beepnięcia", czyli prostego sygnału dźwiękowego,
	//	-'freq' - parametr określający jaką nutę zagrać,
	//	-należy mieć na uwadzę, aby czas trwania "beepnięcia" był większy od wartości częstotliwości,
	//	-chodzi o to aby "beepnięcie" zostało wygenerowane,
	//	-funkcja polega na włączeniu generowania określonego sygnału PWM na zadany czas i następnie wyłączeniu jego generowania,

	ledc_timer_resume(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0);	//	Wznowienie pracy zegara,
	PWM_start(&PWM_buzzer, freq, 10, 500);					//	Włączenie generowania dźwięku "beepnięcia" :)
	esp_rom_delay_us(20000);								//	Czas trwania "beepnięcia" :)
	ledc_timer_pause(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0);	//	Zatrzymanie pracy zegara,
}

void playlist(uint8_t mode, const uint16_t song_number)
{
	//play_song_pooling(mode, *(PLAYLIST+song_number));
	//play_song_pooling(mode, *(PLAYLIST+song));

	//buzzer_play_song(PANIE_JANIE);
	//buzzer_play_song(PANIE_JANIE),
	//buzzer_play_song(TANIEC);
	//buzzer_play_song(PRZYBIEZELI);
	//play_song(MARIO);
	//buzzer_play_song(INTEL);
	//buzzer_play_song(CONTRA);
	//buzzer_play_song(BOONDOCK_SAINTS);
	//buzzer_play_song(ODA_DO_RADOSCI);
	//buzzer_play_song(BLUE_EYED_STRANGER);
	//buzzer_play_song(STARY_NIEDZWIEDZ);
	//buzzer_play_song(THE_LICK);
	//buzzer_play_song(ROZMARYN);
	//buzzer_play_song(NYAN_CAT);
	 //play_song_pooling(mode, NYAN_CAT);
}


int64_t play_note_pooling(const uint16_t freq, const uint32_t duration)
{
	//	-funkcja generuje dźwięk o częstotliwości 'freq',
	//	-częstotliwość drgań decyduje o wysokości dźwięku, zachodzi zależność, że im wyższa częstotliwość tym wyższy dźwięk,
	//	-'freq' - częstotliwość dźwięku muzycznego,
	//	-jako argumenty wpisujemy np. A4, C5, D4x, F5b,
	//	-w odróżnieniu od funkcji 'play_note()' nie zadajemy czasu trwania dźwięku,
	//	-robimy to w celu aby nie zajmować czasu uC na odmierzanie czasu,


	static uint16_t last_note = 0;	//	Ostatnio generowana nuta,
	static int64_t time_start = 0;	//	Zmienna, której wartość jest nadawana w momencie rozpoczęcia generowania dźwięku,

	//	$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	//	Sprawdzenie czy ostatnio wygenerowana nuta nie jest taka sama jak obecnie oczekująca na zagranie,
	//	Jest to sprawdzane, aby w razie zgodności nut wykonać krótką przerwę pomiędzy dźwiękami,
	if(freq == last_note)
	{
		ledc_timer_pause(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0);
		esp_rom_delay_us(1000);
		ledc_timer_resume(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0);
	}

	//	$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	//	Jeśli nie musimy wykonać pauzy to gramy dźwięk,
	if(freq != PAUSE_SONG)
	{
		//PWM_set(BUZZER_LINE, freq, 10, 500);	//	Włączenie generowania fali PWM --> rozpoczęcie wydawania dźwięku,
		PWM_start(&PWM_buzzer, freq, 10, 500);
		time_start = esp_timer_get_time();		//	Pomiar czasu rozpoczęcia generowania dźwięku,
		last_note = freq;						//	Zapis wygenerowanej nuty, jako ostatnio zagranej,

		//ESP_LOGI(TAG, "Czestotliwosc: %d [Hz]", freq);
		//ESP_LOGI(TAG, "Czas trwania: %ld [ms]", duration);
		//ESP_LOGI(TAG, " ");

		return time_start;
	}
	//	$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	//	Generowanie pauzy o wyznaczonym czasie trwania w [us],
	else
	{
		ledc_timer_pause(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0);
		time_start = esp_timer_get_time();		//	Pomiar czasu rozpoczęcia generowania dźwięku,
		last_note = freq;						//	Zapis wygenerowanej nuty, jako ostatnio zagranej,

		return time_start;
		//esp_rom_delay_us(duration);
		//ledc_timer_resume(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0);
	}

	last_note = freq;	//	Zapis wygenerowanej nuty, jako ostatnio zagranej,

	return 0;
}

uint8_t play_song_pooling_(uint8_t mode, const uint16_t song[])
{
	//	-odgrywanie utworu za pomocą metody pooling czyli cyklicznego odpytywania o to czy minął czas trwania generowania danego dźwięku,
	//	-parametr 'mode' oznacza czy odgrywanie ma być kontynuowane, przerwane czy zapauzowane,

	static uint8_t internal_work_mode = STOP_MODE;	//	Zmienna oznaczająca w jakim obecnie trybie znajduje się odtwarzanie dźwięków,
	static uint32_t time_note_min = 0;		//	Długość czasu trwania najkrótszego dźwięku w zależności od tempa utworu,
	static int64_t time_start = 0;			//	Czas rozpoczęcia odgrywania danego dźwięku,
	static uint8_t i = 1;					//	Licznik, który dźwięk odgrywamy,
	static int64_t time_note_old = 0;		//	Długość czasu odgrywania zadanego dźwięku,

	//static uint8_t play_song = 0;
	//static uint8_t mode_internal = 0;

	//	Jeśli otrzymamy informację o zatrzymaniu odtwarzania piosenki z jakiegokolwiek powodu:
	if(mode == STOP_MODE)
	//if(mode == STOP_MODE && mode_internal == 0)
	{
		ledc_timer_pause(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0);	//	Zatrzymujemy generowanie sygnału PWM,
		//play_song = 0;
		i = 1;				//	Licznik dźwięków wraca na pozycję początkową,
		time_note_old = 0;
		//mode_internal = 1;
		internal_work_mode = STOP_MODE;
	}

	//	Przygotowanie do ropoczęcia odtwarzania nowego utworu:
	if( internal_work_mode == STOP_MODE && mode == PLAY_MODE )
	//if(play_song == 0 && mode == PLAY_MODE && internal_work_mode == STOP_MODE)
	{
		//	Obliczenie czasu trwania najkrótszej nuty, wartość tempa pobierana jest z pierwszej komórki tablicy z nutami danej piosenki,
		time_note_min = ( ( ( 1000000 / NOTE_MIN ) * 240 ) / (*song) );
		//play_song = 1;
		//mode_internal = 0;

		internal_work_mode = PLAY_MODE;

		//ESP_LOGI(TAG, "Time note min: %ld [us]", time_note_min);
	}

	//	Sprawdzenie czy można zacząć odtwarzać nowy dźwięk:
	//if(play_song == 1 && mode == PLAY_MODE)
	if(internal_work_mode == PLAY_MODE && mode == PLAY_MODE)
	{
		int64_t time_now = esp_timer_get_time();	//	Pobranie aktualnego czasu,
		int64_t time_diff = time_now - time_start;	//	Obliczenie różnicy czasów: od czasu aktualnego odejmujemy czas rozpoczęcia odtwarzania,
		//ESP_LOGI(TAG, "Time diff : %lld [us]", time_diff);

		//	Sprawdzenie czy minął wymagany czas odtwarzania danego dźwięku tzn czy różnica czasów jest większa od wymaganego czasu trwania dźwięku,
		if(time_diff > time_note_old)
		{
			//	Jeśli tak to mamy dwie opcje, dokonujemy sprawdzenia jak wygląda kolejna nuta do zagrania:
			if(*(song+i) == END_SONG)	//	Jeśli jest to koniec utworu ...
			{
				//play_song = 0;
				internal_work_mode = STOP_MODE;
				i=1;
				ledc_timer_pause(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0);
				return 1;	//	Utwór został zagrany do końca,
			}
			else	//	Jeśli nie jest to koniec utworu to zagrywamy kolejną nutę,
			{
				ledc_timer_resume(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0);
				int64_t time_note = time_note_min * ( NOTE_MIN / (*(song+i+1)) );	//	Obliczenie wymaganego czasu trwania obecnie granego dźwięku,
				//ESP_LOGI(TAG, "Time note : %lld [us]", time_note);
				time_start = play_note_pooling(*(song+i), time_note);	//	Rozpoczynamy odtwarzanie kolejnego dźwięku i zapisujemy od kiedy jest odtwarzany,
				time_note_old = time_note;
				i++;
				i++;
			}
		}
	}

	return 0;
}

uint8_t play_song_pooling(uint8_t mode, uint8_t song_number)
{
	//	-odgrywanie utworu za pomocą metody pooling czyli cyklicznego odpytywania o to czy minął czas trwania generowania danego dźwięku,
	//	-parametr 'mode' oznacza czy odgrywanie ma być kontynuowane, przerwane czy zapauzowane,

	static uint8_t internal_work_mode = STOP_MODE;	//	Zmienna oznaczająca w jakim obecnie trybie znajduje się odtwarzanie dźwięków,
	static uint32_t time_note_min = 0;		//	Długość czasu trwania najkrótszego dźwięku w zależności od tempa utworu,
	static int64_t time_start = 0;			//	Czas rozpoczęcia odgrywania danego dźwięku,
	static uint8_t i = 0;					//	Licznik, który dźwięk odgrywamy,
	static int64_t time_note_old = 0;		//	Długość czasu odgrywania zadanego dźwięku,

	//static uint8_t play_song = 0;
	//static uint8_t mode_internal = 0;

	//	Jeśli otrzymamy informację o zatrzymaniu odtwarzania piosenki z jakiegokolwiek powodu:
	if(mode == STOP_MODE)
	//if(mode == STOP_MODE && mode_internal == 0)
	{
		ledc_timer_pause(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0);	//	Zatrzymujemy generowanie sygnału PWM,
		//play_song = 0;
		i = 0;				//	Licznik dźwięków wraca na pozycję początkową,
		time_note_old = 0;
		//mode_internal = 1;
		internal_work_mode = STOP_MODE;
	}

	//	Przygotowanie do ropoczęcia odtwarzania nowego utworu:
	if( internal_work_mode == STOP_MODE && mode == PLAY_MODE )
	//if(play_song == 0 && mode == PLAY_MODE && internal_work_mode == STOP_MODE)
	{
		//	Obliczenie czasu trwania najkrótszej nuty, wartość tempa pobierana jest z pierwszej komórki tablicy z nutami danej piosenki,
		time_note_min = ( ( ( 1000000 / NOTE_MIN ) * 240 ) / (*(PLAYLIST_ALL+song_number))->tempo );
		//play_song = 1;
		//mode_internal = 0;

		internal_work_mode = PLAY_MODE;

		//ESP_LOGI(TAG, "Time note min: %ld [us]", time_note_min);
	}

	//	Sprawdzenie czy można zacząć odtwarzać nowy dźwięk:
	//if(play_song == 1 && mode == PLAY_MODE)
	if(internal_work_mode == PLAY_MODE && mode == PLAY_MODE)
	{
		int64_t time_now = esp_timer_get_time();	//	Pobranie aktualnego czasu,
		int64_t time_diff = time_now - time_start;	//	Obliczenie różnicy czasów: od czasu aktualnego odejmujemy czas rozpoczęcia odtwarzania,
		//ESP_LOGI(TAG, "Time diff : %lld [us]", time_diff);

		//	Sprawdzenie czy minął wymagany czas odtwarzania danego dźwięku tzn czy różnica czasów jest większa od wymaganego czasu trwania dźwięku,
		if(time_diff > time_note_old)
		{
			//	Jeśli tak to mamy dwie opcje, dokonujemy sprawdzenia jak wygląda kolejna nuta do zagrania:
			if((*(PLAYLIST_ALL+song_number))->notes[i] == END_SONG)	//	Jeśli jest to koniec utworu ...
			{
				//play_song = 0;
				internal_work_mode = STOP_MODE;
				i=0;
				ledc_timer_pause(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0);
				return 1;	//	Utwór został zagrany do końca,
			}
			else	//	Jeśli nie jest to koniec utworu to zagrywamy kolejną nutę,
			{
				ledc_timer_resume(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0);
				int64_t time_note = time_note_min * ( NOTE_MIN / ((*(PLAYLIST_ALL+song_number))->notes[i+1]) );	//	Obliczenie wymaganego czasu trwania obecnie granego dźwięku,
				//ESP_LOGI(TAG, "Time note : %lld [us]", time_note);
				time_start = play_note_pooling((*(PLAYLIST_ALL+song_number))->notes[i], time_note);	//	Rozpoczynamy odtwarzanie kolejnego dźwięku i zapisujemy od kiedy jest odtwarzany,
				time_note_old = time_note;
				i++;
				i++;
			}
		}
	}

	return 0;
}


void play_song_(void)
{

	play_song(&NYAN_CAT_song.notes[0]);

}

*/




//	##############################################################################################################################
/*
void buzzer_TEST(void)
{
	//	-pierwsze odpalenie generowania dźwięku poprzez PWM podawanege na buzzer,

	//PWM_set(BUZZER_LINE, 440, 10, 39000);

	PWM_set(BUZZER_LINE, 100, 10, 512);
}

void buzzer_note(const uint16_t freq, const uint32_t duration)
{

	uint32_t time_us = duration * 1000;

	PWM_set(BUZZER_LINE, freq, 10, 500);
	esp_rom_delay_us(time_us);
}

void buzzer_melody(void)
{

	ledc_timer_resume(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0);

	uint16_t par = 20;

	buzzer_note(294, par*5);
	buzzer_note(330, par*5);
	buzzer_note(350, par*5);
	buzzer_note(392, par*5);
	buzzer_note(330, par*10);
	buzzer_note(262, par*5);
	buzzer_note(294, par*45);

	ledc_timer_pause(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0);

}

void buzzer_melody_2(void)
{

	ledc_timer_resume(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0);

	uint16_t par = 50;

	buzzer_note(C4, par*5);
	buzzer_note(D4, par*5);
	buzzer_note(E4, par*5);
	buzzer_note(F4, par*5);

	buzzer_note(G4, par*10);
	buzzer_note(E4, par*10);

	buzzer_note(G4, par*10);
	buzzer_note(E4, par*10);

	buzzer_note(E4, par*5);
	buzzer_note(E4, par*5);
	buzzer_note(D4, par*5);
	buzzer_note(D4, par*5);

	buzzer_note(C4, par*10);


	ledc_timer_pause(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0);

}




*/







