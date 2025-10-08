/*
 * VT100.c
 *
 *  Created on: 15 wrz 2024
 *      Author: asmnop
 */

#include "..\..\LIB\Inc\VT100.h"

#include "register.h"
#include "../../LIB/Inc/universal_functions.h"


//	INFORMACJE:
//	-na początku każdej komendy muszą znajdować się dwa znaki: Escape czyli znak numer 27 (0x1b) oraz znak '[' --> (0x5B),
//	-zapis znaku ESC wygląda następująco: "\x1b",


//	KOMENDY TERMINALA:
const char VT100_CLS[] = 			"\x1b""[2J";	//	Wyczyść ekran,
const char VT100_CHIDE[] = 			"\x1b""[?25l";	//	Ukryj kursor,
const char VT100_CSHOW[] = 			"\x1b""[?25h";	//	Pokaż kursor,
const char VT100_CHOME[] = 			"\x1b""[H";		//	Kursor na home,
const char VT100_ATTR_OFF[] =		"\x1b""[m";		//	Atrybuty domyślne,
const char VT100_ERASE_TO_END[] =	"\x1b""[K";		//	Wymazanie zawartości linii od obecnej pozycji do końca,
const char VT100_CPOS[] = 			"\x1b""[6n";	//	Zapytanie o pozycję kursora,
const char VT100_NEXT_LINE[] = 		"\x1b""[E";		//	Przejście kursora do kolejnej linii, na jej początek :),

const char VT100_C_DOWN[] = 	"\x1b""[4B";
const char VT100_C_BACK[] = 	"\x1b""[4D";

const char VT100_C_DOWN_[] = 	"\x1b""[B";

const char VT100_LOC[] = 		"\x1b""[6;2H";

const char VT100_SET_TAB[] = 	"\x1b""H";		//	Wstaw tabulator - <ESC>H,


//	##############################################################################################################################




//	############################################################################
void VT100_cursor_hide(const uint8_t cur_on_off)
{
	//	-funkcja służąca do ukrywania kursora na terminalu,
	//	-'1' - kursor ukryty, '0' - kursor widoczny,

	if(cur_on_off)
		USART_TX_string(&USART_2, VT100_CHIDE);
	else
		USART_TX_string(&USART_2, VT100_CSHOW);
}

void VT100_cls(const uint8_t cur_on_off)
{
	//	-funkcja służąca do czyszczenia ekranu oraz powrotu na jej początek,
	//	-można również sterować włączeniem/wyłączeniem kursora,
	//	-'1' - kursor ukryty, '0' - kursor widoczny,

	USART_TX_string(&USART_2, VT100_ATTR_OFF);	//	Wyłączenie wszystkich atrybutów,
	VT100_cursor_hide(cur_on_off);				//	Włącz/wyłącz kursor,
	USART_TX_string(&USART_2, VT100_CLS);			//	Wyczyść wyświetlacz,
	USART_TX_string(&USART_2, VT100_CHOME);		//	Kursor na początek ekranu,
}

void VT100_fill_line(const uint8_t ascii, const uint8_t length)
{
	//	-funkcja służąca do wstawiania do linii ilości 'length' znaków 'ascii',

	for(uint8_t i=0; i<length; i++)
		USART_TX_char(&USART_2, ascii);
}

void VT100_next_line(const uint8_t lines)
{
	//	-przesuwanie kursora do kolejnej linii,
	//	-jako argument funkcji podajemy numer linii o ile należy przejść w dół,

	USART_TX_char(&USART_2, '\x1b');
	USART_TX_char(&USART_2, '[');
	USART_TX_number(&USART_2, lines);
	USART_TX_char(&USART_2, 'E');
}

void VT100_move_cursor_left(const uint8_t lines)
{
	//	-przesuwanie kursora w lewo w jednej linii,
	//	-jako argument funkcji podajemy liczbę kolumn, o które się przesuwamy,

	USART_TX_char(&USART_2, '\x1b');
	USART_TX_char(&USART_2, '[');
	USART_TX_number(&USART_2, lines);
	USART_TX_char(&USART_2, 'D');
}

void VT100_move_cursor_right(const uint8_t lines)
{
	//	-przesuwanie kursora w lewo w jednej linii,
	//	-jako argument funkcji podajemy liczbę kolumn, o które się przesuwamy,

	USART_TX_char(&USART_2, '\x1b');
	USART_TX_char(&USART_2, '[');
	USART_TX_number(&USART_2, lines);
	USART_TX_char(&USART_2, 'C');
}

void VT100_EOL(void)
{
	//	EOL - end of line,
	//	-przejście na początek kolejnej linii,
	//	-jest to odpalenie dwóch poleceń: CR - carriage return oraz LF - line feed,

	USART_TX_char(&USART_2, '\n');
	USART_TX_char(&USART_2, '\r');
}

void VT100_location(const uint8_t row, const uint8_t column)
{
	//	-funkcja wstawia kursor do wiersza numer 'row' i kolumny numer 'column',
	//	-współrzędne home to 1,1,
	//	<ESC>[{PC_ROW};{PC_COLUMN}H

	USART_TX_char(&USART_2, '\x1b');
	USART_TX_char(&USART_2, '[');
	USART_TX_number(&USART_2, row);
	USART_TX_char(&USART_2, ';');
	USART_TX_number(&USART_2, column);
	USART_TX_char(&USART_2, 'H');
}

void VT100_text(const char *string)
{
	//	-funkcja do wyświetlania tekstu,
	//	-jest to odpalenie takiej samej funkcji z USART, ale ze zmienioną nazwą
	//	żeby było wiadomo, że to idzie na terminal,

	USART_TX_string(&USART_2, string);
}

void VT100_number(const uint16_t number)
{
	//	-funkcja do wyświetlania liczby 16-bitowej,
	//	-jest to odpalenie takiej samej funkcji z USART, ale ze zmienioną nazwą
	//	żeby było wiadomo, że to idzie na terminal,

	USART_TX_number(&USART_2, number);
}

void VT100_unumber(const uint16_t number)
{
	USART_TX_unumber(&USART_2, number);
}

void VT100_float(const float number, const uint8_t decimal)
{
	char tmp_arr[10];
	VT100_text(ftoa(number, tmp_arr, decimal));
}

void VT100_pen_color(const uint8_t color)
{
	//	-funkcja zmienia kolor tekstu wypisywanego w terminalu,
	//	-domyślnie kolor tekstu to biały,
	//	Set Attribute Mode	<ESC>[{attr1};...;{attrn}m

	USART_TX_char(&USART_2, '\x1b');
	USART_TX_char(&USART_2, '[');
	USART_TX_char(&USART_2, '3');
	USART_TX_char(&USART_2, 48+color);
	USART_TX_char(&USART_2, 'm');
}

void VT100_back_color(const uint8_t color)
{
	//	-funkcja zmienia kolor tła terminalu,
	//	-domyślnie kolor tła to czarny,
	//	Set Attribute Mode	<ESC>[{attr1};...;{attrn}m

	USART_TX_char(&USART_2, '\x1b');
	USART_TX_char(&USART_2, '[');
	USART_TX_char(&USART_2, '4');
	USART_TX_char(&USART_2, 48 + color);
	USART_TX_char(&USART_2, 'm');
}

void VT100_set_tab(void)
{


	USART_TX_string(&USART_2, VT100_SET_TAB);

}

void VT100_main_title(const char *string)
{
	//	-uniwersalna funkcja do wyświetlania komunikatu powitalnego,
	//	-na wyjściu z funkcji ustawiane są domyślne kolory tła (czarny) i tekstu (biały),

	const uint8_t LENGTH = 80;
	uint8_t counter = 0;

	while(*string)	//	Obliczenie długości napisu,
	{
		counter++;
		string++;
	}

	string = string - counter;

	VT100_location(1, 1);
	VT100_back_color(RED);
	VT100_pen_color(BLACK);
	VT100_fill_line('*', LENGTH);

	VT100_location(2, (LENGTH>>1) - (counter>>1) );
	VT100_back_color(YELLOW);
	VT100_pen_color(BLACK);
	VT100_text(string);

	VT100_location(3, 1);
	VT100_back_color(RED);
	VT100_pen_color(BLACK);
	VT100_fill_line('*', LENGTH);

	VT100_back_color(BLACK);
	VT100_pen_color(WHITE);
}

void VT100_title(const char *string)
{
	//	-funkcja do wyświetlania wyróżnionej linii tekstu,
	//	-na wyjściu z funkcji ustawiane są domyślne kolory tła (czarny) i tekstu (biały),

	const uint8_t LENGTH = 80;
	uint8_t counter = 0;

	while(*string)	//	Obliczenie długości napisu,
	{
		counter++;
		string++;
	}

	string = string - counter;

	VT100_EOL();

	VT100_fill_line('*', LENGTH);
	VT100_EOL();
	VT100_fill_line('*', LENGTH);

	VT100_move_cursor_left(80);
	VT100_move_cursor_right(35);
	VT100_text(string);
	VT100_EOL();

	VT100_fill_line('*', LENGTH);
	VT100_EOL();
}

//	////////////////////////////////////////////////////////////////////////////
void VT100_show_data(const char *string, const int16_t number)
{
	/*
	VT100_text(string);
	VT100_number((int16_t)number);
	VT100_EOL();
	*/

	uint32_t reg_tmp = number;		//	Pobranie wartości z rejestru,

	for(uint8_t i=7; i!=255; i--)
	{
		USART_TX_number(&USART_2, (reg_tmp & (1<<i)) >> i);	//	Wysył pojedynczej liczby informującej o stanie bitu,
		USART_TX_char(&USART_2, ' ');
	}

	USART_TX_string(&USART_2, string);

	VT100_EOL();

}

void VT100_show_data_16(const uint16_t reg, const char *name)
{
	//	-wyświetlenie na ekranie zawartości rejestru z uC oraz jego nazwy,
	//	-wyświetlenie w postaci: 0 1 0 1 0 0 1 1,
	//	- MSB, ... , LSB,

	uint32_t reg_tmp = reg;		//	Pobranie wartości z rejestru,

	for(uint8_t i=15; i!=255; i--)
	{
		USART_TX_number(&USART_2, (reg_tmp & (1<<i)) >> i);	//	Wysył pojedynczej liczby informującej o stanie bitu,
		USART_TX_char(&USART_2, ' ');

		if(i == 8)
		{
			USART_TX_char(&USART_2, ' ');
			USART_TX_char(&USART_2, ' ');
			USART_TX_char(&USART_2, ' ');
		}
	}

	USART_TX_string(&USART_2, name);

	VT100_EOL();
}


void VT100_show_register(const uint32_t reg, const char *name)
{
	//	-wyświetlenie na ekranie zawartości rejestru z uC oraz jego nazwy,
	//	-wyświetlenie w postaci: 0 1 0 1 0 0 1 1,
	//	- MSB, ... , LSB,

	uint32_t reg_tmp = reg;		//	Pobranie wartości z rejestru,

	for(uint8_t i=31; i!=255; i--)
	{
		USART_TX_number(&USART_2, (reg_tmp & (1<<i)) >> i);	//	Wysył pojedynczej liczby informującej o stanie bitu,
		USART_TX_char(&USART_2, ' ');

		if( (i == 24) || (i == 16) || (i == 8))
		{
			USART_TX_char(&USART_2, ' ');
			USART_TX_char(&USART_2, ' ');
			USART_TX_char(&USART_2, ' ');
		}
	}

	USART_TX_string(&USART_2, name);

	VT100_EOL();
}


uint16_t VT100_create_number(void)
{
	//	-pobranie wpisanej w konsoli liczby,
	//	-jeśli nie wpiszemy żadnej liczby to nie nastąpi przejście dalej,
	//	-jeśli wpiszemy znaki '001' to będzie to zinterpretowane jako '1',

	#define NUMBER_OF_DATA	3	//	Liczba znaków, z których składa się liczba,

	const uint8_t empty_sign = 255;				//	Znak opisujący, że nie zaszło wpisanie cyfry,
	uint8_t number_arr[NUMBER_OF_DATA] = {0};	//	Tablica na pobrane dane,
	uint16_t number = 0;		//	Wartość zwracana,
	uint8_t index = 0;			//	Numer pola w tablicy, do którego nastąpi wpisanie danej,
	uint8_t data_read = 0;		//	Odczytana dana,
	uint16_t multiplier = 1;	//	Mnożnik służący do wyliczenia jedności, dziesiątek, setek, ...

	//	Przygotowanie danych początkowych w tablicy:
	for(uint8_t i=0; i<NUMBER_OF_DATA; i++)
	{
		number_arr[i] = empty_sign;
	}

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
				//	Cofnięcie kursora, wskaźnika o jedno pole w tył,
				USART_TX_char(&USART_2, '\x1b');
				USART_TX_char(&USART_2, '[');
				USART_TX_number(&USART_2, 1);
				USART_TX_char(&USART_2, 'D');

				USART_TX_char(&USART_2, ' ');

				//	Cofnięcie kursora, wskaźnika o jedno pole w tył,
				USART_TX_char(&USART_2, '\x1b');
				USART_TX_char(&USART_2, '[');
				USART_TX_number(&USART_2, 1);
				USART_TX_char(&USART_2, 'D');

				if(index > 0)
				{
					index--;
					number_arr[index] = empty_sign;
				}
			}

			//	Dopisywanie wprowadzonej liczby:
			if(data_read >= '0' && data_read <= '9')
			{
				if(index < NUMBER_OF_DATA)
				{
					USART_TX_char(&USART_2, data_read);
					number_arr[index] = data_read - 48;
					index++;
				}
			}
		}
	}while( (data_read != ENTER) || (index == 0) );

	//	OBLICZENIE WYGENEROWANEJ LICZBY:
	for(uint8_t i=NUMBER_OF_DATA-1; i != 255; i--)
	{
		if(number_arr[i] != empty_sign)
		{
			number = number + number_arr[i] * multiplier;
			multiplier = multiplier * 10;
		}
	}

	return number;
}


void VT100_send_USART(void)
{
	//	-wysłanie zawartości wszystkich rejestrów z USART,

	VT100_location(5, 0);

	VT100_text("UCSR0A: ");
	//VT100_register(UCSR0A);
	VT100_next_line(2);

	VT100_text("UCSR0B: ");
	//VT100_register(UCSR0B);
	VT100_next_line(2);

	VT100_text("UCSR0C: ");
	//VT100_register(UCSR0C);
	VT100_next_line(2);

	VT100_text("UBRR0H: ");
	//VT100_register(UBRR0H);
	VT100_next_line(2);

	VT100_text("UBRR0L: ");
	//VT100_register(UBRR0L);
	VT100_next_line(2);
}



void VT100_init_connection(void)
{
	//	-funkcja w której oczekuje się na odbiór odpowiednich znaków składających się na hasło do ustanowienia
	//	połączenia programowego pomiędzy PC a modułem BT,

	#define LENGTH 6
	const uint8_t password[LENGTH] = "32167""\x0D";
	//const uint8_t connect[25] = "CONNECT  '9439-E5-4F6C0A'";
	//const uint8_t disconnect[28] = "DISCONNECT  '9439-E5-4F6C0A'";

	VT100_cls(1);
	VT100_text("PODAJ HASLO:");

	//uint8_t incoming_password[LENGTH];
	uint8_t match = 0;

	do
	{
		match = 0;

		//	Ciągłe sprawdzanie czy odebrany łańcuch znaków zgadza się z hasłem:
		for(uint8_t i=0; i<LENGTH; i++)
		{
			if(password[i] == USART_2.RX_data[USART_2.buf-LENGTH + i])
			{
				match++;
			}
		}
/*
		if(USART_get_flag_new_data(&USART_2) == 1)
		{
			for(uint8_t i=0; i<(LENGTH-1); i++)
			{
				incoming_password[i] = incoming_password[i+1];
			}
			incoming_password[LENGTH-1] = USART_get_data(&USART_2);
			USART_reset_flag_new_data(&USART_2);
		}
		*/
	}while(match != LENGTH);

	VT100_next_line(1);
	VT100_text("ENTER THE MATRIX");

#undef LENGTH

}

uint8_t VT100_cmd(const char command[], const uint8_t length)
{
	//	-funkcja, w której dokonuje się sprawdzenia czy łańcuch znaków w tablicy odebranych
	//	danych jest zgodny z łańcuchem znaków 'command' o długości 'length',

	uint8_t match = 0;

	//	Sprawdzenie czy odebrany łańcuch znaków zgadza się z komendą:
	for(uint8_t i=0; i<length; i++)
	{
		if(command[i] == USART_2.RX_data[USART_2.buf-length-1 + i])
		{
			match++;

			if(match == length)	//	Jeżeli ilość znaków odpowiada ilości poprawnych w tablicy
			{
				if(USART_2.RX_data[USART_2.buf-1] == '\x0D')	//	Sprawdzamy czy został wciśnięty ENTER,
				{
					match = 0;
					return 0;
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

/*

uint8_t VT100_command(const char command[], const uint8_t length)
{
	//	-funkcja w której sprawdza się czy nie został wysłany komunikat o zakończeniu połączenia PC z modułem BT,
	//	-takie zakończenie połączenia może zajść gdy: wyłączymy okno terminala PuTTY,

	#define QUANTITY	40
	//const uint8_t password[6] = "32167""\x0D";
	//const uint8_t password[6] = "\x0D""76123";
	//const uint8_t connect[25] = "CONNECT  '9439-E5-4F6C0A'";
	//const uint8_t disconnect[QUANTITY] = "DISCONNECT  '9439-E5-4F6C0A'";

	static uint8_t match = 0;


	match = 0;

	for(uint8_t i=1; i<(length+1); i++)
	{
		if(command[i-1] == USART_data_arr[QUANTITY-length-2+i])
		{
			match++;

			if(match == length)
			{
				match = 0;
				return 1;
			}
		}
		else
		{
			return 0;
		}
	}
	return 0;
}

*/
/*

uint8_t VT100_init_connection_2(void)
{
	//	-funkcja w której sprawdza się czy nie został wysłany komunikat o zakończeniu połączenia PC z modułem BT,
	//	-takie zakończenie połączenia może zajść gdy: wyłączymy okno terminala PuTTY,

	#define QUANTITY	28
	//const uint8_t password[QUANTITY] = "32167";
	//const uint8_t connect[25] = "CONNECT  '9439-E5-4F6C0A'";
	const uint8_t disconnect[QUANTITY] = "DISCONNECT  '9439-E5-4F6C0A'";

	static uint8_t incoming_password[QUANTITY];
	static uint8_t match = 0;


	if(flag_USART_new == 1)
	{
		match = 0;

		for(uint8_t i=0; i<(QUANTITY-1); i++)
		{
			incoming_password[i] = incoming_password[i+1];
		}
		incoming_password[QUANTITY-1] = USART_data;

		for(uint8_t i=0; i<QUANTITY; i++)
		{
			if(disconnect[i] == incoming_password[i])
			{
				match++;
			}
		}

		if(match == QUANTITY)
		{
			match = 0;
			flag_USART_new = 0;
			//LED_1_OFF;
			//LED_2_OFF;
			//LED_3_OFF;
			return 1;
		}
	}

	flag_USART_new = 0;

	return 0;
	#undef QUANTITY
}

uint8_t VT100_init_connection_3(void)
{
	//	-funkcja w której sprawdza się czy nie został wysłany komunikat o zakończeniu połączenia PC z modułem BT,
	//	-takie zakończenie połączenia może zajść gdy: wyłączymy okno terminala PuTTY,

	#define QUANTITY	30
	const uint8_t password[6] = "32167""\x0D";
	//const uint8_t password[6] = "\x0D""76123";
	//const uint8_t connect[25] = "CONNECT  '9439-E5-4F6C0A'";
	//const uint8_t disconnect[QUANTITY] = "DISCONNECT  '9439-E5-4F6C0A'";

	static uint8_t incoming_password[QUANTITY];
	static uint8_t match = 0;


	match = 0;

	for(uint8_t i=0; i<6; i++)
	{
		if(password[i] == USART_data_arr[QUANTITY-6+i])
		{
			match++;

			if(match == 6)
			{
				match = 0;
				flag_USART_new = 0;
				return 1;
			}
		}
		else
		{
			flag_USART_new = 0;
			return 0;
		}
	}

	flag_USART_new = 0;

	return 0;
	#undef QUANTITY
}
*/



void VT100_erase_line(void)
{
	//	-skasowanie zawartości linii od pola położenia kursora do końca,

	USART_TX_string(&USART_2, VT100_ERASE_TO_END);
}


void VT100_show_periph(uint32_t reg, const char* const names[], const char *string)
{
	//	-wyświetlenie w konsoli pełnej grupy rejestrów dotyczących danego peryferium,

	LL_USART_EnableIT_TXE(USART2);

	uint8_t num = 0;

	for(uint8_t i=0; i<200; i++)
	{
		if( names[i] != END)
		{
			num++;
		}
		else
		{
			break;
		}
	}

	VT100_title(string);

	/*
	VT100_fill_line('*', 80);
	VT100_EOL();
	VT100_text(string);
	VT100_EOL();
	VT100_fill_line('*', 80);
	VT100_EOL();
*/

	for(uint8_t i=0; i<num; i++)
	{
		uint32_t *REGISTER = (uint32_t*)(reg + (i<<2));
		VT100_show_register(*REGISTER, names[i]);
	}

	VT100_EOL();
}


void VT100_data_arr(uint8_t *data, uint8_t number, const char* const names[])
{
	//	-wyświetlenie w terminalu zawartości tablicy 'data'
	//	-wyświetlanie odbywa się w trybie POOLING,

	//LL_USART_EnableIT_TXE(USART2);

	for(uint8_t i=0; i<number; ++i)
	{
		USART_TX_string(&USART_2, names[i]);
		USART_TX_char(&USART_2, ' ');
		USART_TX_number(&USART_2, data[i]);
		USART_TX_char(&USART_2, ' ');
		VT100_text("\n\r");
	}
}

void DES(void)
{
	//	DES - Device Electronic Signature
	//	-pobranie danych z pamięci układu bez pomocy LL,

	//	PRZYKLADOWY REJESTR W CMSIS:
	//	#define PERIPH_BASE           (0x40000000UL) /*!< Peripheral base address */
	//	#define AHB2PERIPH_BASE       (PERIPH_BASE + 0x08000000UL)
	//	#define GPIOB_BASE            (AHB2PERIPH_BASE + 0x0400UL)
	//	#define GPIOB                 ((GPIO_TypeDef *) GPIOB_BASE)
	//typedef struct
	//{
	//	__IO uint32_t OTYPER;      /*!< GPIO port output type register,        Address offset: 0x04
	//} GPIO_TypeDef;
	//GPIOB->OTYPER

	//	POBRANIE WARTOŚCI Z POD ADRESU:
	uint32_t *ADDRESS = (uint32_t*)(0x40000000UL + 0x08000000UL + 0x0400UL + 0x04UL);
	VT100_EOL();
	VT100_text("PRZYKLADOWY ADRES: ");
	VT100_number(*ADDRESS);
	VT100_EOL();



	#define FLASH_SIZE_DATA_REGISTER	0x1FFF75E0
	#define DEV_ID						0x1FFF7590
	uint16_t *flash_size = (uint16_t*)(0x1FFF75E0);
	uint8_t *flash_size_L = (uint8_t*)(0x1FFF75E0);
	uint8_t *flash_size_H = (uint8_t*)(0x1FFF75E1);


	VT100_EOL();
	VT100_text("WIELKOSC PAMIECI FLASH: ");
	VT100_number(*flash_size);
	VT100_EOL();

	VT100_text("LSB: ");
	VT100_number(*flash_size_L);
	VT100_EOL();

	VT100_text("MSB: ");
	VT100_number(*flash_size_H);
	VT100_EOL();

	VT100_show_register(*flash_size, " ");
	VT100_EOL();

	//////////////////
	uint32_t *ID_1 = (uint32_t*)(DEV_ID);
	VT100_EOL();
	VT100_text("ID 1: ");
	VT100_number(*ID_1);
	VT100_EOL();
	VT100_show_register(*ID_1, " ");
	VT100_EOL();

	uint32_t *ID_2 = (uint32_t*)(DEV_ID + 0x04);
	VT100_EOL();
	VT100_text("ID 2: ");
	VT100_number(*ID_2);
	VT100_EOL();
	VT100_show_register(*ID_2, " ");
	VT100_EOL();

	uint32_t *ID_3 = (uint32_t*)(DEV_ID + 0x08);
	VT100_EOL();
	VT100_text("ID 3: ");
	VT100_number(*ID_3);
	VT100_EOL();
	VT100_show_register(*ID_3, " ");
	VT100_EOL();



	/*
	for(uint32_t i=0x1FFF7590; i<0x1FFF8590; i = i + 4)
	{
		VT100_show_register(*((uint32_t*)(i)));
		VT100_EOL();
	}
	VT100_show_register(*((uint32_t*)(0x1FFF7590)));
	VT100_EOL();
	*/
}

void DES_(void)
{
	//	DES - Device Electronic Signature
	//	-pobranie danych z pamięci układu bez pomocy LL,

	//	PRZYKLADOWY REJESTR W CMSIS:
	//	#define PERIPH_BASE           (0x40000000UL) /*!< Peripheral base address */
	//	#define AHB2PERIPH_BASE       (PERIPH_BASE + 0x08000000UL)
	//	#define GPIOB_BASE            (AHB2PERIPH_BASE + 0x0400UL)
	//	#define GPIOB                 ((GPIO_TypeDef *) GPIOB_BASE)
	//typedef struct
	//{
	//	__IO uint32_t OTYPER;      /*!< GPIO port output type register,        Address offset: 0x04
	//} GPIO_TypeDef;
	//GPIOB->OTYPER

	//	POBRANIE WARTOŚCI Z POD ADRESU:
	uint32_t *ADDRESS = (uint32_t*)(0x40000000UL + 0x08000000UL + 0x0400UL + 0x04UL);
	VT100_EOL();
	VT100_text("PRZYKLADOWY ADRES: ");
	VT100_number(*ADDRESS);
	VT100_EOL();



	#define FLASH_SIZE_DATA_REGISTER	0x1FFF75E0
	uint16_t *flash_size = (uint16_t*)(0x1FFF75E0);
	uint8_t *flash_size_L = (uint8_t*)(0x1FFF75E0);
	uint8_t *flash_size_H = (uint8_t*)(0x1FFF75E1);


	VT100_EOL();
	VT100_text("WIELKOSC PAMIECI FLASH: ");
	VT100_number(*flash_size);
	VT100_EOL();

	VT100_text("LSB: ");
	VT100_number(*flash_size_L);
	VT100_EOL();

	VT100_text("MSB: ");
	VT100_number(*flash_size_H);
	VT100_EOL();

	VT100_show_register(*flash_size, " ");
	VT100_EOL();

	/*
	for(uint32_t i=0x1FFF7590; i<0x1FFF8590; i = i + 4)
	{
		VT100_show_register(*((uint32_t*)(i)));
		VT100_EOL();
	}
	VT100_show_register(*((uint32_t*)(0x1FFF7590)));
	VT100_EOL();
	*/
}

////////////////////////////////////////////////////////////////////////////////




