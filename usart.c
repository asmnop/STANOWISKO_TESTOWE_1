/*
 * usart.c
 *
 *  Created on: 15 wrz 2024
 *      Author: asmnop
 */


#include "../../LIB/Inc/usart.h"
#include "../../LIB/Inc/universal_functions.h"
#include "../../LIB/inc/VT100.h"

//	DMA - WYSYLANIE:
//	-jeśli chcemy wykonać wysył danych z uC na TERMINAL to wykonanie tego jest
//	dopiero opłacalne przy transmisji minimalnie trzech bajtów,
//	-jeśli rejestr nadawczy i bufor wyjściowy jest pusty i chcemy wysłać jeden bajt
//	to wysył zachodzi natychmiast,
//	-jeśli rejestr nadawczy i bufor wyjściowy jest pusty i chcemy wysłać dwa bajty
//	to po załadowaniu pierwszego do TXD jest on przekazywany do bufora, wtedy flaga
//	TXE jest ustawiana i zachodzi zapisanie rejestru TXD,
//	-dopiero przy więcej niż dwóch bajtach zajdzie konieczność wejścia do obsługi
//	przerwania i załadowania TXD nowymi danymi do wysłania,

//	DMA - ODBIERANIE:
//	-jeśli chodzi o odbiór danych wysyłanych z TERMINALA do uC to musieli byśmy
//	zadeklarować stałą długość wysyłanych komend, poleceń, PO CO???


//	////////////////////////////////////////////////////////////////////////////
void USART_init(USART_t *pUSARTx)
{
	//	-procedura inicjalizacji odpalana jest tylko raz, gdyż parametry ramki są
	//	takie same dla nadawania i odbierania,

	//	Ustawienie źródła zegara dla USART:
	//	LL_RCC_USART2_CLKSOURCE_PCLK1	- SYSCLK podzielone przez AHB preskaler oraz APB1 preskaler,
	//	LL_RCC_USART2_CLKSOURCE_SYSCLK
	//	LL_RCC_USART2_CLKSOURCE_HSI		- możliwa obsługa przebudzenia z trybu STOP,
	//	LL_RCC_USART2_CLKSOURCE_LSE		- możliwa obsługa przebudzenia z trybu STOP,

#if defined(STM32G4) || defined(STM32L4)
	LL_RCC_SetUSARTClockSource(pUSARTx->clock_source);	//	RCC->CCIPR
#endif

	//	Włączenie taktowania dla USART:
	//	Włączenie zegara peryferyjnego APB1:
	if(	pUSARTx->periphs == LL_APB1_GRP1_PERIPH_USART2
#if defined(USART3)
			|| pUSARTx->periphs == LL_APB1_GRP1_PERIPH_USART3
#endif /* USART3 */
#if defined(UART4)
		|| pUSARTx->periphs == LL_APB1_GRP1_PERIPH_UART4
#endif /* UART4 */
#if defined(UART5)
		|| pUSARTx->periphs == LL_APB1_GRP1_PERIPH_UART5
#endif /* UART5 */
)
	{
		LL_APB1_GRP1_EnableClock(pUSARTx->periphs);		//	RCC->APB1ENR1
	}
	else if(pUSARTx->periphs == LL_APB2_GRP1_PERIPH_USART1)
	{
		LL_APB2_GRP1_EnableClock(pUSARTx->periphs);		//	RCC->APB2ENR1
	}

	//	Ustawienie parametrów protokołu: wielkość ramki danych, bit parzystości, liczba bitów stopu,
	//	W funkcji konfigurowane są dwa bity w rejestrach: pUSARTx->CR1, pUSARTx->CR2,
	LL_USART_ConfigCharacter(pUSARTx->pUSARTx, pUSARTx->data_width, pUSARTx->parity, pUSARTx->stop_bits);

	//	Ustawienie wartości oversamplingu, preskalera i ostatecznie baud rate:
	//LL_USART_SetOverSampling(USART2, LL_USART_OVERSAMPLING_16);	//	USART_CR1_OVER8
	//LL_USART_SetPrescaler(USART2, LL_USART_PRESCALER_DIV1);

	//	UWAGA NA WARTOŚĆ DRUGIEGO PARAMETRU !!!,
	//	To musi być wartość pochodząca od wybranego zegara!!!
#if defined (STM32L4)
	LL_USART_SetBaudRate(pUSARTx->pUSARTx, SYS_CORE_CLOCK, pUSARTx->over_sampling, pUSARTx->baud_rate);	//	pUSARTx->BRR
#endif

#if defined (STM32F3)
	LL_USART_SetBaudRate(pUSARTx->pUSARTx, SYS_CORE_CLOCK, pUSARTx->over_sampling, pUSARTx->baud_rate);	//	pUSARTx->BRR
#endif

#if defined (STM32G4)
	LL_USART_SetBaudRate(pUSARTx->pUSARTx, SYS_CORE_CLOCK, pUSARTx->over_sampling, LL_USART_OVERSAMPLING_16, pUSARTx->baud_rate);	//	pUSARTx->BRR
#endif

	LL_USART_DisableOverrunDetect(pUSARTx->pUSARTx);
	LL_USART_DisableOneBitSamp(pUSARTx->pUSARTx);

	//	Wybór trybu pracy jako asynchroniczny:
	LL_USART_ConfigAsyncMode(pUSARTx->pUSARTx);	//	pUSARTx->CR2, pUSARTx->CR3

	//	Początkowe wypełnienie tablicy na odbierane dane:
	for(uint8_t i=0; i<sizeof(pUSARTx->RX_data)/sizeof(pUSARTx->RX_data[0]); i++)
	{
		pUSARTx->RX_data[i] = '_';
	}
}

void USART_start(const USART_t *pUSARTx)
{
	//	Włączenie możliwości nadawania i odbierania:
	LL_USART_EnableDirectionTx(pUSARTx->pUSARTx);		//	USART_CR1_TE,
	LL_USART_EnableDirectionRx(pUSARTx->pUSARTx);		//	USART_CR1_RE,

	//LL_USART_SetTransferDirection(pUSARTx->pUSARTx, LL_USART_DIRECTION_TX_RX);

	//	Aktywacja przerwań:
	//	NADAJNIK:
	//	Zdjęcie domyślnie ustawionych flag:
	//LL_USART_RequestTxDataFlush(USART2);
	//LL_USART_ClearFlag_TC(USART2);
	//	Aktywacja przerwań:

	//LL_USART_EnableIT_TC(USART2);

	//LL_USART_RequestTxDataFlush(USART2);
	//LL_USART_ClearFlag_TC(USART2);

	//	ODBIORNIK:
	//LL_USART_EnableIT_RXNE(USART2);	//	Włączenie INT od nadejścia nowych danych,
	//	Po nadejściu nowych danych, wchodzimy do ISR, sprawdzamy flagę czy RXNE = 1,
	//	Jeśli tak to odczytujemy RDR, flaga zostaje zdjęta

	//

	//	Uruchomienie magistrali:
	LL_USART_Enable(pUSARTx->pUSARTx);		//	USART_CR1_UE
}

void USART_stop(const USART_t *pUSARTx)
{
	LL_USART_Disable(pUSARTx->pUSARTx);
	LL_APB1_GRP1_DisableClock(pUSARTx->periphs);
}

void USART_IT_conf(const USART_t *pUSARTx)
{
	//	-przerwania dostępne do obsługi komunikacji protokołem szeregowym USART,
	//	-w uC STM32L476RG mamy do dyspozycji 23 przerwania od USART,
	//	-bity konfiguracyjne są w rejestrze USART_ISR - Interrupt and Status Register,
	//	-po ustawieniu jakiejkolwiek flagi wywoływana jest funkcja USART2_IRQHandler(),
	//	-odczyt, która flaga została ustawiona robiony programowo,
	//	-w zależności, które zdarzenie zaszło musimy wykonać zdjęcie flagi,
	//	-włączenie przerwań dokonywane jest w rejestrze NVIC,
	//	-poszczególne przerwania włączamy w rejestrze USART_CR1

	//	FLAGA TXE - Transmit Data Register Empty:
	//	Informuje, że rejestr danych wysyłanych TDR jest pusty. Pozwala nam określić, kiedy
	//	wrzucić nowe dane do rejestru bez obawy, że poprzednie zostaną nadpisane przed ich
	//	wysłaniem.
	//	-rejestr TDR - Transmit Data Register to rejestr na dane, z którego są przesyłane
	//	do bufora nadawczego jeśli jest on pusty. Dane są przesyłane bajt po bajcie właśnie
	//	z bufora nadawczego,
	//	ANALOGIA: TDR - magazynek na naboje w rewolwerze, bufor - lufa w rewolwerze,
	//	CZYLI: jeśli TDR jest pusty to znaczy, że możemy do niego wpisać dane, możliwość
	//	wpisu danych nie mówi nam nic o tym czy wysył z bufora ma miejsce,
	//	UŻYCIE: jeśli wysyłamy x danych to po wysłaniu danej o numerze x-1 do bufora
	//	nadawczego, nastąpi ustawienie flagi TXE, wtedy ładujemy do TXE daną o numerze x
	//	i możemy uznać, że nastąpiło ostatnie załadowanie danych, pomimo, że są jeszcze
	//	wysyłane z bufora dane x-1 oraz że w TXE czeka jeszcze dana numer x,
	//	UWAGA!!!
	//	Ustawienie flagi nie następuje po wpisie danych do TXE!!! tylko w momencie przesłania
	//	danej z TXE do bufora nadawczego,

	//	FLAGA TC - Transmision Complete:
	//	Informuje, że dane fizycznie zostały wysłane. Generowany jest po znaku STOP w
	//	ramce USART. W większości przypadków może być stosowany zamiennie z TXE, jednak
	//	czasami przydaje się ich rozróżnienie np. w przypadku, gdy chcemy po fizycznym
	//	wysłaniu danych przejść w inny tryb pracy na danym pinie.
	//	CZYLI informacja o końcu transmisji danych i zadanej ilości
	//	ALEEEEEE - czy znak STOP jest ustawiany po zakończeniu transmisji z bufora nadawaczego
	//	czy może po przesłaniu ostatniej danej z TDR do bufora??????????

	//	FLAGA RXNE - Read Data Register Not Empty:
	//	Informuje, że w rejestrze RDR znajdują się dane. Dzięki temu wiemy, że ramka została
	//	odebrana i możemy odczytać je z rejestru odbiorczego.
	//	CZYLI po prostu nadeszły nowe dane do odczytu.
	//	PYTANIE: jaka jest pojemność pojemnika na odebrane dane?????

	//	FLAGA IDLE
	//	Informuje, żę zakończyła się transmisja danych na linii RX. Bardzo przydatne
	//	przerwanie w przypadku odbierania danych, kiedy nie mamy określonej długości
	//	ramki, a chcemy wiedzieć, kiedy dane przestały przychodzić do magistrali USART.
	//	CZYLI jak jakichś slave do nas wysyła dane to my nie wiemy ile ich jest, a ustawienie
	//	flagi nam to wykrywa. ALE w jaki sposób? WYDAJE MI SIĘ że po odebraniu danej jest
	//	zliczany jakichś czas i jeśli zostanie przekroczony to uznawane jest, że koniec
	//	transmisji nastąpił,
	//	ALE co jeśli ten czas jest zbyt mały i jeszcze przyjdą dane należące do już zakończonej
	//	ramki danych????


	//	AKTYWACJA PRZERWAŃ POCHODZĄCYCH OD NADAJNIKA:
	//	Zdjęcie domyślnie ustawionych flag:
	LL_USART_RequestTxDataFlush(USART2);
	//LL_USART_ClearFlag_TC(USART2);
	//	Aktywacja przerwań:

	//LL_USART_ClearFlag_TXE(pUSARTx->pUSARTx);
	/////////////LL_USART_EnableIT_TXE(pUSARTx->pUSARTx);	//	USART_CR1_TXEIE
	//LL_USART_EnableIT_TC(pUSARTx->pUSARTx);

	//LL_USART_RequestTxDataFlush(USART2);
	//LL_USART_ClearFlag_TC(pUSARTx->pUSARTx);

	//	AKTYWACJA PRZERWAŃ POCHODZĄCYCH OD ODBIORNIKA:
	LL_USART_EnableIT_RXNE(pUSARTx->pUSARTx);	//	Włączenie INT od nadejścia nowych danych,

	//	Po nadejściu nowych danych, wchodzimy do ISR, sprawdzamy flagę czy RXNE = 1,
	//	Jeśli tak to odczytujemy RDR, flaga zostaje zdjęta

	//LL_USART_EnableIT_IDLE(pUSARTx->pUSARTx);
	//LL_USART_EnableIT_TXE(pUSARTx->pUSARTx);	//	USART_CR1_TXEIE

}


//	////////////////////////////////////////////////////////////////////////////
void USART_RX_new_data(USART_t *pUSART, const uint8_t data)
{
	//	-zapis nowo odebranej danej do tablicy z odebranymi danymi,
	//	-nowa dana jest zapisywana do najwyższej komórki pamięci,
	//	-podczas zapisu w tablicy dane są przesuwane w kierunku do niższej komórki,

	//	Przepisanie danych o jedną komórkę w dół dla wszystkich danych oprócz jednej:
	for(uint8_t i=0; i<(pUSART->buf-1); i++)
	{
		pUSART->RX_data[i] = pUSART->RX_data[i+1];
	}

	pUSART->RX_data[pUSART->buf-1] = data;
}

uint8_t USART_get_data(const USART_t *pUSART)
{
	//	-funkcja zwraca najnowszą odebraną daną,
	//	-dana ta znajduje się w ostatniej komórce tablicy odebranych danych,

	return pUSART->RX_data[pUSART->buf - 1];
}

uint8_t USART_check_command(const USART_t *pUSART, const char command[], const uint8_t length)
{
	//	-sprawdzenie czy łańcuch znaków w tablicy odebranych danych, która jest
	//	częścią struktury 'USART_t' jest zgodny z łańcuchem znaków 'command' o długości 'length',
	//	-wartość 'length' może być mniejsza od 'pUSART->buf' ale nie może być większa,
	//	-zmienna 'command_ending' określa zawartość pola w programie ROBOREMO --> interface --> Command ending
	//	-jest to znak lub łańcuch znaków umieszczany na końcu każdej komendy wysłanej z ROBOREMO,
	//	-ilości tej zmiennej nie uwzględniamy w wartości argumentu 'length',

	uint8_t match = 0;
	const char command_ending = '\n';

	//	Sprawdzenie czy odebrany łańcuch znaków zgadza się z komendą:
	for(uint8_t i=0; i<length; i++)
	{
		if(command[i] == pUSART->RX_data[pUSART->buf-length-1 + i])
		{
			match++;

			if(match == length)	//	Jeżeli ilość zgodnych znaków jest równa ilości wymaganej,
			{
				//	Sprawdzamy czy został odebrany znak kończący:
				if(pUSART->RX_data[pUSART->buf-1] == command_ending)
				{
					return 0;	//	Odebrano poprawne dane :)
				}
				else
				{
					return 1;
				}
			}
		}
		else
		{
			return 1;
		}
	}

	return 1;
}

uint8_t USART_check_string(USART_t *pUSART, const char command[], const uint8_t length)
{
	//	-sprawdzenie czy łańcuch znaków w tablicy odebranych danych, która jest
	//	częścią struktury 'USART_t' jest zgodny z łańcuchem znaków 'command' o długości 'length',
	//	-wartość 'length' może być mniejsza od 'pUSART->buf' ale nie może być większa,
	//	-zmienna 'command_ending' określa zawartość pola w programie ROBOREMO --> interface --> Command ending
	//	-jest to znak lub łańcuch znaków umieszczany na końcu każdej komendy wysłanej z ROBOREMO,
	//	-ilości tej zmiennej nie uwzględniamy w wartości argumentu 'length',

	uint8_t match = 0;

	//	Sprawdzenie czy odebrany łańcuch znaków zgadza się z komendą:
	for(uint8_t i=0; i<length; i++)
	{
		if(command[i] == pUSART->RX_data[pUSART->buf-length + i])
		{
			match++;

			if(match == length)	//	Jeżeli ilość zgodnych znaków jest równa ilości wymaganej,
			{
				return 0;	//	Odebrano poprawne dane :)
			}
		}
		else
		{
			return 1;
		}
	}

	return 1;
}


void USART_set_flag_new_data(USART_t *pUSART)
{
	pUSART->flag = 1;
}

void USART_reset_flag_new_data(USART_t *pUSART)
{
	pUSART->flag = 0;
}

uint8_t USART_get_flag_new_data(const USART_t *pUSART)
{
	return pUSART->flag;
}


//	////////////////////////////////////////////////////////////////////////////
void USART_TX_char(const USART_t *pUSART, const char data)
{
	//	-wysłanie pojedynczego znaku po USART w trybie POLLING,

	while(!LL_USART_IsActiveFlag_TXE(pUSART->pUSARTx));

	LL_USART_TransmitData8(pUSART->pUSARTx, data);
}

void USART_TX_string(const USART_t *pUSART, const char *string)
{
	//	-wysłanie łańcucha znaków po USART w trybie POLLING,

	while(*string)
	{
		USART_TX_char(pUSART, *string++);
	}
}

void USART_TX_number(const USART_t *pUSART, const int16_t number)
{
	//	-wysłanie liczby po USART w trybie POLLING,

	char buffer[17];
	itoa(number, buffer, 10);
	USART_TX_string(pUSART, buffer);
}

void USART_TX_unumber(const USART_t *pUSART, const uint16_t number)
{
	//	-wysłanie liczby po USART w trybie POLLING,

	char buffer[17];
	utoa(number, buffer, 10);
	USART_TX_string(pUSART, buffer);
}


void USART_TX_char_IT(const USART_t *pUSART, const char data)
{
	//	-wysłanie pojedynczego znaku po USART za pomocą przerwań,
	//	-skoro mamy wysłać tylko jeden znak to nie musimy włączać
	//	przerwań, bo musielibyśmy jako argument przekazywać tablicę
	//	dwuelementową,
	//	UWAGA!!! - argument 'data' jest przekazywany jako kopia
	//	więc jeśli byśmy użyli 'pUSART->ptr = &data;' to po wyjściu
	//	z funkcji wskaźnik wskazywałby w nieznane miejsce w pamięci,

	while(pUSART->ptr != NULL);
	while(!LL_USART_IsActiveFlag_TXE(pUSART->pUSARTx));

	LL_USART_TransmitData8(pUSART->pUSARTx, data);
}

void USART_TX_string_IT(USART_t *pUSART, char *string)
{
	//	-wysłanie łańcucha znaków po USART za pomocą przerwań,
	//	-ustawiamy wskaźnik na początek nowego łańcucha znaków do wysłania,
	//	-wysyłanie można zainicjować jedynie, jeżeli żaden łańcuch znaków nie
	//	jest wysyłany, czyli jeżeli wskaźnik wskazuje na NULL,

	while(!(pUSART->ptr == NULL));

	pUSART->ptr = string;
	LL_USART_EnableIT_TXE(pUSART->pUSARTx);
}

void USART_TX_number_IT(USART_t *pUSART, const int16_t number)
{
	//	-wysłanie liczby po USART w trybie przerwań,

	char buffer[17];
	itoa(number, buffer, 10);
	USART_TX_string_IT(pUSART, buffer);
}


void USART_change_ptr(USART_t *pUSART)
{
	//	-przygotowanie wskaźnika na kolejny adres z pod którego
	//	będzie wysyłana zawartość,


	if(*(pUSART->ptr) != '\0')
	//if((*pUSART).ptr != 0)
	{
		pUSART->ptr++;
	}
	else
	{
		pUSART->ptr = NULL;
		LL_USART_DisableIT_TXE(pUSART->pUSARTx);
	}

}

void USART_control_panel(USART_t *pUSART, void (*foo_1)(void), void (*foo_2)(void), void (*foo_3)(void), void (*foo_4)(void))
{
	//	====================================================
	//	OBSŁUGA NOWO ODEBRANYCH DANYCH PO USART Z TERMINALA:
	//	====================================================
	if(USART_get_flag_new_data(pUSART) == 1)
	{
		uint8_t data_read = USART_get_data(pUSART);	//	Pobranie ostatnio odebranej danej,

		if(data_read == 'w')
		{
			foo_1();
			USART_reset_flag_new_data(pUSART);
		}
		else if(data_read == 's')
		{
			foo_2();
			USART_reset_flag_new_data(pUSART);
		}
		else if(data_read == 'a')
		{
			foo_3();
			USART_reset_flag_new_data(pUSART);
		}
		else if(data_read == 'd')
		{
			foo_4();
			USART_reset_flag_new_data(pUSART);
		}
	}
}






//	############################################################################
/*
 *



void USART_init_clock(void)
{
	//	-procedura inicjalizacji odpalana jest tylko raz, gdyż parametry ramki są
	//	takie same dla nadawania i odbierania,

	//	Ustawienie źródła zegara dla USART:
	//	LL_RCC_USART2_CLKSOURCE_PCLK1	- SYSCLK podzielone przez AHB preskaler oraz APB1 preskaler,
	//	LL_RCC_USART2_CLKSOURCE_SYSCLK
	//	LL_RCC_USART2_CLKSOURCE_HSI		- możliwa obsługa przebudzenia z trybu STOP,
	//	LL_RCC_USART2_CLKSOURCE_LSE		- możliwa obsługa przebudzenia z trybu STOP,

#if defined(STM32G4)
	LL_RCC_SetUSARTClockSource(LL_RCC_USART2_CLKSOURCE_PCLK1);	//	RCC->CCIPR
#endif

	//	Włączenie taktowania dla USART:
	//	Włączenie zegara peryferyjnego APB1:

	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);		//	RCC->APB1ENR1


	//	Ustawienie parametrów protokołu: wielkość ramki danych, bit parzystości
	//	liczba bitów stopu,
	//	W funkcji konfigurowane są dwa bity w rejestrach: pUSARTx->CR1, pUSARTx->CR2,
	LL_USART_ConfigCharacter(USART2, LL_USART_DATAWIDTH_8B, LL_USART_PARITY_NONE, LL_USART_STOPBITS_1);

	//	Ustawienie wartości oversamplingu, preskalera i ostatecznie baud rate:
	//LL_USART_SetOverSampling(USART2, LL_USART_OVERSAMPLING_16);	//	USART_CR1_OVER8
	//LL_USART_SetPrescaler(USART2, LL_USART_PRESCALER_DIV1);

	//	UWAGA NA WARTOŚĆ DRUGIEGO PARAMETRU !!!,
	//	To musi być wartość pochodząca od wybranego zegara!!!
#if defined (STM32L4)
	LL_USART_SetBaudRate(USART2, SYS_CORE_CLOCK, LL_USART_OVERSAMPLING_16, 230400);	//	pUSARTx->BRR
#endif
	LL_USART_DisableOverrunDetect(USART2);
	LL_USART_DisableOneBitSamp(USART2);
	LL_USART_ConfigAsyncMode(USART2);	//	Wybór trybu pracy jako asynchroniczny -- pUSARTx->CR2, pUSARTx->CR3



	//	Włączenie możliwości nadawania i odbierania:
	LL_USART_EnableDirectionTx(USART2);		//	USART_CR1_TE,
	LL_USART_EnableDirectionRx(USART2);		//	USART_CR1_RE,
	//	Uruchomienie magistrali,

	LL_USART_ClearFlag_FE(USART2);
	LL_USART_ClearFlag_ORE(USART2);
	LL_USART_ClearFlag_IDLE(USART2);
	LL_USART_ClearFlag_NE(USART2);
	LL_USART_RequestRxDataFlush(USART2);

	LL_USART_Enable(USART2);				//	USART_CR1_UE
	LL_USART_ClearFlag_FE(USART2);

	//LL_USART_EnableIT_TXE(USART2);
	LL_USART_EnableIT_RXNE(USART2);
}









 * //	////////////////////////////////////////////////////////////////////////////
uint8_t DMA_buffer[1000] = {"DMA - wysylanie\n\r"};

void USART_DMA_MemToPer(void)
{
	//	Uruchomienie zegara dla DMA1:
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

	//	Konfiguracja parametrów pracy DMA1, ustawiamy źródło dla kanału 7,
#if defined (STM32L4)
	LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_7, LL_DMA_REQUEST_2);
#endif
	//	Ustawienie kierunku transmisji:
	LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_7, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

	//	Ustawienie priorytetu:
	LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_7, LL_DMA_PRIORITY_HIGH);

	//	Ustawienie trybu pracy:
	LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_7, LL_DMA_MODE_NORMAL);

	//	Czy adresy mają być inkrementowane czy nie:
	LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_7, LL_DMA_PERIPH_NOINCREMENT);
	LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_7, LL_DMA_MEMORY_INCREMENT);

	//	Jaki jest rozmiar pojedynczego elementu w transmisji po stronie peryferium i pamięci
	LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_7, LL_DMA_PDATAALIGN_BYTE);
	LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_7, LL_DMA_MDATAALIGN_BYTE);

	//	Konfiguracja i włączenie przerwania od transmisji całej ramki:
	NVIC_SetPriority(DMA1_Channel7_IRQn, 4);
	NVIC_EnableIRQ(DMA1_Channel7_IRQn);
	LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_7);
	LL_DMA_EnableIT_HT(DMA1, LL_DMA_CHANNEL_7);

	//	Ustawiamy adres źródłowy, z którego będą pobierane dane do wysłania:
	LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_7, (uint32_t)DMA_buffer,
		LL_USART_DMA_GetRegAddr(USART2, LL_USART_DMA_REG_DATA_TRANSMIT),
		LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_7));

	//	Informacja ile danych będziemy przesyłać, wartość jest dekrementowana:
	//	Dane są pobierane z 'SrcAddress', my określamy ile ich należy pobrać,
	//	Po przesłaniu danych jest generowanie przerwanie od TC:
	LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_7, sizeof(DMA_buffer));

	//	Włączamy przerwanie od DMA dla wysyłu danych:
	LL_USART_EnableDMAReq_RX(USART2);
	LL_USART_EnableDMAReq_TX(USART2);

	//	Włączamy kanał 7 w DMA1:
	LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_7);

}

 *
 *
 *
void USART_TX_Polling(void)
{
	#define USART3_TX_PORT GPIOB
	#define USART3_TX_PIN LL_GPIO_PIN_9

	#define USART3_RX_PORT GPIOB
	#define USART3_RX_PIN LL_GPIO_PIN_8

#if defined(STM32G4)
	LL_AHB2_GRP1_EnableClock     (LL_AHB2_GRP1_PERIPH_GPIOB);
#endif

	LL_GPIO_SetPinOutputType     (GPIOB, LL_GPIO_PIN_9, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull           (GPIOB, LL_GPIO_PIN_9, LL_GPIO_PULL_NO);
	LL_GPIO_SetPinSpeed          (GPIOB, LL_GPIO_PIN_9, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetAFPin_8_15        (GPIOB, LL_GPIO_PIN_9, LL_GPIO_AF_7);
	LL_GPIO_SetPinMode           (GPIOB, LL_GPIO_PIN_9, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetOutputPin         (GPIOB, LL_GPIO_PIN_9);

	LL_GPIO_SetPinOutputType     (GPIOB, LL_GPIO_PIN_8, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull           (GPIOB, LL_GPIO_PIN_8, LL_GPIO_PULL_NO);
	LL_GPIO_SetPinSpeed          (GPIOB, LL_GPIO_PIN_8, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetAFPin_8_15        (GPIOB, LL_GPIO_PIN_8, LL_GPIO_AF_7);
	LL_GPIO_SetPinMode           (GPIOB, LL_GPIO_PIN_8, LL_GPIO_MODE_ALTERNATE);
	LL_GPIO_SetOutputPin         (GPIOB, LL_GPIO_PIN_8);

#if defined(STM32G4)
	LL_RCC_SetUSARTClockSource   (LL_RCC_USART3_CLKSOURCE_PCLK1);
#endif
	LL_APB1_GRP1_EnableClock     (LL_APB1_GRP1_PERIPH_USART3);
	LL_USART_ConfigCharacter     (USART3, LL_USART_DATAWIDTH_8B, LL_USART_PARITY_NONE, LL_USART_STOPBITS_1);
#if defined(STM32G4)
	LL_USART_SetBaudRate         (USART3, SYS_CORE_CLOCK, LL_USART_PRESCALER_DIV1, LL_USART_OVERSAMPLING_16, 19200);
#endif
	LL_USART_DisableOverrunDetect(USART3);
	LL_USART_DisableOneBitSamp   (USART3);
	LL_USART_ConfigAsyncMode     (USART3);

	LL_USART_Enable              (USART3);
	LL_USART_EnableDirectionTx   (USART3);
	LL_USART_EnableDirectionRx   (USART3);



	#define SIZE 6
	uint8_t data_to_send[SIZE] = "led 0\n";

	for(uint8_t i=0; i<SIZE; i++)
	{
		 while(!LL_USART_IsActiveFlag_TXE(USART3));

		 LL_USART_TransmitData8(USART3, data_to_send[i]);
	}

	for(uint8_t i=0; i<SIZE; i++)
	{
		 while(!LL_USART_IsActiveFlag_TXE(USART3));

		 LL_USART_TransmitData8(USART3, data_to_send[i]);
	}
}



*/




void USART3_init(uint32_t baud_rate)
{
	LL_USART_Disable(USART1);
	LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_USART1);		//	RCC->APB1ENR1


	//	-procedura inicjalizacji odpalana jest tylko raz, gdyż parametry ramki są
	//	takie same dla nadawania i odbierania,

	//	Ustawienie źródła zegara dla USART_2:
	LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_PCLK2);	//	RCC->CCIPR

	//	Włączenie taktowania dla USART2:
	//	Włączenie zegara peryferyjnego APB1:
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);		//	RCC->APB1ENR1

	//	Ustawienie parametrów protokołu: wielkość ramki danych, bit parzystości
	//	liczba bitów stopu,
	//	W funkcji konfigurowane są dwa bity w rejestrach: pUSARTx->CR1, pUSARTx->CR2,
	LL_USART_ConfigCharacter(USART1, LL_USART_DATAWIDTH_8B, LL_USART_PARITY_NONE, LL_USART_STOPBITS_1);

	//	Ustawienie wartości oversamplingu, preskalera i ostatecznie baud rate:
	//LL_USART_SetOverSampling(USART2, LL_USART_OVERSAMPLING_16);	//	USART_CR1_OVER8
	//LL_USART_SetPrescaler(USART2, LL_USART_PRESCALER_DIV1);

	//	UWAGA NA WARTOŚĆ DRUGIEGO PARAMETRU !!!
#ifdef STM32L4
	LL_USART_SetBaudRate(USART1, SYS_CORE_CLOCK, LL_USART_OVERSAMPLING_16, baud_rate);	//	pUSARTx->BRR
#endif

#ifdef STM32G4
	LL_USART_SetBaudRate(USART1, SYS_CORE_CLOCK, LL_USART_PRESCALER_DIV1, LL_USART_OVERSAMPLING_16, baud_rate);	//	pUSARTx->BRR

#endif

	LL_USART_DisableOverrunDetect(USART1);
	LL_USART_DisableOneBitSamp(USART1);

	//	Wybór trybu pracy jako asynchroniczny:
	LL_USART_ConfigAsyncMode(USART1);	//	pUSARTx->CR2, pUSARTx->CR3

	LL_USART_EnableDirectionTx(USART1);		//	USART_CR1_TE,
	LL_USART_EnableDirectionRx(USART1);		//	USART_CR1_RE,
	LL_USART_Enable(USART1);
}

void USART3_start(void)
{
	//LL_USART_ClearFlag_TC(USART3);
	//	Włączenie możliwości nadawania i odbierania:
	LL_USART_EnableDirectionTx(USART1);		//	USART_CR1_TE,
	LL_USART_EnableDirectionRx(USART1);		//	USART_CR1_RE,

	//	Aktywacja przerwań:
	//	NADAJNIK:
	//	Zdjęcie domyślnie ustawionych flag:
	//LL_USART_RequestTxDataFlush(USART2);
	//LL_USART_ClearFlag_TC(USART2);
	//	Aktywacja przerwań:

	//LL_USART_EnableIT_TC(USART2);

	//LL_USART_RequestTxDataFlush(USART2);
	//LL_USART_ClearFlag_TC(USART2);

	//	ODBIORNIK:
	//LL_USART_EnableIT_RXNE(USART2);	//	Włączenie INT od nadejścia nowych danych,
	//	Po nadejściu nowych danych, wchodzimy do ISR, sprawdzamy flagę czy RXNE = 1,
	//	Jeśli tak to odczytujemy RDR, flaga zostaje zdjęta

	//

	//	Uruchomienie magistrali,
	LL_USART_Enable(USART1);				//	USART_CR1_UE



}

/*

void USART_start(void)
{
	//	Włączenie możliwości nadawania i odbierania:
	LL_USART_EnableDirectionTx(USART2);		//	USART_CR1_TE,
	LL_USART_EnableDirectionRx(USART2);		//	USART_CR1_RE,

	//	Aktywacja przerwań:
	//	NADAJNIK:
	//	Zdjęcie domyślnie ustawionych flag:
	//LL_USART_RequestTxDataFlush(USART2);
	//LL_USART_ClearFlag_TC(USART2);
	//	Aktywacja przerwań:

	//LL_USART_EnableIT_TC(USART2);

	//LL_USART_RequestTxDataFlush(USART2);
	//LL_USART_ClearFlag_TC(USART2);

	//	ODBIORNIK:
	//LL_USART_EnableIT_RXNE(USART2);	//	Włączenie INT od nadejścia nowych danych,
	//	Po nadejściu nowych danych, wchodzimy do ISR, sprawdzamy flagę czy RXNE = 1,
	//	Jeśli tak to odczytujemy RDR, flaga zostaje zdjęta

	//

	//	Uruchomienie magistrali,
	LL_USART_Enable(USART2);				//	USART_CR1_UE

	//SET_BIT(USART2->CR1, USART_CR1_UE);
	//USART2->CR1 |= USART_CR1_UE;
	//USART2->CR1 = USART2->CR1 | USART_CR1_UE;



}





void USART_init(void)
{
	//	-procedura inicjalizacji odpalana jest tylko raz, gdyż parametry ramki są
	//	takie same dla nadawania i odbierania,

	//	Ustawienie źródła zegara dla USART_2:
	LL_RCC_SetUSARTClockSource(LL_RCC_USART2_CLKSOURCE_PCLK1);	//	RCC->CCIPR

	//	Włączenie taktowania dla USART2:
	//	Włączenie zegara peryferyjnego APB1:
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);		//	RCC->APB1ENR1

	//	Ustawienie parametrów protokołu: wielkość ramki danych, bit parzystości
	//	liczba bitów stopu,
	//	W funkcji konfigurowane są dwa bity w rejestrach: pUSARTx->CR1, pUSARTx->CR2,
	LL_USART_ConfigCharacter(USART2, LL_USART_DATAWIDTH_8B, LL_USART_PARITY_NONE, LL_USART_STOPBITS_1);

	//	Ustawienie wartości oversamplingu, preskalera i ostatecznie baud rate:
	//LL_USART_SetOverSampling(USART2, LL_USART_OVERSAMPLING_16);	//	USART_CR1_OVER8
	//LL_USART_SetPrescaler(USART2, LL_USART_PRESCALER_DIV1);

	//	UWAGA NA WARTOŚĆ DRUGIEGO PARAMETRU !!!
	LL_USART_SetBaudRate(USART2, SystemCoreClock, LL_USART_OVERSAMPLING_16, 115200);	//	pUSARTx->BRR

	//	Wybór trybu pracy jako asynchroniczny:
	LL_USART_ConfigAsyncMode(USART2);	//	pUSARTx->CR2, pUSARTx->CR3
}


void USART_RX_pin_init(void)
{
	//LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);	//	RCC->AHB2ENR

	LL_GPIO_SetPinPull(USART2_RX_GPIO_Port, USART2_RX_Pin, LL_GPIO_PULL_NO);
	LL_GPIO_SetPinSpeed(USART2_RX_GPIO_Port, USART2_RX_Pin, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetAFPin_0_7(USART2_RX_GPIO_Port, USART2_RX_Pin, LL_GPIO_AF_7);
	LL_GPIO_SetPinMode(USART2_RX_GPIO_Port, USART2_RX_Pin, LL_GPIO_MODE_ALTERNATE);
}

void USART_TX_pin_init(void)
{
	//LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);	//	RCC->AHB2ENR

	LL_GPIO_SetPinPull(USART2_TX_GPIO_Port, USART2_TX_Pin, LL_GPIO_PULL_NO);
	LL_GPIO_SetPinSpeed(USART2_TX_GPIO_Port, USART2_TX_Pin, LL_GPIO_SPEED_FREQ_LOW);
	LL_GPIO_SetAFPin_0_7(USART2_TX_GPIO_Port, USART2_TX_Pin, LL_GPIO_AF_7);
	LL_GPIO_SetPinMode(USART2_TX_GPIO_Port, USART2_TX_Pin, LL_GPIO_MODE_ALTERNATE);
}
*/





