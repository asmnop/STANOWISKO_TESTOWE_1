/*
 * terminal.c
 *
 *  Created on: 17 lis 2024
 *      Author: asmnop
 */


#include "../../LIB/Inc/terminal.h"
#include "../../LIB/Inc/universal_data.h"
#include "../../LIB/Inc/universal_functions.h"
//#include "software_timers.h"
#include "../../LIB/Inc/sys_tick.h"



#define ENTER		'\x0D'


static void TERMINAL_fill_line(TERMINAL_t *pTERMINAL, const uint8_t row, const char ascii)
{
	//	-funkcja służąca do wypełnienia całej linii numer 'row' znakami 'ascii',

	for(uint8_t i=0; i<pTERMINAL->column; i++)
	{
		pTERMINAL->menu[(pTERMINAL->total_column) * row + i] = ascii;
	}
}

void TERMINAL_main_title(TERMINAL_t *pTERMINAL, char *string)
{
	//	-uniwersalna funkcja do wyświetlania komunikatu powitalnego,
	//	-na wyjściu z funkcji ustawiane są domyślne kolory tła (czarny) i tekstu (biały),

	uint8_t counter = 0;

	while(*string)	//	Obliczenie długości napisu,
	{
		counter++;
		string++;
	}

	string = string - counter;

	//	Wypełnienie linii zadanym znakiem:
	TERMINAL_fill_line(pTERMINAL, 0, '*');
	TERMINAL_fill_line(pTERMINAL, 1, '*');
	TERMINAL_fill_line(pTERMINAL, 2, '*');

	//	Wstawienie napisu symetrycznie względem środka ekranu:
	TERMINAL_line(pTERMINAL, 1, (pTERMINAL->column>>1) - (counter>>1), string);
}

static char* TERMINAL_create_arr(TERMINAL_t *pTERMINAL)
{
	//	-dynamiczna alokacja pamięci na tablicę znaków, które są wyświetlane,
	//	na obecnie aktywnym ekranie terminala,

	char* ptr = (char*)malloc(pTERMINAL->total_size * sizeof(char));

	return ptr;
}

static void TERMINAL_fill(TERMINAL_t *pTERMINAL, const char sign)
{
	//	wypełnienie tablicy zadanym znakiem 'sign',

	for(uint16_t i=0; i<pTERMINAL->total_size; i++)
	{
		*(pTERMINAL->menu + i) = sign;
	}
}


void TERMINAL_enable(TERMINAL_t *pTERMINAL)
{
	//	-włączenie terminala,
	//	-wykonywać raz na początku programu przed 'main',

	VT100_cls(1);
	TERMINAL_init(pTERMINAL);
	DMA_USART2_TX.ptr_8 = (uint8_t*)pTERMINAL->menu;
	DMA_USART2_TX.number_of_data = pTERMINAL->total_size;
	DMA_USART2_TX_init(&DMA_USART2_TX);
	DMA_start(&DMA_USART2_TX);
}

void TERMINAL_disable(TERMINAL_t *pTERMINAL)
{
	//	-wyłączenie aktywnego terminala,

	LL_USART_DisableDMAReq_TX(USART2);
	DMA_stop(&DMA_USART2_TX);
	free(pTERMINAL->menu);
	pTERMINAL->menu = NULL;
	VT100_cls(1);
}

void TERMINAL_change(TERMINAL_t *pTERMINAL_out, TERMINAL_t *pTERMINAL_in)
{
	//	-zmiana menu wyświetlanego w terminalu,

	TERMINAL_disable(pTERMINAL_out);
	TERMINAL_enable(pTERMINAL_in);
}


void TERMINAL_line(TERMINAL_t *pTERMINAL, const uint8_t row, const uint8_t column, const char *string)
{
	//	-skopiowanie łańcucha znaków do tablicy z danymi do wyświetlenia na ekranie,
	//	-kopiowane jest tyle znaków, ile wynosi długość łańcucha nie licząc znaku '\0'

	strncpy(&pTERMINAL->menu[(pTERMINAL->total_column) * row + column], string, strlen(string));
}

void TERMINAL_number(TERMINAL_t *pTERMINAL, const uint8_t row, const uint8_t column, uint32_t number, uint8_t space)
{
	char tmp[33];
	itoa_space(number, tmp, 10, space);

	TERMINAL_line(pTERMINAL, row, column, tmp);

	//strncpy(&pTERMINAL->menu[(pTERMINAL->total_column) * row + column], tmp, space);
}

void TERMINAL_float(TERMINAL_t *pTERMINAL, const uint8_t row, const uint8_t column, float number, uint8_t space)
{
	char tmp_arr[10];

	ftoa(number, tmp_arr, 3);
	TERMINAL_line(pTERMINAL, row, column, tmp_arr);
}

void TERMINAL_arr(TERMINAL_t *pTERMINAL, const uint8_t row, const uint8_t column, char *string, const uint8_t number_of_data)
{
	//	-skopiowanie łańcucha znaków do tablicy z danymi do wyświetlenia na ekranie,
	//	-kopiowane jest tyle znaków, ile wynosi długość łańcucha nie licząc znaku '\0'

	strncpy(&pTERMINAL->menu[pTERMINAL->total_column * row + column], string, number_of_data);
}

void TERMINAL_init(TERMINAL_t *pTERMINAL)
{
	//	-inicjalizacja zawartości terminala obsługiwanego przez DMA z USART2,

	const char end_command[] = "\x1b""[H""\x1b""[?25l";
	//const uint8_t start_column = 4;

	pTERMINAL->menu = TERMINAL_create_arr(pTERMINAL);

	TERMINAL_fill(pTERMINAL, ' ');

	//	Wpisanie do tablicy znaków domyślnych do każdego początku wiersza oraz komend wiersza:
	for(uint8_t i=0; i<(pTERMINAL)->row; i++)
	{
		TERMINAL_line((pTERMINAL), i, (pTERMINAL)->column-1, "|");
		if(i == ((pTERMINAL)->row -1 ))	break;
		TERMINAL_line((pTERMINAL), i, (pTERMINAL)->column, "\r\n");
	}

	//	Wypełnienie tablicy parametrami do wyświetlenia:
	for(uint8_t i = 0; i<pTERMINAL->row; i++)
	{
		//TERMINAL_line((pTERMINAL), i, pTERMINAL->line[i].text_position, pTERMINAL->line[i].text);
		//TERMINAL_float(pTERMINAL, i, pTERMINAL->line[i].number_2_position, *(pTERMINAL->line[i].number_2), 8);
		//	!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//	PONIŻSZE DO WYWALENIA!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		////////TERMINAL_line((pTERMINAL), i, start_column, *(pTERMINAL->menu_item + i));
	}

	//char tmp_arr[10];
	//TERMINAL_float(pTERMINAL, 5, pTERMINAL->line[4].number_2_position, *(float*)(pTERMINAL->line[4].number_2), 8);
	//TERMINAL_float(pTERMINAL, 6, pTERMINAL->line[5].number_2_position, *(float*)(pTERMINAL->line[5].number_2), 8);
	//TERMINAL_float(pTERMINAL, 7, pTERMINAL->line[6].number_2_position, *(float*)(pTERMINAL->line[6].number_2), 8);
	//TERMINAL_line((pTERMINAL), 8, start_column, ftoa(MOTOR_1_PID.k_p, tmp_arr, 3));

	//	Menu:
	TERMINAL_main_title(pTERMINAL, pTERMINAL->name);

	//	Zapis komendy końcowej:
	TERMINAL_line((pTERMINAL), pTERMINAL->row - 1, pTERMINAL->column, end_command);
	//strcpy(&(pTERMINAL)->menu[pTERMINAL->total_size - pTERMINAL->end_command], end_command);


/*

	TERMINAL_line((pTERMINAL), 4, start_column, "Wskazanie impulsatora  - ");
	TERMINAL_line((pTERMINAL), 5, start_column, "Wskazanie enkodera     - ");
	TERMINAL_line((pTERMINAL), 6, start_column, "Licznik sekund         - ");
	TERMINAL_line((pTERMINAL), 7, start_column, "Ostatnio odebrany znak - ");
	TERMINAL_line((pTERMINAL), 8, start_column, "Lancuch znakow         - ");
	TERMINAL_line((pTERMINAL), 9, start_column, "Ostatnie polecenie LED - ");

	TERMINAL_line((pTERMINAL), 10, start_column, "LED 1 - ");
	TERMINAL_line((pTERMINAL), 11, start_column, "LED 2 - ");
	TERMINAL_line((pTERMINAL), 12, start_column, "LED 3 - ");
	TERMINAL_line((pTERMINAL), 13, start_column, "LED 4 - ");
	TERMINAL_line((pTERMINAL), 14, start_column, "LED 5 - ");
	TERMINAL_line((pTERMINAL), 15, start_column, "LED 6 - ");
	TERMINAL_line((pTERMINAL), 16, start_column, "LED 7 - ");
	TERMINAL_line((pTERMINAL), 17, start_column, "LED 8 - ");

*/

}

uint16_t TERMINAL_create_number(TERMINAL_t *pTERMINAL, const uint8_t row, const uint8_t column)
{
	//	-pobranie wpisanej w konsoli liczby,
	//	-jeśli nie wpiszemy żadnej liczby to nie nastąpi przejście dalej,
	//	-jeśli wpiszemy znaki '001' to będzie to zinterpretowane jako '1',

	#define NUMBER_OF_DATA	4	//	Liczba znaków, z których składa się liczba,

	const char empty_sign = '_';				//	Znak opisujący, że nie zaszło wpisanie cyfry
	char number_arr[NUMBER_OF_DATA + 1] = {'0'};		//	Tablica na pobrane dane,
	uint16_t number = 0;		//	Wartość zwracana,
	uint8_t index = 0;			//	Numer pola w tablicy, do którego nastąpi wpisanie danej,
	char data_read = 0;			//	Odczytana dana,
	uint16_t multiplier = 1;	//	Mnożnik służący do wyliczenia jedności, dziesiątek, setek, ...

	//	Przygotowanie danych początkowych w tablicy:
	for(uint8_t i=1; i<NUMBER_OF_DATA; i++)
	{
		number_arr[i] = empty_sign;
	}

	number_arr[NUMBER_OF_DATA] = '\0';

	TERMINAL_arr(pTERMINAL, row, column, number_arr, NUMBER_OF_DATA);
	/*
	for(uint8_t i=0; i<(pUSART->buf-1); i++)
	{
		pUSART->RX_data[i] = pUSART->RX_data[i+1];
	}

	pUSART->RX_data[pUSART->buf-1] = data;
*/
	do
	{
		//	////////////////////////////////////////////////////////////////////
		//	OBSLUGA NOWYCH DANYCH ODEBRANYCH PO USART:
		if(USART_get_flag_new_data(&USART_2) == 1)
		{
			USART_reset_flag_new_data(&USART_2);
			data_read = USART_get_data(&USART_2);

			//	Kasowanie wprowadzonej liczby:
			if(data_read == 'q')
			{
				for(uint8_t i=0; i<NUMBER_OF_DATA; i++)
				{
					number_arr[i] = empty_sign;
				}

				index = 0;
				TERMINAL_arr(pTERMINAL, row, column, number_arr, NUMBER_OF_DATA);
			}

			//	Kasowanie cyfry do tyłu:
			if( (data_read == 'z') && (index != 0) )
			{
				index--;
				number_arr[index] = empty_sign;
				TERMINAL_arr(pTERMINAL, row, column, number_arr, NUMBER_OF_DATA);
			}

			//	Dopisywanie wprowadzonej liczby:
			if(data_read >= '0' && data_read <= '9')
			{
				if(index < NUMBER_OF_DATA)
				{
					number_arr[index] = data_read;
					index++;
					TERMINAL_arr(pTERMINAL, row, column + index - 1, &data_read, 1);
				}
			}
		}
	}while( (data_read != ENTER) );

	//	OBLICZENIE WYGENEROWANEJ LICZBY:
	for(uint8_t i = NUMBER_OF_DATA - 1; i != 255; i--)
	{
		number_arr[i] = number_arr[i] - 48;

		if((number_arr[i] >= 0) && (number_arr[i] <= 9))
		{
			number = number + number_arr[i] * multiplier;
			multiplier = multiplier * 10;
		}
	}
#undef NUMBER_OF_DATA
	return number;
}

uint16_t TERMINAL_create_number_(TERMINAL_t *pTERMINAL, const uint8_t row, const uint8_t column)
{
	//	-pobranie wpisanej w konsoli liczby,
	//	-jeśli nie wpiszemy żadnej liczby to nie nastąpi przejście dalej,
	//	-jeśli wpiszemy znaki '001' to będzie to zinterpretowane jako '1',

	#define NUMBER_OF_DATA	3	//	Liczba znaków, z których składa się liczba,

	const char empty_sign = '_';				//	Znak opisujący, że nie zaszło wpisanie cyfry
	char number_arr[NUMBER_OF_DATA + 1] = {'0'};		//	Tablica na pobrane dane,
	uint16_t number = 0;		//	Wartość zwracana,
	uint8_t index = NUMBER_OF_DATA - 1;			//	Numer pola w tablicy, do którego nastąpi wpisanie danej,
	char data_read = 0;			//	Odczytana dana,
	uint16_t multiplier = 1;	//	Mnożnik służący do wyliczenia jedności, dziesiątek, setek, ...


	number_arr[NUMBER_OF_DATA] = '\0';

	//	Przygotowanie danych początkowych w tablicy:
	for(uint8_t i=0; i<NUMBER_OF_DATA; i++)
	{
		number_arr[i] = empty_sign;
	}

	number_arr[NUMBER_OF_DATA - 1] = '0';

	TERMINAL_arr(pTERMINAL, row, column, number_arr, NUMBER_OF_DATA);

	do
	{
		//	////////////////////////////////////////////////////////////////////
		//	OBSLUGA NOWYCH DANYCH ODEBRANYCH PO USART:
		if(USART_get_flag_new_data(&USART_2) == 1)
		{
			USART_reset_flag_new_data(&USART_2);
			data_read = USART_get_data(&USART_2);

			//	Kasowanie wprowadzonej liczby:
			if(data_read == 'q')
			{
				for(uint8_t i=0; i<NUMBER_OF_DATA; i++)
				{
					number_arr[i] = empty_sign;
				}

				index = NUMBER_OF_DATA - 1;
				number_arr[NUMBER_OF_DATA - 1] = '0';
				TERMINAL_arr(pTERMINAL, row, column, number_arr, NUMBER_OF_DATA);
			}

			//	Kasowanie cyfry do tyłu:
			if( (data_read == 'z') && (index != NUMBER_OF_DATA - 1) )
			{
				index++;
				number_arr[index] = empty_sign;

				if(index == NUMBER_OF_DATA - 1)
				{
					number_arr[NUMBER_OF_DATA - 1] = '0';
				}
				TERMINAL_arr(pTERMINAL, row, column, number_arr, NUMBER_OF_DATA);
			}

			//	Dopisywanie wprowadzonej liczby:
			if(data_read >= '0' && data_read <= '9')
			{
				if(index != 255)
				{
					number_arr[index] = data_read;
					index--;
					TERMINAL_arr(pTERMINAL, row, column + index + 1, &data_read, 1);
				}
			}
		}
	}while( (data_read != ENTER) );

	//	OBLICZENIE WYGENEROWANEJ LICZBY:
	for(uint8_t i=NUMBER_OF_DATA - 1; i != 255; i--)
	{
		number_arr[i] = number_arr[i] - 48;

		if(number_arr[i] >= 0 && number_arr[i] <= 9)
		{
			number = number + number_arr[i] * multiplier;
			multiplier = multiplier * 10;
		}
	}

	return number;

#undef NUMBER_OF_DATA
}

float TERMINAL_create_float(TERMINAL_t *pTERMINAL, const uint8_t row, const uint8_t column)
{
	//	-pobranie od użytkownika znaków opisujących liczbę typu float zamiana ich na liczbę typu float,
	//	-dla ułatwienia wprowadzania danych liczba float składać się będzie z dwóch łańcuchów znaków,

	#define INTEGER			3	//	Liczba znaków na część całkowitą,
	#define DECIMAL			3	//	Liczba znaków na część dziesiętną,
	#define NUMBER_OF_DATA	(INTEGER + DECIMAL)		//	Liczba znaków, z których składa się liczba typu float,
	#define WHOLE_DATA		(NUMBER_OF_DATA + 2)	//	Całkowita liczba znaków w tablicy do wyświetlenia na terminalu,

	//const char empty_sign = '_';	//	Znak opisujący, że nie zaszło wpisanie cyfry
	char number_arr[WHOLE_DATA] = {'\0'};		//	Tablica na dane do wyświetlenia na terminalu,
	char integer_arr[INTEGER + 1] = {'0', '\0'};		//	Tablica na znaki opisujące liczbę całkowitą,
	char decimal_arr[DECIMAL + 1] = {'0', '\0'};		//	Tablica na znaki opisujące część dziesiętną,
	uint8_t flag_int_dec = 0;
	uint8_t begin = 1;

	char data_read = 0;					//	Odczytana dana,
	char nul = '\0';

	uint8_t space = strlen(integer_arr) + strlen(decimal_arr);
	strncat(number_arr, "__________", 6 - space);
	strcat(number_arr, integer_arr);
	strcat(number_arr, ".");
	strcat(number_arr, decimal_arr);

	TERMINAL_arr(pTERMINAL, row, column, number_arr, strlen(number_arr));

	do
	{
		//	////////////////////////////////////////////////////////////////////
		//	OBSLUGA NOWYCH DANYCH ODEBRANYCH PO USART:
		if(USART_get_flag_new_data(&USART_2) == 1)
		{
			USART_reset_flag_new_data(&USART_2);
			data_read = USART_get_data(&USART_2);

			if(data_read == 'a')
			{
				flag_int_dec = 1;
			}

			if(data_read == 's')
			{
				flag_int_dec = 0;
			}

			//	Kasowanie wprowadzonej liczby:
			if(data_read == 'q')
			{
				number_arr[0] = '\0';
				integer_arr[0] = '0';
				integer_arr[1] = '\0';
				decimal_arr[0] = '0';
				decimal_arr[1] = '\0';
				begin = 1;
			}

			//	Kasowanie cyfry do tyłu:
			if( (data_read == 'z') )
			{
				if(flag_int_dec == 1 && strlen(integer_arr) > 0)
				{
					if(integer_arr[0] == '0' && integer_arr[1] == '\0')
					{
						integer_arr[0] = '0';
					}
					else
					{
						strcpy(integer_arr + strlen(integer_arr) - 1, &nul);

						if(strlen(integer_arr) == 0)
						{
							integer_arr[0] = '0';
						}
					}
				}

				if(flag_int_dec == 0 && strlen(decimal_arr) > 0)
				{
					if(decimal_arr[0] == '0' && decimal_arr[1] == '\0')
					{
						decimal_arr[0] = '0';
						begin = 1;
					}
					else
					{
						strcpy(decimal_arr + strlen(decimal_arr) - 1, &nul);

						if(strlen(decimal_arr) == 0)
						{
							decimal_arr[0] = '0';

						}
					}
				}
			}

			//	Dopisywanie wprowadzonej liczby:
			if(data_read >= '0' && data_read <= '9')
			{
				//	INTEGER:
				if(flag_int_dec == 1 && strlen(integer_arr) < INTEGER)
				{
					if(integer_arr[0] == '0' && integer_arr[1] == '\0')
					{
						integer_arr[0] = '\0';
					}
					strncat(integer_arr, &data_read, 1);
				}

				//	DECIMAL:
				if(flag_int_dec == 0 && strlen(decimal_arr) < DECIMAL)
				{
					if(decimal_arr[0] == '0' && decimal_arr[1] == '\0' && begin == 1)
					{
						decimal_arr[0] = data_read;
						begin = 0;
					}
					else
					{
						strncat(decimal_arr, &data_read, 1);
					}
				}
			}
		}

		number_arr[0] = '\0';
		space = strlen(integer_arr) + strlen(decimal_arr);
		strncat(number_arr, "__________", 6 - space);
		strcat(number_arr, integer_arr);
		strcat(number_arr, ".");
		strcat(number_arr, decimal_arr);

		TERMINAL_arr(pTERMINAL, row, column, number_arr, strlen(number_arr));

	}while( (data_read != ENTER) );

	number_arr[0] = '\0';
	strcat(number_arr, integer_arr);
	strcat(number_arr, ".");
	strcat(number_arr, decimal_arr);

#undef NUMBER_OF_DATA
	return atof(number_arr);
}

uint8_t TERMINAL_column(TERMINAL_t *pTERMINAL, const uint8_t field)
{
	uint8_t column = 0;

	for(uint8_t i=0; i<field; i++)
	{
		column = column + pTERMINAL->field[i];
	}

	return column;
}


void USART_terminal_info(TERMINAL_t *pTERMINAL, const uint8_t line, const uint8_t row_1, const uint8_t row_2)
{
	//	LINIA 1:
	//	Wpisanie kodu ASCII ostatnio odebranego znaku z terminala:
	TERMINAL_line(pTERMINAL, line, row_1, "KOD KLAWISZA:");
	TERMINAL_number(pTERMINAL, line, row_2, USART_get_data(&USART_2), 3);

	//	LINIA 2:
	//	Wpisanie ostatnio odebranego znaku z terminala:
	uint8_t tmp_last_key[10] = {USART_get_data(&USART_2), '\0'};

	if((tmp_last_key[0] == '\x0D') || (tmp_last_key[0] == 127) || (tmp_last_key[0] == 9))
	{
		tmp_last_key[0] = '!';
	}
	TERMINAL_line(pTERMINAL, line + 1, row_1, "Ostatni klawisz:");
	TERMINAL_line(pTERMINAL, line + 1, row_2, (char*)&tmp_last_key);

	//	LINIA 3:
	//	Wpisanie ostatniej sekwencji znaków z terminala:
	for(uint8_t i=0; i<USART_2.buf; i++)
	{
		if((USART_2.RX_data[i] == '\x0D') || (USART_2.RX_data[i] == 127) || (USART_2.RX_data[i] == 9))	//	Jeżeli odebrano ENTER,
		{
			tmp_last_key[i] = '!';
		}
		else
		{
			tmp_last_key[i] = USART_2.RX_data[i];
		}
	}

	//	SEKWENCJA KLAWISZY:
	TERMINAL_line(pTERMINAL, line + 2, row_1, "Sekwencja klawiszy:");
	TERMINAL_arr(pTERMINAL, line + 2, row_2, (char*)tmp_last_key, 10);

	//	LINIA 5:
	//	ODSTĘP CZASU POMIĘDZY DWOMA WYKRYCIAMI ZAJŚCIA NOWEJ DANEJ:
	TERMINAL_line(pTERMINAL, line + 3, row_1, "Odstep czasu [ms]:");
	TERMINAL_number(pTERMINAL, line + 3, row_2, latch_value_diff, 8);
}


uint8_t TERMINAL_get_terminal(TERMINAL_DISPLAY_t *pTERMINAL_DISPLAY)
{
	//	-pobranie numeru terminala, który jest obecnie aktywny, wyświetlany,

	return pTERMINAL_DISPLAY->terminal_ptr;
}

uint8_t TERMINAL_set_terminal(TERMINAL_DISPLAY_t *pTERMINAL_DISPLAY, uint8_t terminal_number)
{
	//	-ustawienie numeru nowego terminala i jego włączenie,

	terminal_number = terminal_number - 49;

	if(terminal_number != pTERMINAL_DISPLAY->terminal_ptr)
	{
		pTERMINAL_DISPLAY->terminal_ptr = terminal_number;
	}

	return pTERMINAL_DISPLAY->terminal_ptr;

}









