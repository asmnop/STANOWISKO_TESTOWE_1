/*
 * encoder.c
 *
 *  Created on: 2 lip 2024
 *      Author: asmnop
 */

#include "..\..\LIB\Inc\encoder.h"
#include "../../LIB/Inc/universal_data.h"

//	INFO:
//

//	INFORMACJE:
//	-dwa główne rodzaje enkoderów: inkrementalne (przyrostowe) i absolutne,
//	-inkrementalne zliczają impulsy, ale nie znają kątowego położenia wału, na którym jest zamontowany,
//	-absolutne znają położenie kątowe wału, wartość nie zanika po ponownym włączeniu zasilania,
//	-jeśli sygnał dociera z nadajnika do odbiornika to wystawiany stan niski '0', dioda LED świeci,
//	-jeśli sygnał nie dociera z nadajnika do odbiornika to wystawiany stan wysoki '1', dioda LED zgaszona,
//	-dla tarczy z jednym wycięciem na wejściu do uC przeważnie panuje stan wysoki,
//	-z tego wynika, że zbocze opadające jest sygnałem o zajściu pełnego obrotu,
//	-może się zdarzyć, że pozycją startową tarczy enkodera z jendym wycięciem będzie szczelina, wtedy
//	jako pierwsze pojawi się zbocze rosnące,
//	-coś takiego jak drganie styków podczas normalnej prędkości obrotowej nie zachodzi!!!,
//	-drganie styków może występować jeśli krawędź pomiędzy polami stoi w miejscu działania transoptora szczelinowego,

//	DO CZEGO MOŻNA UŻYĆ:
//	-pomiar prędkości obrotowej wału silnika, na którym jest zamocowany enkoder,
//	-za powyższym idzie także obliczenie pokonanej drogi lub obliczenie przyśpieszenia,
//	-pokrętło generujące sygnał do obsługi menu,
//	-ustalanie położenia kątowego danego podzespołu, do którego jest zamocowany enkoder,
//	-ustalanie położenia liniowego karetki w drukarce,
//	-informacja o położeniu głowicy w gaźniku,

//	POMIAR ILOŚCI ZMIAN STANU NA LINIACH:
//	-konfigurujemy timer do generowania przerwania co określony przedział czasu,
//	-przedział czasu nazywamy czasem próbkowania, sample time, skrót ST,
//	-w obsłudze przerwania dokonujemy odczytu zliczonych zmian stanu,
//	-może wystąpić sytuacja, że nastąpi przejście z 0 na CPR lub z CPR na 0,
//	-takie zdarzenie jest obsługiwane w przerwaniu,
//	-w przerwaniu jest badany kierunek obrotu podczas przejścia w interesującej nas chwili,
//	-jeżeli nastąpi przejście z CPR na 0 to zwiększamy licznik,
//	-jeżeli nastąpi przejście z 0 na CPR to zmniejszamy licznik,
//	-wartość licznika jest zerowana w obsłudze przerwania od czasu próbkowania,
//	-ilość zmian stanu na liniach jest zliczana przez licznik,
//	-

//	POMIAR PRĘDKOŚCI:
//	-do zmierzenia prędkości obrotowej potrzebujemy dwie zmienne: czas oraz liczba impulsów,
//	-dla najdokładniejszego pomiaru prędkości każde obliczenie wykonujemy po zmianie zliczonej
//	liczby impulsów tzn. przerwanie w przypadku zmiany stanu na linii sygnałowej,
//	-co określoną jednostkę czasu odpalane jest przerwanie, w którym jest odczytywana ilość impulsów,
//	---ryzyko jeśli nastąpi zmiana kierunku obrotów i suma impulsów wyniesie zero, a w rzeczywistości wał
//	silnika zacznie obracać się w przeciwnym kierunku,

//	PRZYDATNE PARAMETRY:
//	-kierunek obrotu zakończonej sekwencji,
//	-rozpoczęty kierunek obrotu,
//	-czy rozpoczęty kierunek obrotu zakończył się prawidłowo

//	PARAMETRY:
//	PPR - Pulses Per Revolution
//	CPR - Counts Per Revolution
//	https://www.rls.si/eng/faq/index/show/cat_id/2/faq/59/
//	https://eltra-encoder.eu/news/encoder-ppr-and-cpr

//	DZIALANIA:
//	-podłączenie zegarów dla wybranych peryferiów TIM oraz PWM,
//	-konfiguracja pinów w trybie AF,
//	-wybór trybu pracy enkodera: 3 możliwości,
//	-konfiguracja preskalera, ARR oraz filtrów,
//	-przepisanie wartości z rejestrów xxx do rejestrów buforowych,
//	-uruchomienie licznika,

//	UWAGI:
//	-w trybie standardowym nie potrzebujemy włączeć jakichkolwiek przerwań,
//	-jeśli potrzebujemy informację o zajściu pełnego obrotu, a raczej informacji o
//	przepełnieniu licznika to ustawiamy zezwolenie na przerwanie od UPDATE,
//	-jeśli chcemy zliczać kroki do wybranej wartości to najpierw ustalamy położenie
//	tarczy enkodera po czym włączamy odpowiednią opcję generowania przerwań od
//	wybranej zmiany stanu na wybranej linii, wtedy włączamy licznik tych przerwań
//	i po osiągnięciu zadanej wartości wyłączamy silnik,
//	można też zmniejszać prędkość silnika przy dochodzeniu do zadanej wartości obrotów
//	lub impulsów z enkodera,
//	-po zakończeniu zliczania zerujemy parametry generowania przerwań,

//	SPOSOBY PRACY:
//	-do pracy jako wybieracz opcji nie ustawiamy zadnych przerwań do aktywacji,
//	-do pracy jako ręczny generator wypełnienia do sterowania silnikiem wybieramy
//	opcję z ustaloną wartością ARR, która jest równa ilości zliczeń od osiągnięcia
//	maksymalnej prędkości obrotowej,
//	-do pracy jako wykonywacz zadanej ilości króków do wybranej wartości to najpierw ustalamy położenie
//	tarczy enkodera po czym włączamy odpowiednią opcję generowania przerwań od
//	wybranej zmiany stanu na wybranej linii, wtedy włączamy licznik tych przerwań
//	i po osiągnięciu zadanej wartości wyłączamy silnik,
//	-do pracy w ramach algorytmu PID ustawiamy tylko przerwanie od UPDATE aby zliczać
//	pełne obroty tarczy enkodera,


//	############################################################################
void ENCODER_init(ENCODER_t *pEncoder)
{
	//	-inicjalizacja enkodera do podstawowego trybu pracy,
	//	-jest to zliczanie impulsów do zadanej wartości, po której następuje
	//	aktualizacja wartości licznika,
	//	nie są tutaj włączane żadne przerwania,

	//	Ustawienie trybu enkodera:
	//	SMS = '001' - licznik licza tylko zbocza na T2I,
	//	SMS = '010' - licznik licza tylko zbocza na T1I,
	//	SMS = '011' - licznik licza zarówno zbocza na T1I oraz T2I,

	//	Wybór polaryzacji wejścia: CC1P oraz CC2P w CCER
	//	CC1NP oraz CC2NP muszą być równe 0
	//	Kiedy jest to niezbędne to możemy zaprogramować filtr wejściowy, CC1P oraz CC2P na 0

	//	Dwa wejścia T1 i T2 są używane jako enkoder inkrementalny
	//	Licznik jest zliczany po każdej poprawnej transmisji na TI1FP1 i TI2FP2

	/*
	• CC1S = 01 (TIMx_CCMR1 register, TI1FP1 mapped on TI1)
	• CC2S = 01 (TIMx_CCMR1 register, TI2FP2 mapped on TI2)
	• CC1P and CC1NP = ‘0’ (TIMx_CCER register, TI1FP1 noninverted, TI1FP1=TI1)
	• CC2P and CC2NP = ‘0’ (TIMx_CCER register, TI2FP2 noninverted, TI2FP2=TI2)
	• SMS= 011 (TIMx_SMCR register, both inputs are active on both rising and falling
	edges)
	• CEN= 1 (TIMx_CR1 register, Counter is enabled)
	*/

	/*
	Aby wybrać tryb interfejsu enkodera, wpisz SMS='001' w rejestrze TIMx_SMCR, jeśli
	licznik zlicza tylko zbocza TI2, SMS=010, jeśli zlicza tylko zbocza TI1 i SMS=011,
	jeśli zlicza zarówno zbocza TI1, jak i TI2.

	Wybierz polaryzację TI1 i TI2, programując bity CC1P i CC2P w rejestrze TIMx_CCER.
	CC1NP i CC2NP muszą być puste. W razie potrzeby można również zaprogramować filtr
	wejściowy. CC1NP i CC2NP muszą być niskie.

	Dwa wejścia TI1 i TI2 służą do interfejsu z enkoderem przyrostowym. Zobacz Tabelę 202.
	Licznik jest taktowany przez każde prawidłowe przejście na TI1FP1 lub TI2FP2 (TI1 i TI2
	po filtrze wejściowym i wyborze polaryzacji, TI1FP1=TI1 jeśli nie jest filtrowane i nie
	jest odwrócone, TI2FP2=TI2 jeśli nie jest filtrowane i nie jest odwrócone) zakładając,
	że jest włączone (bit CEN w rejestrze TIMx_CR1 zapisany na ‘1). Sekwencja przejść dwóch
	wejść jest oceniana i generuje impulsy zliczania, a także sygnał kierunku. W zależności
	od sekwencji, w jakiej licznik zlicza w górę lub w dół, bit DIR w rejestrze TIMx_CR1
	jest odpowiednio modyfikowany przez sprzęt. Bit DIR jest obliczany przy każdym przejściu
	na dowolnym wejściu (TI1 lub TI2), bez względu na to, czy licznik zlicza tylko na TI1,
	tylko na TI2 czy zarówno na TI1, jak i TI2.

	Tryb interfejsu enkodera działa po prostu jako zewnętrzny zegar z wyborem kierunku.
	Oznacza to, że licznik po prostu ciągle liczy między 0 a wartością automatycznego
	przeładowania w rejestrze TIMx_ARR (od 0 do ARR lub ARR w dół do 0 w zależności
	od kierunku). Dlatego TIMx_ARR musi zostać skonfigurowany przed rozpoczęciem.
	W ten sam sposób funkcje przechwytywania, porównywania, preskalera i wyzwalania
	wyjścia nadal działają normalnie. W tym trybie licznik jest modyfikowany automatycznie
	zgodnie z prędkością i kierunkiem enkodera kwadraturowego, a jego zawartość zawsze
	reprezentuje pozycję enkodera.
	Kierunek zliczania odpowiada kierunkowi obrotu podłączonego czujnika. Tabela podsumowuje
	możliwe kombinacje, zakładając, że TI1 i TI2 nie przełączają się w tym samym czasie.

	Zewnętrzny enkoder inkrementalny można podłączyć bezpośrednio do MCU bez zewnętrznej
	logiki interfejsu. Jednak komparatory są zwykle używane do konwersji różnicowych wyjść
	enkodera na sygnały cyfrowe. Znacznie zwiększa to odporność na zakłócenia. Trzecie
	wyjście enkodera, które wskazuje mechaniczną pozycję zerową, może być podłączone
	do zewnętrznego wejścia przerwania i wyzwalać reset licznika.

	Rysunek 321 przedstawia przykład działania licznika, pokazując generowanie sygnału zliczania i sterowanie kierunkiem. Pokazuje również, jak kompensowany jest jitter wejściowy, gdy wybrane są obie krawędzie. Może się to zdarzyć, jeśli czujnik jest umieszczony w pobliżu jednego z punktów przełączania. W tym przykładzie zakładamy, że konfiguracja jest następująca:
	• CC1S= 01 (rejestr TIMx_CCMR1, TI1FP1 mapowany na TI1)
	• CC2S= 01 (rejestr TIMx_CCMR1, TI2FP2 mapowany na TI2)
	• CC1P i CC1NP = „0” (rejestr TIMx_CCER, TI1FP1 nieodwrócony, TI1FP1=TI1)
	• CC2P i CC2NP = „0” (rejestr TIMx_CCER, TI2FP2 nieodwrócony, TI2FP2=TI2)
	• SMS= 011 (rejestr TIMx_SMCR, oba wejścia są aktywne zarówno na zboczach narastających, jak i opadających)
	• CEN= 1 (rejestr TIMx_CR1, licznik jest włączony)

	Timer, gdy jest skonfigurowany w trybie Encoder Interface, dostarcza informacji o
	aktualnej pozycji czujnika. Informacje dynamiczne (prędkość, przyspieszenie,
	zwalnianie) można uzyskać, mierząc okres między dwoma zdarzeniami enkodera za
	pomocą drugiego timera skonfigurowanego w trybie przechwytywania. Wyjście enkodera,
	które wskazuje zero mechaniczne, może być wykorzystane w tym celu. W zależności od
	czasu między dwoma zdarzeniami licznik może być również odczytywany w regularnych
	odstępach czasu. Można to zrobić, zatrzaskując wartość licznika w trzecim rejestrze
	przechwytywania wejściowego, jeśli jest dostępny (wtedy sygnał przechwytywania musi
	być okresowy i może być generowany przez inny timer). Jeśli jest dostępny, możliwe
	jest również odczytanie jego wartości za pomocą żądania DMA generowanego przez zegar
	czasu rzeczywistego.

	*/

	GPIO_set_pin_as_input(pEncoder->ENCO_A);
	GPIO_set_pin_as_input(pEncoder->ENCO_B);

	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM4);		//	RCC->APB1ENR1
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM5);		//	RCC->APB1ENR1

	//	Skoro licznik zlicza kroki enkodera to może doliczyć do pewnej wartości po czym zlicza od nowa,
	LL_TIM_SetAutoReload(pEncoder->TIMx, pEncoder->CPR-1);	//	TIMx->ARR

	//	Skoro mamy zamiar zliczać impulsy z enkodera to musimy uaktywnić kanał na jaki
	//	będą one podawane oraz źródło tych impulsów,
	//	Musimy wybrać kierunek kanału: weście albo wyjście oraz jeśli wybierzemy wejście to
	//	co będzie wchodzić do kanału,
	//	Wybieramy to w rejestrze SMCR, bity CCxS,
	//	Wybór CCxS determinuje kierunek kanału,
	//	Dla CH1:
	//	DIRECTTI - IC1 jest mapowane przez TI1,
	//	INDIRECTTI - IC1 jest mapowane przez TI2,
	//	Dla CH2:
	//	DIRECTTI - IC2 jest mapowane przez TI2,
	//	INDIRECTTI - IC2 jest mapowane przez TI1,
	LL_TIM_IC_SetActiveInput(pEncoder->TIMx, LL_TIM_CHANNEL_CH1, LL_TIM_ACTIVEINPUT_DIRECTTI);
	LL_TIM_IC_SetActiveInput(pEncoder->TIMx, LL_TIM_CHANNEL_CH2, LL_TIM_ACTIVEINPUT_DIRECTTI);
	//TIM4->CCMR1 |= (TIM_CCMR1_CC1S_0 | TIM_CCMR1_CC2S_0);

	//TIM4->CCMR1 &=~(3UL << CC1S);
	//TIM4->CCMR1 |= (1UL << CC1S);

	LL_TIM_IC_SetFilter(pEncoder->TIMx, LL_TIM_CHANNEL_CH1, LL_TIM_IC_FILTER_FDIV32_N8);
	LL_TIM_IC_SetFilter(pEncoder->TIMx, LL_TIM_CHANNEL_CH2, LL_TIM_IC_FILTER_FDIV32_N8);

	//	Skoro wybraliśmy kanał jako wejście to musimy ustawić zbocza wyzwalające zliczanie:
	//	3. Select the edge of the active transition on the TIx channel by writing the CC1P and
	//	CC1NP and CC1NP bits to 000 in the TIMx_CCER register (rising edge in this case).
	////////////LL_TIM_IC_SetPolarity(pEncoder->TIMx, LL_TIM_CHANNEL_CH1, LL_TIM_IC_POLARITY_RISING);
	/////////LL_TIM_IC_SetPolarity(pEncoder->TIMx, LL_TIM_CHANNEL_CH2, LL_TIM_IC_POLARITY_RISING);
	//TIM4->CCER &= ~(TIM_CCER_CC1P | TIM_CCER_CC2P);

	//LL_TIM_IC_SetPrescaler(TIM4, LL_TIM_CHANNEL_CH1, LL_TIM_ICPSC_DIV2);
	//LL_TIM_IC_SetPrescaler(TIM4, LL_TIM_CHANNEL_CH2, LL_TIM_ICPSC_DIV2);

	//LL_TIM_OC_SetCompareCH1(pEncoder->TIMx, 1);
	/////////LL_TIM_CC_EnableChannel(pEncoder->TIMx, LL_TIM_CHANNEL_CH1);

	//LL_TIM_OC_SetCompareCH2(pEncoder->TIMx, 6);
	//////////LL_TIM_CC_EnableChannel(pEncoder->TIMx, LL_TIM_CHANNEL_CH2);

	//LL_TIM_EnableIT_CC1(pEncoder->TIMx);
	//LL_TIM_EnableIT_CC2(pEncoder->TIMx);
	//LL_TIM_EnableIT_UPDATE(pEncoder->TIMx);


	//	Zezwolenie na generowanie zdarzenia od odświerzenia - update,
	//	Po zezwoleniu mamy trzy opcje generowania i te trzy opcje je generują
	LL_TIM_EnableUpdateEvent(pEncoder->TIMx);

	//	ALE nie wszytkie muszą wtedy generować przerwanie i to ustawiamy poniżej,

	//	Wybór źródła zajścia przerwania,
	//	LL_TIM_UPDATESOURCE_REGULAR - odświerzenie zachodzi i flaga zajścia przerwania
	//	jest ustawiana dla trzech zdarzeń,
	//	LL_TIM_UPDATESOURCE_COUNTER - odświerzenie zachodzi ALE flaga zajścia
	//	przerwania jest generowana tylko dla przepełnienia lub niedopełnienia,
	LL_TIM_SetUpdateSource(pEncoder->TIMx, LL_TIM_UPDATESOURCE_COUNTER);



	//	Włączenie buforowania rejestru ARR, czyli aktualizacja podczas między innymi wymuszenia,
	LL_TIM_EnableARRPreload(pEncoder->TIMx);


	LL_TIM_SetEncoderMode(pEncoder->TIMx, LL_TIM_ENCODERMODE_X4_TI12);

	LL_TIM_EnableCounter(pEncoder->TIMx);
}

void ENCODER_force_update(ENCODER_t *pEncoder)
{
	//	-wymuszenie zajścia UPDATE,
	//	-wykorzystywane do aktualizacji parametrów sterujących,

	LL_TIM_GenerateEvent_UPDATE(pEncoder->TIMx);	//	Wymuszenie UPDATE,
	LL_TIM_ClearFlag_UPDATE(pEncoder->TIMx);		//	Zdjęcie ewentualnie ustawionej flagi,
}

void ENCODER_set_IT(ENCODER_t *pEncoder, uint32_t channel, uint32_t edge_polarity)
{
	//	-ustawienie na jakim kanale i jakie zbocze będzie generowało przerwanie,
	//	-w przerwaniu można zliczać ilość przerwań,

	LL_TIM_IC_SetPolarity(pEncoder->TIMx, channel, edge_polarity);
	LL_TIM_CC_EnableChannel(pEncoder->TIMx, channel);

	if(channel == LL_TIM_CHANNEL_CH1)
	{
		LL_TIM_EnableIT_CC1(pEncoder->TIMx);
	}
	else if(channel == LL_TIM_CHANNEL_CH2)
	{
		LL_TIM_EnableIT_CC2(pEncoder->TIMx);
	}
	else if(channel == LL_TIM_CHANNEL_CH3)
	{
		LL_TIM_EnableIT_CC3(pEncoder->TIMx);
	}
	else if(channel == LL_TIM_CHANNEL_CH4)
	{
		LL_TIM_EnableIT_CC4(pEncoder->TIMx);
	}
}


//	############################################################################
uint16_t ENCODER_get_counts_in_sample_time(ENCODER_t *pEncoder)
{
	//	-wyznaczenie ilości zmian stanu na liniach sygnałowych enkodera w zadanym czasie próbkowania,
	//	-jest najbardziej pierwotne wyliczenie danych, które mogą być zwrócone po upłynięciu czasu próbkowania,
	//	-w algorytmie PID można podawać tą liczbę impulsów LUB przekształconą wartość w RPS albo RPM albo drogę albo V,

	static uint32_t rp_sample_time_prev = 0;		//	Liczba zmian stanu zliczona w czasie poprzedniego samplowania,

	LL_TIM_DisableIT_UPDATE(pEncoder->TIMx);
	uint32_t rp_sample_time = LL_TIM_GetCounter(pEncoder->TIMx);	//	Obecnie zliczone zmiany stanu w czasie ST,
	//uint32_t dir = LL_TIM_GetDirection(pEncoder->TIMx);
	int8_t UPDATE_counter_tmp = ENCO_get_UPDATE_counter(pEncoder);	//	Liczba zajść przerwania od UPDATE w czasie ST,
	LL_TIM_EnableIT_UPDATE(pEncoder->TIMx);

	ENCO_reset_UPDATE_counter(pEncoder);
	uint32_t measurment = 0;	//	Całkowita liczba zmian stanu w czasie trwania samplowania,

	//	OBLICZENIE ILOŚCI ZMIAN STANU NA LINIACH ENKODERA:
	if(UPDATE_counter_tmp == 0)	//	Jeżeli nie nastąpiło przejście z 0-->CPR ani z CPR-->0,
	{
		if(rp_sample_time_prev > rp_sample_time)
		{
			measurment = rp_sample_time_prev - rp_sample_time;
		}
		else if(rp_sample_time_prev < rp_sample_time)
		{
			measurment = rp_sample_time - rp_sample_time_prev;
		}
		else
		{
			measurment = 0;
		}
	}
	else if(UPDATE_counter_tmp > 0)
	{
		measurment = rp_sample_time - rp_sample_time_prev + (UPDATE_counter_tmp * pEncoder->CPR);
	}
	else if(UPDATE_counter_tmp < 0)
	{
		UPDATE_counter_tmp = UPDATE_counter_tmp * (-1);
		measurment = rp_sample_time_prev - rp_sample_time + (UPDATE_counter_tmp * pEncoder->CPR);
	}

	/*
	if(UPDATE_counter > 0)	//	Obroty na pewno były w CW,
	{
		measurment = pEncoder->CPR - rp_sample_time_old + UPDATE_counter * pEncoder->CPR + rp_sample_time;
	}
	else if(UPDATE_counter < 0)	//	Obroty na pewno były w CCW,
	{
		measurment = rp_sample_time_old + ( UPDATE_counter * ((-1) * pEncoder->CPR) ) + rp_sample_time - pEncoder->CPR;
	}
	else
	{
		measurment = rp_sample_time - rp_sample_time_old;
		if(measurment < 0)
		{
			measurment = measurment * (-1);
		}
	}
	*/

	/*
	else if(dir == LL_TIM_COUNTERDIRECTION_UP)
	{
		measurment = rp_sample_time - rp_sample_time_old;
	}
	else
	{
		measurment = rp_sample_time_prev - rp_sample_time;
	}
*/
	//////////////////////////////////////////////////////////////////////
	/*
	if( (ENCO_get_dir(pEncoder) == 0) && (rp_sample_time > rp_sample_time_old) )
	{
		measurment = rp_sample_time - rp_sample_time_old;
	}
	else if( (ENCO_get_dir(pEncoder) == 0) && (rp_sample_time < rp_sample_time_old) )
	{
		measurment = pEncoder->CPR - rp_sample_time + rp_sample_time_old;
	}
	else if( (ENCO_get_dir(pEncoder) == 1) && (rp_sample_time < rp_sample_time_old) )
	{
		measurment = pEncoder->CPR - rp_sample_time + rp_sample_time_old;
	}
	else if( (ENCO_get_dir(pEncoder) == 1) && (rp_sample_time > rp_sample_time_old) )
	{
		measurment = rp_sample_time - rp_sample_time_prev;
	}
*/

	rp_sample_time_prev = rp_sample_time;

	return measurment;
}

uint8_t ENCO_get_start_position(ENCODER_t *pEncoder)
{
	//	-odczyt pozycji w jakiej znajduje się tarcza enkodera,
	//	-pozycja oznacza: pozycja = (A, B),
	//	0 = (0, 0)
	//	1 = (0, 1)
	//	2 = (1, 1)
	//	3 = (1, 0)

	uint8_t input_A_state = GPIO_get_input_pin(pEncoder->ENCO_A);
	uint8_t input_B_state = GPIO_get_input_pin(pEncoder->ENCO_B);

	///////return (input_A_state << 1 || input_B_state << 0);


	if( (input_A_state == 0) && (input_B_state == 0) )
	{
		return 0;
	}

	if( (input_A_state == 0) && (input_B_state == 1) )
	{
		return 1;
	}

	if( (input_A_state == 1) && (input_B_state == 1) )
	{
		return 2;
	}

	if( (input_A_state == 1) && (input_B_state == 0) )
	{
		return 3;
	}


/*
	if( (GPIO_get_input_pin(pEncoder->ENCO_A) == 0) && (GPIO_get_input_pin(pEncoder->ENCO_B) == 0) )
	{
		return 0;
	}

	if( (GPIO_get_input_pin(pEncoder->ENCO_A) == 0) && (GPIO_get_input_pin(pEncoder->ENCO_B) == 1) )
	{
		return 1;
	}

	if( (GPIO_get_input_pin(pEncoder->ENCO_A) == 1) && (GPIO_get_input_pin(pEncoder->ENCO_B) == 1) )
	{
		return 2;
	}

	if( (GPIO_get_input_pin(pEncoder->ENCO_A) == 1) && (GPIO_get_input_pin(pEncoder->ENCO_B) == 0) )
	{
		return 3;
	}
*/

	return 66;
}

void ENCO_set_ENCODER_parameters(ENCODER_t *pEncoder, const uint32_t direction, const uint32_t counts)
{
	//	-ustawienie parametrów ekodera w zależności od danych ruchu,

	uint32_t arr_CW[4][2] = {
		{LL_TIM_CHANNEL_CH1, LL_TIM_IC_POLARITY_RISING},
		{LL_TIM_CHANNEL_CH2, LL_TIM_IC_POLARITY_RISING},
		{LL_TIM_CHANNEL_CH1, LL_TIM_IC_POLARITY_FALLING},
		{LL_TIM_CHANNEL_CH2, LL_TIM_IC_POLARITY_FALLING},
	};

	uint32_t arr_CCW[4][2] = {
		{LL_TIM_CHANNEL_CH2, LL_TIM_IC_POLARITY_RISING},
		{LL_TIM_CHANNEL_CH1, LL_TIM_IC_POLARITY_RISING},
		{LL_TIM_CHANNEL_CH2, LL_TIM_IC_POLARITY_FALLING},
		{LL_TIM_CHANNEL_CH1, LL_TIM_IC_POLARITY_FALLING},
	};

	uint8_t reszta = counts % 4;
	int8_t false_pointer = 0;
	uint8_t base = ENCO_get_start_position(pEncoder);

	if(direction == CW)
	{
		false_pointer = reszta - base - 1;

		/*
		if(base == 0)
		{
			false_pointer = reszta - 1;
		}

		if(base == 1)
		{
			false_pointer = reszta - 2;
		}

		if(base == 2)
		{
			false_pointer = reszta - 3;
		}

		if(base == 3)
		{
			false_pointer = reszta - 4;
		}
		*/

		if(false_pointer < 0)
		{
			false_pointer = false_pointer + 4;
		}

		LL_TIM_IC_SetPolarity(pEncoder->TIMx, arr_CW[false_pointer][0], arr_CW[false_pointer][1]);
		LL_TIM_CC_EnableChannel(pEncoder->TIMx, arr_CW[false_pointer][0]);

		if(arr_CW[false_pointer][0] == LL_TIM_CHANNEL_CH1)
		{
			LL_TIM_EnableIT_CC1(pEncoder->TIMx);
		}
		else
		{
			LL_TIM_EnableIT_CC2(pEncoder->TIMx);
		}
	}

	if(direction == CCW)
	{
		false_pointer = reszta + base - 1;

		/*
		if(base == 0)
		{
			false_pointer = reszta - 1;
		}

		if(base == 1)
		{
			false_pointer = reszta - 0;
		}

		if(base == 2)
		{
			false_pointer = reszta + 1;
		}

		if(base == 3)
		{
			false_pointer = reszta + 2;
		}
		*/

		if(false_pointer < 0)
		{
			false_pointer = false_pointer + 4;
		}

		LL_TIM_IC_SetPolarity(pEncoder->TIMx, arr_CCW[false_pointer][0], arr_CCW[false_pointer][1]);
		LL_TIM_CC_EnableChannel(pEncoder->TIMx, arr_CCW[false_pointer][0]);

		if(arr_CCW[false_pointer][0] == LL_TIM_CHANNEL_CH1)
		{
			LL_TIM_EnableIT_CC1(pEncoder->TIMx);
		}
		else
		{
			LL_TIM_EnableIT_CC2(pEncoder->TIMx);
		}
	}

	LL_TIM_DisableIT_UPDATE(pEncoder->TIMx);

	if(counts < 5)
	{
		pEncoder->get_counts = 1;
	}
	else
	{
		pEncoder->get_counts = (counts >> 2) + 1;
	}
}

uint16_t ENCO_get_rps(ENCODER_t *pEncoder)
{
	//	-obliczenie liczby obrotów na sekundę na podstawie zliczonej ilości zmian stanu w zadanym czasie samplowania,

	pEncoder->rps = ((pEncoder->counts * 1000) / pEncoder->sample_time) / pEncoder->CPR;

	return pEncoder->rps;
}


//	############################################################################
uint8_t ENCODER_as_key(ENCODER_t *pEncoder, void(*foo)(void), void(*foo_2)(void))
{
	//	-sprawdzenie czy nie zaszło wykonanie jednego kroku,
	//	-wykonanie akcji w zależności od kierunku wykonanego kroku,

	static uint8_t steps_pulser = 0;
	static uint8_t steps_pulser_old = 0;

	//if(ENCO_get_flag(pEncoder) == 1)
	{
		steps_pulser = ENCO_get_steps(pEncoder);

		if(steps_pulser%4 == 0)
		{
			ENCO_reset_flag(pEncoder);
			if( ((steps_pulser > steps_pulser_old) && (steps_pulser - steps_pulser_old == 4)) \
					|| (steps_pulser == 0 && steps_pulser_old == pEncoder->CPR-4 ) )
			{
				foo();
				steps_pulser_old = steps_pulser;
				return 1;
			}
			else if( ((steps_pulser < steps_pulser_old) && (steps_pulser_old - steps_pulser == 4)) \
					|| (steps_pulser == pEncoder->CPR-4 && steps_pulser_old == 0) )
			{
				foo_2();
				steps_pulser_old = steps_pulser;
				return 1;
			}

			steps_pulser_old = steps_pulser;
			return 0;
		}
	}

	return 2;
}


//	############################################################################
void ENCO_set_steps(ENCODER_t *pEncoder, const uint32_t steps)
{
	if(steps < pEncoder->CPR)
	{
		pEncoder->counts = steps;
		LL_TIM_SetCounter(pEncoder->TIMx, steps);
	}
}

void ENCO_reset_steps(ENCODER_t *pEncoder)
{
	pEncoder->counts = 0;
	LL_TIM_SetCounter(pEncoder->TIMx, 0);
}

uint16_t ENCO_get_steps(ENCODER_t *pEncoder)
{
	return pEncoder->counts = LL_TIM_GetCounter(pEncoder->TIMx);
}


void ENCO_set_flag(ENCODER_t *pEncoder)
{
	pEncoder->flag = 1;
}

void ENCO_reset_flag(ENCODER_t *pEncoder)
{
	pEncoder->flag = 0;
}

uint8_t ENCO_get_flag(ENCODER_t *pEncoder)
{
	return pEncoder->flag;
}


void ENCO_set_UPDATE_counter(ENCODER_t *pEncoder, const int32_t UPDATE_counter)
{
	pEncoder->UPDATE_counter = UPDATE_counter;
}

void ENCO_reset_UPDATE_counter(ENCODER_t *pEncoder)
{
	pEncoder->UPDATE_counter = 0;
}

int8_t ENCO_get_UPDATE_counter(ENCODER_t *pEncoder)
{
	return pEncoder->UPDATE_counter;
}


void ENCODER_set_CPR(ENCODER_t *pEncoder, const uint16_t CPR)
{
	//	-funkcja ustawiająca do jakiej wartości ma zliczać licznik enkodera,
	//	-zakładamy, że zawartość ARR jest buforowana, więc do aktualizacji
	//	potrzeba wywołać ręcznie odświerzenie,
	//	-dodatkowo zerowany jest licznik enkodera, ponieważ po odświerzeniu
	//	wartość CNT jest ustawiana w zależności od DIR,
	//	-parametry dla, których podmiana działa:
	//	LL_TIM_SetUpdateSource(pEncoder->TIMx, LL_TIM_UPDATESOURCE_COUNTER);
	//	LL_TIM_EnableUpdateEvent(pEncoder->TIMx);
	//	LL_TIM_EnableARRPreload(pEncoder->TIMx);

	pEncoder->CPR = CPR;
	LL_TIM_SetAutoReload(pEncoder->TIMx, pEncoder->CPR-1);	//	TIMx->ARR

	ENCODER_force_update(pEncoder);	//	Wymuszenie aktualizacji i zdjęcie flagi,

	//	Wymuszony UPDATE ustawia wartość CNT w zależności od DIR, musimy CNT wyzerować,
	ENCO_reset_steps(pEncoder);
	ENCO_reset_UPDATE_counter(pEncoder);
}


void TIM_Encoder_2(void)
{
	//	Tryb enkoder - w tym trybie licznik zlicza więc praca jako slave
	//	Skoro zlicza to tryb mode 1????
	//	Skoro zlicza to zlicza do jakiejś wartości czyli ARR
	//	Skoro zlicza to może nastąpić zgodne porównanie i przerwania od tego,


	//	Ustawienie trybu enkodera:
	//	SMS = '001' - licznik licza tylko zbocza na T2I,
	//	SMS = '010' - licznik licza tylko zbocza na T1I,
	//	SMS = '011' - licznik licza tylko zbocza na T1I oraz T2I,

	//	Wybór polaryzacji wejścia: CC1P oraz CC2P w CCER
	//	CC1NP oraz CC2NP muszą być równe 0
	//	Kiedy jest to niezbędne to możemy zaprogramować filtr wejściowy, CC1P oraz CC2P na 0

	//	Dwa wejścia T1 i T2 są używane jako enkoder inkrementalny
	//	Licznik jest zliczany po każdej poprawnej transmisji na TI1FP1 i TI2FP2

	/*
	• CC1S= 01 (TIMx_CCMR1 register, TI1FP1 mapped on TI1)
	• CC2S= 01 (TIMx_CCMR1 register, TI2FP2 mapped on TI2)
	• CC1P and CC1NP = ‘0’ (TIMx_CCER register, TI1FP1 noninverted, TI1FP1=TI1)
	• CC2P and CC2NP = ‘0’ (TIMx_CCER register, TI2FP2 noninverted, TI2FP2=TI2)
	• SMS= 011 (TIMx_SMCR register, both inputs are active on both rising and falling
	edges)
	• CEN= 1 (TIMx_CR1 register, Counter is enabled)
	*/

	////////////////////////////////////////////////////////////////////////

	LL_TIM_SetEncoderMode(TIM4, LL_TIM_ENCODERMODE_X4_TI12);

	LL_TIM_SetClockSource(TIM4, LL_TIM_CLOCKSOURCE_EXT_MODE1);	//	TIM_SMCR_SMS | TIM_SMCR_ECE
	//LL_TIM_SetAutoReload(TIM4, 100);	//	TIMx->ARR

	//	1. Select the active input: TIMx_CCR1 must be linked to the TI1 input, so write the CC1S
	//	bits to 01 in the TIMx_CCMR1 register. As soon as CC1S becomes different from 00,
	//	the channel is configured in input and the TIMx_CCR1 register becomes read-only.
	LL_TIM_IC_SetActiveInput(TIM4, LL_TIM_CHANNEL_CH3, LL_TIM_ACTIVEINPUT_DIRECTTI);
	LL_TIM_IC_SetActiveInput(TIM4, LL_TIM_CHANNEL_CH4, LL_TIM_ACTIVEINPUT_DIRECTTI);

	//	2. Program the appropriate input filter duration in relation with the signal connected to the
	//	timer (when the input is one of the TIx (ICxF bits in the TIMx_CCMRx register). Let’s
	//	imagine that, when toggling, the input signal is not stable during at must 5 internal clock
	//	cycles. We must program a filter duration longer than these 5 clock cycles. We can
	//	validate a transition on TI1 when 8 consecutive samples with the new level have been
	//	detected (sampled at fDTS frequency). Then write IC1F bits to 0011 in the
	//	TIMx_CCMR1 register.
	LL_TIM_IC_SetFilter(TIM4, LL_TIM_CHANNEL_CH3, LL_TIM_IC_FILTER_FDIV1);
	LL_TIM_IC_SetFilter(TIM4, LL_TIM_CHANNEL_CH4, LL_TIM_IC_FILTER_FDIV1);


	//	3. Select the edge of the active transition on the TI1 channel by writing the CC1P and
	//	CC1NP and CC1NP bits to 000 in the TIMx_CCER register (rising edge in this case).
	LL_TIM_IC_SetPolarity(TIM4, LL_TIM_CHANNEL_CH3, LL_TIM_IC_POLARITY_RISING);
	LL_TIM_IC_SetPolarity(TIM4, LL_TIM_CHANNEL_CH4, LL_TIM_IC_POLARITY_RISING);

	//	4. Program the input prescaler. In our example, we wish the capture to be performed at
	//	each valid transition, so the prescaler is disabled (write IC1PS bits to 00 in the
	//	TIMx_CCMR1 register).
	LL_TIM_IC_SetPrescaler(TIM4, LL_TIM_CHANNEL_CH3, LL_TIM_ICPSC_DIV1);
	LL_TIM_IC_SetPrescaler(TIM4, LL_TIM_CHANNEL_CH4, LL_TIM_ICPSC_DIV1);

	///////////////////////////////////////////////////////////////
	//LL_TIM_SetClockSource(TIM3, LL_TIM_CLOCKSOURCE_INTERNAL);
	LL_TIM_SetTriggerInput(TIM4, LL_TIM_TS_TI1FP1);
	LL_TIM_SetTriggerInput(TIM4, LL_TIM_TS_TI2FP2);

	//	5. Enable capture from the counter into the capture register by setting the CC1E bit in the
	//	TIMx_CCER register
	LL_TIM_CC_EnableChannel(TIM4, LL_TIM_CHANNEL_CH3);
	LL_TIM_CC_EnableChannel(TIM4, LL_TIM_CHANNEL_CH4);

	//	6. If needed, enable the related interrupt request by setting the CC1IE bit in the
	//	TIMx_DIER register, and/or the DMA request by setting the CC1DE bit in the
	//	TIMx_DIER register.

	//	Podpięcie zegara:
	//LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);

	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM4);			//	RCC->APB1ENR1


	//	Ustawienie źródła zegara:
	LL_TIM_SetClockSource(TIM4, LL_TIM_CLOCKSOURCE_INTERNAL);	//	TIM_SMCR_SMS | TIM_SMCR_ECE

	//	Kierunek zliczania:
	LL_TIM_SetCounterMode(TIM4, LL_TIM_COUNTERMODE_UP);			//	TIM_CR1_DIR | TIM_CR1_CMS

	//	Wartość preskalera:
	LL_TIM_SetPrescaler(TIM4, 0);		//	TIM_CR1_DIR | TIM_CR1_CMS

	//	Obliczenie wartości rejestru automatycznego przeładowania:
	//uint32_t timer_arr = __LL_TIM_CALC_ARR(10000000, 10000, 2);

	//	Zapis do rejestru ARR. Wpisana wartość musi zostać jeszcze załadowana do rejestru cieni:
	LL_TIM_SetAutoReload(TIM4, 100);	//	TIMx->ARR

	LL_TIM_DisableARRPreload(TIM4);
	LL_TIM_SetTriggerOutput(TIM4, LL_TIM_TRGO_RESET);
	LL_TIM_DisableMasterSlaveMode(TIM4);

	LL_TIM_EnableCounter(TIM4);
}








