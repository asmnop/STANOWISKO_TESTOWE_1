/*
 * DS18B20.c
 *
 *  Created on: 24 wrz 2024
 *      Author: mm
 */


#include "../../LIB/Inc/DS18B20.h"
#include "../../LIB/Inc/one_wire.h"

#include "../../LIB/Inc/VT100.h"


#include "software_timers.h"


extern USART_t USART_2;


//	WYBÓR ŹRÓDŁA NADAWCZEGO:
#define OW_SIGNAL			1
#define UART_SIGNAL			2

#define SIGNAL_SOURCE		UART_SIGNAL


//	POLECENIA ROM:
#define SEARCH_ROM			0xF0
#define READ_ROM			0x33
#define MATCH_ROM			0x55
#define SKIP_ROM			0xCC
#define ALARM_SEARCH		0xEC

//	KOMENDY FUNKCYJNE:
#define CONVERT_T			0x44
#define WRITE_SCRATCHPAD	0x4E
#define READ_SCRATCHPAD		0xBE
#define COPY_SCRATCHPAD		0x48
#define RECALL_E2			0xB8
#define READ_POWER_SUPPLY	0xB4

#define DS18B20_FAMILY_CODE	0x28


#define DS18B20_ROM_CODE_LENGTH		8
#define DS18B20_SCRATCHPAD			9


#define INFO	1



const char DS18B20_rom_code_0[] = "8-BIT FAMILY CODE";
const char DS18B20_rom_code_1[] = "SERIAL NUMBER 0";
const char DS18B20_rom_code_2[] = "SERIAL NUMBER 1";
const char DS18B20_rom_code_3[] = "SERIAL NUMBER 2";
const char DS18B20_rom_code_4[] = "SERIAL NUMBER 3";
const char DS18B20_rom_code_5[] = "SERIAL NUMBER 4";
const char DS18B20_rom_code_6[] = "SERIAL NUMBER 5";
const char DS18B20_rom_code_7[] = "8-BIT CRC";

const char* const pDS18B20_rom_code[DS18B20_ROM_CODE_LENGTH] =
{
	&DS18B20_rom_code_0[0],
	DS18B20_rom_code_1,
	DS18B20_rom_code_2,
	DS18B20_rom_code_3,
	DS18B20_rom_code_4,
	DS18B20_rom_code_5,
	DS18B20_rom_code_6,
	DS18B20_rom_code_7,
};


const char DS18B20_scratchpad_0[] = "TEMPERATURE LSB";
const char DS18B20_scratchpad_1[] = "TEMPERATURE MSB";
const char DS18B20_scratchpad_2[] = "TH REGISTER";
const char DS18B20_scratchpad_3[] = "TL REGISTER";
const char DS18B20_scratchpad_4[] = "CONF REGISTER";
const char DS18B20_scratchpad_5[] = "RESERVED (FFh)";
const char DS18B20_scratchpad_6[] = "RESERVED";
const char DS18B20_scratchpad_7[] = "RESERVED (10h)";
const char DS18B20_scratchpad_8[] = "CRC";

const char* const pDS18B20_scratchpad[DS18B20_SCRATCHPAD] =
{
	&DS18B20_scratchpad_0[0],
	DS18B20_scratchpad_1,
	DS18B20_scratchpad_2,
	DS18B20_scratchpad_3,
	DS18B20_scratchpad_4,
	DS18B20_scratchpad_5,
	DS18B20_scratchpad_6,
	DS18B20_scratchpad_7,
	DS18B20_scratchpad_8,
};


typedef struct
{
	uint8_t resolution;
	uint16_t convert_T_time;
}DS18B20_RES_t;


DS18B20_RES_t DS18B20_RES[4] =
{
	{.resolution = DS18B20_RES_9, .convert_T_time = 94},
	{.resolution = DS18B20_RES_10, .convert_T_time = 188},
	{.resolution = DS18B20_RES_11, .convert_T_time = 375},
	{.resolution = DS18B20_RES_12, .convert_T_time = 750}
};





//	ZNANE CZUJNIKI:
uint8_t DS18B20_serial_1[DS18B20_ROM_CODE_LENGTH] = {DS18B20_FAMILY_CODE, 0xB6, 0x81, 0xE5, 0x02, 0x00, 0x00, 0x2A};




//	############################################################################
uint8_t DS18B20_read_rom_code(DS18B20_t *pDS18B20)
{
	//	-odczyt kodu ROM,
	//	-procedura powinna być używana jedynie w sytuacji kiedy na linii jest podłączony tylko jeden układ slave,
	//	-jeśli będzie ich więcej to nastąpi kolizja przesyłanych danych,

	if(OW_reset(pDS18B20->pUSARTx) == 1)
	{
		OW_write(pDS18B20->pUSARTx, READ_ROM);

		for(uint8_t i=0; i<8; i++)
		{
			pDS18B20->ROM[i] = OW_read(pDS18B20->pUSARTx);
		}

		#if INFO == 1
			VT100_text("\n\r");
			VT100_text("ZAWARTOSC KODU ROM:\n\r");
			VT100_data_arr(pDS18B20->ROM, 8, pDS18B20_rom_code);
		#endif

		return 1;
	}
	else
	{
		#if INFO == 1
			VT100_text("\n\r");
			VT100_text("ODCZYT KODU ROM NIEMOZLIWY\n\r");
		#endif

		return 0;
	}
}

uint8_t DS18B20_read_scratchpad(DS18B20_t *pDS18B20)
{
	//	-odczyt zawartości scratchpad-a,
	//	-pobrane dane zapisywane są w strukturze w tablicy,

	if(OW_reset(pDS18B20->pUSARTx) == 1)
	{
		OW_reset(pDS18B20->pUSARTx);
		OW_write(pDS18B20->pUSARTx, SKIP_ROM);
		OW_write(pDS18B20->pUSARTx, READ_SCRATCHPAD);

		for(uint8_t i=0; i<9; i++)
		{
			pDS18B20->scratchpad[i] = OW_read(pDS18B20->pUSARTx);
		}

		#if INFO == 1
			VT100_text("\n\r");
			VT100_text("SCRATCHPAD:\n\r");
			VT100_data_arr(pDS18B20->scratchpad, 9, pDS18B20_scratchpad);
		#endif

		return 1;
	}
	else
	{
		#if INFO == 1
			VT100_text("\n\r");
			VT100_text("ODCZYT SCRATCHPAD-a NIEMOZLIWY\n\r");
		#endif

		return 0;
	}
}

uint8_t DS18B20_read_temperature(DS18B20_t *pDS18B20)
{
	//	-odczyt zawartości dwóch pierwszych rejestrów ze scratchpada,
	//	-inaczej chodzi o odczyt rejestrów z zawartością wskazania temperatury,
	//	-chodzi tylko o odczyt, do wykonania pomiaru należy użyć innej funkcji,

	if(OW_reset(pDS18B20->pUSARTx) == 1)
	{
		OW_reset(pDS18B20->pUSARTx);
		OW_write(pDS18B20->pUSARTx, SKIP_ROM);
		OW_write(pDS18B20->pUSARTx, READ_SCRATCHPAD);
		pDS18B20->scratchpad[0] = OW_read(pDS18B20->pUSARTx);
		pDS18B20->scratchpad[1] = OW_read(pDS18B20->pUSARTx);

		#if INFO == 1
			VT100_text("\n\r");
			VT100_text("TEMPERATURA PO ODCZYCIE:\n\r");
			VT100_data_arr(pDS18B20->scratchpad, 2, pDS18B20_scratchpad);
		#endif

		return 1;
	}
	else
	{
		#if INFO == 1
			VT100_text("\n\r");
			VT100_text("ODCZYT TEMPERATURY NIEMOZLIWY\n\r");
		#endif

		return 0;
	}
}

uint8_t DS18B20_convert_T(DS18B20_t *pDS18B20)
{

	if(OW_reset(pDS18B20->pUSARTx) == 1)
	{
		OW_write(pDS18B20->pUSARTx, SKIP_ROM);
		OW_write(pDS18B20->pUSARTx, CONVERT_T);

		uint64_t counter_begin = SYS_TICK.counter;

		//while(OW_time_slot(pDS18B20->ppUSARTx) == 0);

		uint64_t counter_end = SYS_TICK.counter;

		uint64_t counter_time = counter_end - counter_begin;

		/*
		if(time_ms > 1000)
		{
			time_ms = 1000 - ms_begin + ms_end;
		}
		*/

		#if INFO == 1
			VT100_text("\n\r");
			VT100_text("CZAS KONWERSJI: ");
			VT100_number(counter_time);
			VT100_text("\n\r");
		#endif

		/*
		if(pDS18B20->scratchpad[4] == DS18B20_RES_12)
		{
			delay_ms(800);
		}
		if(pDS18B20->scratchpad[4] == DS18B20_RES_11)
		{
			delay_ms(400);
		}
		if(pDS18B20->scratchpad[4] == DS18B20_RES_10)
		{
			delay_ms(200);
		}
		if(pDS18B20->scratchpad[4] == DS18B20_RES_9)
		{
			delay_ms(100);
		}
		*/

		return 1;
	}
	else
	{
		#if INFO == 1
			VT100_text("\n\r");
			VT100_text("BRAK UKLADU");
		#endif

		return 0;
	}
}

uint8_t DS18B20_compute_temperature(DS18B20_t *pDS18B20)
{
	//	-obliczenie wartości temperatury odczytanej z czujnika,

	//DS18B20_convert_T(pDS18B20);
	DS18B20_read_temperature(pDS18B20);

	//	WYLICZENIE WARTOŚCI PO PRZECINKU:
	uint16_t temp_decimal = 0;

	for(uint8_t i=0; i<4; ++i)
	{
		if(DS18B20_get_resolution(pDS18B20) == DS18B20_RES[i].resolution)
		{
			temp_decimal = (uint16_t)pow(5, i+1) * (pDS18B20->scratchpad[0] & 0b00001111) >> (3-i);
		}
	}


	/*
	if(pDS18B20->scratchpad[4] == DS18B20_RES_12)
	{
		temp_decimal = 625 * (pDS18B20->scratchpad[0] & 0b00001111);
	}

	if(pDS18B20->scratchpad[4] == DS18B20_RES_11)
	{
		temp_decimal = 125 * (pDS18B20->scratchpad[0] & 0b00001110)>>1;
		//after_dot = after_dot / 10;
	}

	if(pDS18B20->scratchpad[4] == DS18B20_RES_10)
	{
		temp_decimal = 25 * (pDS18B20->scratchpad[0] & 0b00001100)>>2;
		//after_dot = after_dot / 100;
	}

	if(pDS18B20->scratchpad[4] == DS18B20_RES_9)
	{
		temp_decimal = 5 * ((pDS18B20->scratchpad[0] & 0b00001000)>>3);
		//after_dot = after_dot / 1000;
	}
*/
	pDS18B20->temp_decimal = temp_decimal;


	//	WYLICZENIE WARTOŚCI CALKOWITEJ:
	uint8_t LSB = 0;
	LSB = pDS18B20->scratchpad[0]>>4;

	uint8_t MSB = 0;
	MSB = pDS18B20->scratchpad[1]<<4;

	pDS18B20->temp_total = LSB + MSB;

	return 1;
}

uint8_t DS18B20_get_temperature_total(DS18B20_t *pDS18B20)
{
	//	-wyświetlenie wartości pobranej i obliczonej temperatury,

	#if INFO == 1
		VT100_text("\n\r");
		VT100_text("ODCZYTANA TEMPERATURA: ");
		VT100_number(pDS18B20->temp_total);
		VT100_text("\n\r");
	#endif

	return pDS18B20->temp_total;
}

uint16_t DS18B20_get_temperature_decimal(DS18B20_t *pDS18B20)
{

	#if INFO == 1
		VT100_text("\n\r");
		VT100_text("ODCZYTANA WARTOSC PO PRZECINKU: ");
		//VT100_number(pDS18B20->after_dot);
		VT100_text("\n\r");
	#endif

	return pDS18B20->temp_decimal;
}

void DS18B20_set_resolution(DS18B20_t *pDS18B20, const uint8_t resolution)
{
	//	-pocedura, która ustawia rozdzielczość pomiaru temperatury,
	//	-dane dotyczące wartości do zapisania do trzech rejestrów scratchpada są pobierane ze struktury,
	//	-po zapisie danych uaktualniana jest zawartość struktury,

	uint8_t data_to_send[2];	//	Tablica na dane do wysłania,

	data_to_send[0] = pDS18B20->scratchpad[2];	//	Pierwsza niezmienna dana do wysłania,
	data_to_send[1] = pDS18B20->scratchpad[3];

	OW_reset(pDS18B20->pUSARTx);
	OW_write(pDS18B20->pUSARTx, SKIP_ROM);
	OW_write(pDS18B20->pUSARTx, WRITE_SCRATCHPAD);
	OW_write(pDS18B20->pUSARTx, data_to_send[0]);
	OW_write(pDS18B20->pUSARTx, data_to_send[1]);
	OW_write(pDS18B20->pUSARTx, resolution);

	pDS18B20->scratchpad[4] = resolution;

	#if INFO == 1
		VT100_text("\n\r");
		VT100_number(resolution);
		VT100_text("\n\r");
		VT100_number(pDS18B20->scratchpad[4]);
		//VT100_data_arr(pDS18B20->scratchpad, 2, pDS18B20_scratchpad);
	#endif
}

uint8_t DS18B20_get_resolution(DS18B20_t *pDS18B20)
{
	//	-funkcja zwraca wartość z rejestru konfiguracyjnego czyli rozdzielczość termometru,

	return pDS18B20->scratchpad[4];
}


//	############################################################################
uint8_t DS18B20_get_alarm_H(DS18B20_t *pDS18B20)
{
	return pDS18B20->scratchpad[2];
}

uint8_t DS18B20_get_alarm_L(DS18B20_t *pDS18B20)
{
	return pDS18B20->scratchpad[3];
}

void DS18B20_set_alarm(DS18B20_t *pDS18B20, const uint8_t H, const uint8_t L)
{
	//	-pocedura, która ustawia rozdzielczość pomiaru temperatury,
	//	-dane dotyczące wartości do zapisania do trzech rejestrów scratchpada są pobierane ze struktury,
	//	-po zapisie danych uaktualniana jest zawartość struktury,

	OW_reset(pDS18B20->pUSARTx);
	OW_write(pDS18B20->pUSARTx, SKIP_ROM);
	OW_write(pDS18B20->pUSARTx, WRITE_SCRATCHPAD);
	OW_write(pDS18B20->pUSARTx, H);
	OW_write(pDS18B20->pUSARTx, L);
	OW_write(pDS18B20->pUSARTx, DS18B20_get_resolution(pDS18B20));

	pDS18B20->scratchpad[2] = H;
	pDS18B20->scratchpad[3] = L;

	#if INFO == 1
		VT100_text("\n\r");
		VT100_number(DS18B20_get_resolution(pDS18B20));
		VT100_text("\n\r");
		VT100_number(pDS18B20->scratchpad[4]);
		//VT100_data_arr(pDS18B20->scratchpad, 2, pDS18B20_scratchpad);
	#endif
}


//	############################################################################
int8_t DS18B20_total_number_temperature(DS18B20_t *pDS18B20)
{
	if(DS18B20_get_resolution(pDS18B20) == DS18B20_RES_12)
	{
		if(pDS18B20->temp_decimal >= 5000)
		{
			return pDS18B20->temp_total + 1;
		}
		else
		{
			return pDS18B20->temp_total;
		}
	}

	if(DS18B20_get_resolution(pDS18B20) == DS18B20_RES_11)
	{
		if(pDS18B20->temp_decimal >= 500)
		{
			return pDS18B20->temp_total + 1;
		}
		else
		{
			return pDS18B20->temp_total;
		}
	}

	if(DS18B20_get_resolution(pDS18B20) == DS18B20_RES_10)
	{
		if(pDS18B20->temp_decimal >= 50)
		{
			return pDS18B20->temp_total + 1;
		}
		else
		{
			return pDS18B20->temp_total;
		}
	}

	if(DS18B20_get_resolution(pDS18B20) == DS18B20_RES_9)
	{
		if(pDS18B20->temp_decimal >= 5)
		{
			return pDS18B20->temp_total + 1;
		}
		else
		{
			return pDS18B20->temp_total;
		}
	}

	return 0;
}











