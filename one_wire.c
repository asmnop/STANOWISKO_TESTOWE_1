/*
 * one_wire.c
 *
 *  Created on: 22 wrz 2024
 *      Author: asmnop
 */


#include "../../LIB/Inc/one_wire.h"


#include "../../LIB/Inc/universal_functions.h"
#include "../../LIB/Inc/VT100.h"

/*
 * ONE WIRE:
 * -jest to interfejs komunikacyjny,
 * -do transmisji potrzebna jest jedna linia danych,
 * -do linii można podłączyć wiele czujników, które mają unikalne adresy,
 * -w transmisji nie jest wykorzystywana żadna linia zegarowa - komunikacja asynchroniczna,
 * -z tego powodu bardzo ważne są czasy trwania sekwencji nadawania,
 * -jedna linia służy zarówno do wysyłania jak i odbierania danych,
 * -piny w uC są ustawiane w tryb 'open drain' z zewnętrznym rezystorem podciągającym,
 * -domyślny stan na wejściu to stan wysoki,
 * -układy podłączone do OW mogą wymuszać stan niski,
 * ***-kontrola obecności czujnika poprzez sprawdzenie czy odpowiada stanem niskim,
 * -występuje podział na dwa rodzaje urządzeń: nadrzędne Master i podrzędne Slave,
 * -urządzenie Master kontroluje transmisję i zawsze inicjalizuje komunikację,
 * -układ Slave tylko odpowiada na żądania wysyłane przez Master,
 * -transmisja danych odbywa się za pomocą 8-bitowych bajtów,
 * -kolejność od LSB - najmniej znaczącego bitu,
 *
 * -do transmisji można wykorzystać interfejs USART,
 * -w takim przypadku ważne jest aby linię TX i RX ustawić jako wyjściowe, bez rezystorów podciągających,
 * -TX ustawić jako: LL_GPIO_OUTPUT_PUSHPULL,
 * -RX ustawić jako: LL_GPIO_OUTPUT_OPENDRAIN,
 *
 * SEKWENCJE KOMUNIKACYJNE:
 * RESET:
 * -informacja dla układów Slave o rozpoczęciu transmisji,
 *
 *
 * PYTANIA:
 * -którą bibliotekę traktować jako położoną niżej, USART czy 1-Wire?
 * -lub może utworzyć dwie osobne, a wybór będzie dokonywany w obsłudze czujnika?
 *
 *
 *
 */



void set_TX(LINE_t *line)
{
	LL_GPIO_SetPinOutputType(line->port, line->pin, LL_GPIO_OUTPUT_OPENDRAIN);
	LL_GPIO_SetPinPull(line->port, line->pin, LL_GPIO_PULL_NO);
	LL_GPIO_SetPinSpeed(line->port, line->pin, LL_GPIO_SPEED_FREQ_VERY_HIGH);
	if(line->pin <= LL_GPIO_PIN_7)	LL_GPIO_SetAFPin_0_7(line->port, line->pin, line->mode);
	if(line->pin >= LL_GPIO_PIN_8)	LL_GPIO_SetAFPin_8_15(line->port, line->pin, line->mode);
	LL_GPIO_SetPinMode(line->port, line->pin, LL_GPIO_MODE_ALTERNATE);
}

void set_RX(LINE_t *line)
{
	LL_GPIO_SetPinOutputType(line->port, line->pin, LL_GPIO_OUTPUT_PUSHPULL);
	LL_GPIO_SetPinPull(line->port, line->pin, LL_GPIO_PULL_NO);
	LL_GPIO_SetPinSpeed(line->port, line->pin, LL_GPIO_SPEED_FREQ_VERY_HIGH);
	if(line->pin <= LL_GPIO_PIN_7)	LL_GPIO_SetAFPin_0_7(line->port, line->pin, line->mode);
	if(line->pin >= LL_GPIO_PIN_8)	LL_GPIO_SetAFPin_8_15(line->port, line->pin, line->mode);
	LL_GPIO_SetPinMode(line->port, line->pin, LL_GPIO_MODE_ALTERNATE);
}

void OW_init(LINE_t *line, LINE_t *line2)
{
	//	-inicjalizacja interfejsu 1-wire,
	//	-linia konfigurowana jako wyjście 'open drain' ze stanem wysokim,
	//	-konfiguracja timera, który służy do odmierzania czasu rzędu us,

	set_TX(line);
	set_RX(line2);
	USART3_init(9600);
	USART3_start();
}

uint8_t OW_reset(USART_t *pUSART)
{
	//	-podczas sekwencji inicjalizacyjnej na linii, MASTER ustawia stan niski na minimalny czas 480 [us],
	//	-nastepnie MASTER zwalnia szynę danych i przechodzi do trybu odbiorczego,
	//	-kiedy szyna jest zwolniona, rezystor podciągający o wartości 5k podciąga linię 1-Wire,
	//	-kiedy czujnik wykryje zbocze rosnące, czeka od 15 do 60 [us] i wtedy wysyła sygnał obecności poprzez ustawienie
	//	na szynie 1-Wire stanu niskiego na czas od 60 do 240 [us],
	//	-czas zwolnienia szyny 1-Wire musi trwać minimum 480 [us],
	//	-próbkowanie sygnału zachodzi po czasie 67 [us] od momentu wejścia linii w stan wejściowy,
	//	-stan domyślny na wejściu to wyjście ze stanem wysokim,
	//	-wykryto stan niski: do linii są podłączone układy 1-Wire, ale nie wiemy ile ich jest, jakiego są typu,
	//	-wykryto stan wysoki: linia jest pusta,

	LL_USART_Disable(pUSART->pUSARTx);
	LL_USART_SetBaudRate(pUSART->pUSARTx, SYS_CORE_CLOCK, LL_USART_OVERSAMPLING_16, 9600);	//	pUSARTx->BRR
	LL_USART_Enable(pUSART->pUSARTx);

	LL_USART_ClearFlag_ORE(pUSART->pUSARTx);
	LL_USART_ClearFlag_NE(pUSART->pUSARTx);
	LL_USART_ClearFlag_FE(pUSART->pUSARTx);
	LL_USART_ClearFlag_PE(pUSART->pUSARTx);

	USART_TX_char(pUSART, 0xF0);	//	0xF0 = 0b11110000

	//while(!LL_USART_IsActiveFlag_TXE(pUSART->pUSARTx));
	//LL_USART_TransmitData8(pUSART->pUSARTx, 0xF0);
	while(!(LL_USART_IsActiveFlag_TC(pUSART->pUSARTx)));
	while(!(LL_USART_IsActiveFlag_RXNE(pUSART->pUSARTx)));

	uint8_t data_read = LL_USART_ReceiveData8(pUSART->pUSARTx);

	LL_USART_Disable(pUSART->pUSARTx);
	LL_USART_SetBaudRate(pUSART->pUSARTx, SYS_CORE_CLOCK, LL_USART_OVERSAMPLING_16, 115200);	//	pUSARTx->BRR
	LL_USART_Enable(pUSART->pUSARTx);

	//VT100_text("\n\r");
	//VT100_number(data_read);

	//if(data_read != 0xF0)	//	Albo 224,
	if(data_read == 224)	//	0b1110000
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void OW_write_1(USART_t *pUSART)
{
	//	-wysłanie stanu wysokiego na magistrale 1-Wire,

	USART_TX_char(pUSART, 0xFF);
}

void OW_write_0(USART_t *pUSART)
{
	//	-wysłanie stanu niskiego na magistrale 1-Wire,

	USART_TX_char(pUSART, 0x00);
}

void OW_write(USART_t *pUSART, const uint8_t data)
{
	//	-wysył przez master stanu wysokiego lub niskiego,
	//	-wszystkie sloty zapisu muszą trwać minimalnie 60 [us] z minimalnym czasem trwania odstępu pomiędzy nimi 1 [us],
	//	-obydwa typy zapisu przedziału czasowego są inicjalizowane przez master, który ściąga linię 1-Wire do stanu niskiego,
	//	-domyślny stan na wejściu: wyjście ze stanem wysokim,
	//	-wszystkie dane i polecenia są przesyłane od LSB,

	for(uint8_t i=0; i<8; i++)
	{
		if( data & (0b00000001 << i))
		{
			OW_write_1(pUSART);
		}
		else
		{
			OW_write_0(pUSART);
		}
	}
}

uint8_t OW_read(USART_t *pUSART)
{
	//	-odczyt jednego bajta danych wysyłanych z układu zależnego,
	//	-odczyt przedziałów czasowych musi trwać minimalnie 60 [us] z minimalnym odstępem czasowym pomiędzy nimi trwającymi 1 [us],

	uint8_t data = 0;

	for(uint8_t i=0; i<8; i++)
	{
		uint8_t data_out = 0xFF;
		uint8_t data_in = 0;
		USART_TX_char(pUSART, data_out);

		while(!(LL_USART_IsActiveFlag_TC(pUSART->pUSARTx)));
		while(!(LL_USART_IsActiveFlag_RXNE(pUSART->pUSARTx)));
		data_in = LL_USART_ReceiveData8(pUSART->pUSARTx);
		data = data | ((data_in & 0x01) << i);
	}

	return data;
}

uint8_t OW_time_slot(USART_t *pUSART)
{
	uint8_t data_out = 0xFF;
	uint8_t data_in = 0;
	USART_TX_char(pUSART, data_out);

	while(!(LL_USART_IsActiveFlag_TC(pUSART->pUSARTx)));
	while(!(LL_USART_IsActiveFlag_RXNE(pUSART->pUSARTx)));
	data_in = LL_USART_ReceiveData8(pUSART->pUSARTx);
	return (data_in & 0x01);
}










