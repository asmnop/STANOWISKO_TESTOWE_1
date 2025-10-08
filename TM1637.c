/*
 * TM1637.c
 *
 *  Created on: Apr 25, 2024
 *      Author: asmnop
 */


#include "..\..\LIB\Inc\TM1637.h"

#include "..\..\LIB\Inc\gpio.h"
#include "..\..\LIB\Inc\ASCII_table.h"
#include "..\..\LIB\Inc\TM1637_phrases.h"
#include "..\..\LIB\Inc\universal_data.h"

#include "software_timers.h"

//	INFORMATIONS:
//	-communications protocol is made by two-wire bus interface,
//	-it is not I2C protocol because we don't have slave address,
//	DATA FRAME:
//	-when CLK is in a high level and DIO changes from high to low START command is made,
//	-start means that sending transmission is begin,
//	-when CLK is in a high level and DIO changes form low to high STOP command is made,
//	-state change on DIO take place when CLK is low,
//	-read data take place when CLK is high, in that time DIO should not change,

//	ACTIONS:
//	-create functions to send start command, stop command, sending data, sending command,

//	DATA FRAME:
//	-default state: DIO - H, CLK - H,
//	-sending START - DIO - L, CLK - L,
//	-when CLK is low we have to set the DIO line to wanted state,
//	-after setting DIO we make CLK - H and CLK - L,
//	-we make it eight times for every bit,
//	-TM1637 sends acknowledge, we have to check state on DIO,
//	-we make a input mode on DIO line, read state,
//	-then we make CLK - H, then we also can read DIO state,
//	-after that we change mode of work DIO as output with low state, and CLK - L,
//	-sending STOP - CLK - H, DIO - H,
//	-we can skip reading acknowledge by make a CLK - H, CLK - L,

//	TODO:
//	-what are the times of making pin change?
//	-do we have to make program blocking delay or times between that instructions are
//	enough?
//	-make a table of ASCII,
//	-make a table of HEX numbers,
//	-make a text function, clear display,
//void TM1637_init_2(TM1637_tt *ptr);
void TM1637_init(TM1637_t *ptr);
void TM1637_demo(TM1637_t *pDisplay_1, TM1637_t *pDisplay_2);
static void TM1637_START(TM1637_t *ptr);
static void TM1637_STOP(TM1637_t *ptr);
static void TM1637_delay(void);
static void TM1637_write_data(TM1637_t *ptr, const uint8_t data);
static void TM1637_cmd(TM1637_t *ptr, const uint8_t data);
static void TM1637_write_display(TM1637_t *ptr, const char sign_arr[]);
void TM1637_cls(TM1637_t *ptr);
void TM1637_text(TM1637_t *ptr, const char sign_arr[], const uint16_t duration);
void TM1637_symbol(TM1637_t *ptr, const char sign_arr[], const uint16_t duration);
void TM1637_film(TM1637_t *ptr, const char* const film_ptr[], const uint16_t duration);
static void TM1637_number_conversion(TM1637_t *ptr, const uint16_t number, const uint8_t system, char number_arr[]);
void TM1637_number(TM1637_t *ptr, const uint16_t number, const uint8_t system, const uint16_t duration);

uint8_t TM1637_get_brigtness(TM1637_t *ptr);
void TM1637_set_brightness(TM1637_t *ptr, uint8_t up_down);
void TM1637_show_brightness(TM1637_t *ptr);


/////////////////////////////////////////////////////////////////////////////////////////
void TM1637_init(TM1637_t *ptr)
{
	GPIO_set_pin_as_output(ptr->line_DIO);
	GPIO_set_pin_as_output(ptr->line_CLK);

	TM1637_cmd(ptr, NORMAL_AUTO_WRITE);	//	Selected working mode,
	TM1637_cmd(ptr, ptr->brightness);	//	Selected display brightness,
}

void TM1637_demo(TM1637_t *pDisplay_1, TM1637_t *pDisplay_2)
{
	//TM1637_write_display(ptr, "@@@@");	//	Show some data on display,
	//TM1637_write_display("????");	//	Show some data on display,

	TM1637_text(pDisplay_1, "_88_", 500);		//	Show some text on display,
	TM1637_text(pDisplay_1, " 666", 500);		//	Show some text on display,
	TM1637_text(pDisplay_1, "HELO", 500);		//	Show some text on display,

	for(uint8_t i=0; i<10; i++)
	{
		TM1637_number(pDisplay_1, i, BIN, 50);
		TM1637_number(pDisplay_1, i, DEC, 50);
		TM1637_number(pDisplay_1, i, DEC_SYS, 50);
		TM1637_number(pDisplay_1, i, HEX, 50);
		TM1637_number(pDisplay_1, i, HEX_SYS, 50);
	}

	while(1)
	{
		TM1637_film(pDisplay_2, snake_film, 100);
		TM1637_film(pDisplay_2, wave_film, 200);
		TM1637_film(pDisplay_1, wave_horizontal_film, 500);
		for(uint8_t i=0; i<6; i++)
		{
			TM1637_film(pDisplay_1, circle_full_film, 200);
		}
		for(uint8_t i=0; i<6; i++)
		{
			TM1637_film(pDisplay_2, circle_film, 200);
		}
	}
}

static void TM1637_START(TM1637_t *ptr)
{
	//	-sending start command,
	//	-we assume that state on two lines CLK and DIO is known, and it's a high state,

	//	Data line is going from high to low, CLK is on high state,
	GPIO_reset_pin(ptr->line_DIO);
	TM1637_delay();

	//	CLK line is going from high to low, DIO is on low state,
	GPIO_reset_pin(ptr->line_CLK);
	TM1637_delay();
}

static void TM1637_STOP(TM1637_t *ptr)
{
	//	-sending stop command,
	//	-we assume that state on CLK line is low and we don't know how is the state on DIO,

	//	Setting low state on DIO pin,
	GPIO_reset_pin(ptr->line_DIO);
	TM1637_delay();

	//	CLK line is going from low to high, DIO is on low state,
	GPIO_set_pin(ptr->line_CLK);
	TM1637_delay();

	//	Data line is going from low to high, CLK is on high state,
	GPIO_set_pin(ptr->line_DIO);
	TM1637_delay();
}

static void TM1637_delay(void)
{
	//	-delay function to make a necessary delay in stable state on lines,
	static uint8_t i = 0;
	i++;
}

static void TM1637_write_data(TM1637_t *ptr, const uint8_t data)
{
	//	-sending one data frame,
	//	-after that we can send another data or stop command,
	//	-at the output we have: CLK - L, DIO - unknown,

	//	Sending eight bits of data:
	for(uint8_t i=0; i<8; i++)
	{
		if( (data >> i) & 0x01)
		{
			GPIO_set_pin(ptr->line_DIO);
		}
		else
		{
			GPIO_reset_pin(ptr->line_DIO);
		}
		TM1637_delay();

		GPIO_set_pin(ptr->line_CLK);
		TM1637_delay();

		GPIO_reset_pin(ptr->line_CLK);
		TM1637_delay();
	}

	//	Receive acknowledge bit or skip that:
	//	Now we make a skip:
	GPIO_set_pin(ptr->line_CLK);
	TM1637_delay();

	GPIO_reset_pin(ptr->line_CLK);
	TM1637_delay();
}

static void TM1637_cmd(TM1637_t *ptr, const uint8_t data)
{
	TM1637_START(ptr);
	TM1637_write_data(ptr, data);
	TM1637_STOP(ptr);
}

static void TM1637_write_display(TM1637_t *ptr, const char sign_arr[])
{
	//	-sending data to show on display,

	TM1637_START(ptr);

	TM1637_write_data(ptr, DISP_ADDR_1);
	for(uint8_t i=0; i<NUMBER_OF_DISPLAYS; i++)
	{
		TM1637_write_data(ptr, sign_arr[i]);
	}

	TM1637_STOP(ptr);
}

void TM1637_cls(TM1637_t *ptr)
{
	TM1637_text(ptr, "    ", 0);
}

void TM1637_text(TM1637_t *ptr, const char sign_arr[], const uint16_t duration)
{

	char buffer_arr[NUMBER_OF_DISPLAYS];	//	Table for converted signs,

	for(uint8_t i=0; i<NUMBER_OF_DISPLAYS; i++)
	{
		buffer_arr[i] = ASCII_table[(uint8_t)sign_arr[i]];	//	Filling table in good ASCII signs,
	}

	TM1637_write_display(ptr, buffer_arr);

	timer_text = duration;
	while(timer_text);
}

void TM1637_symbol(TM1637_t *ptr, const char sign_arr[], const uint16_t duration)
{
	TM1637_write_display(ptr, sign_arr);

	timer_text = duration;
	while(timer_text);
}

void TM1637_film(TM1637_t *ptr, const char* const film_ptr[], const uint16_t duration)
{
	//	-sequence of displaying four signs on LED display,
	//	-empty line could be,
	//	-duration --> time of displaying single movie frame,
	//	-if we in 'duration' argument write '0' then time duration is zero seconds,
	//	-it is not recommended to set 'duration' to zero because time of movie frame will be very short,

	uint8_t i = 0;

	while( (**(film_ptr+i)) != 255 )
	{
		TM1637_write_display(ptr, *(film_ptr+i));	//	Displaying one movie frame,
		i++;

		timer_text = duration;
		while(timer_text);
	}
}


static void TM1637_number_conversion(TM1637_t *ptr, const uint16_t number, const uint8_t system, char number_arr[])
{
	//	############################################################################################
	//	Number conversion to signs needed to display in bin representations,
	if( system == BIN )
	{
		number_arr[0] = BIN_table[(number >> 6) & 0b00000011];
		number_arr[1] = BIN_table[(number >> 4) & 0b00000011];
		number_arr[2] = BIN_table[(number >> 2) & 0b00000011];
		number_arr[3] = BIN_table[number & 0b00000011];
	}

	//	############################################################################################
	//	Number conversion to signs needed to display in decimal representations,
	else if( (system == DEC) || (system == DEC_SYS) )
	{
		uint16_t number_temp = number;

		uint8_t thousand = 0;
		uint8_t hundred = 0;
		uint8_t ten = 0;
		uint8_t one = 0;

		if( number_temp >= 10000 )		//	If number is too big to display it,
		{
			while(1)
			{
				//	Writing info about error:
				TM1637_number(ptr, 666, 10, 1000);
				TM1637_text(ptr, "Eror", 1000);
				TM1637_text(ptr, " biG", 1000);
				TM1637_text(ptr, " nok", 1000);
			};
		}

		while( number_temp >= 1000 )	//	As long as number is bigger or equal 1000,
		{
			thousand++;
			number_temp = number_temp - 1000;
		}

		while( number_temp >= 100 )		//	As long as number is bigger or equal 100,
		{
			hundred++;
			number_temp = number_temp - 100;
		}

		while( number_temp >= 10 )		//	As long as number is bigger or equal 10,
		{
			ten++;
			number_temp = number_temp - 10;
		}

		one = number_temp;				//	The rest of the number is equal to ones,

		//	Writing designated value of thousands, hundreds, tens and ones to output array:
		if( thousand > 0 ) number_arr[0] = HEX_table[thousand];   else number_arr[0] = ASCII_table[' '];
		if( hundred > 0 )  number_arr[1] = HEX_table[hundred];    else number_arr[1] = ASCII_table[' '];
		if( ten > 0 )      number_arr[2] = HEX_table[ten];        else number_arr[2] = ASCII_table[' '];
		number_arr[3] = HEX_table[one];

		if( ten == 0 && ( hundred > 0 || thousand > 0 ) )
		{
			number_arr[2] = HEX_table[ten];
		}

		if( hundred == 0 && thousand > 0 )
		{
			number_arr[1] = HEX_table[hundred];
		}

		if( (system == DEC_SYS) && (thousand == 0) )
		{
			number_arr[0] = number_arr[1];		//	Zapis tysięcy do odpowiedniej pozycji w tablicy,
			number_arr[1] = number_arr[2];		//	Zapis setek do odpowiedniej pozycji w tablicy,
			number_arr[2] = number_arr[3];		//	Zapis dziesiątek do odpowiedniej pozycji w tablicy,
			number_arr[3] = ASCII_table['d'];	//	Symbol 'd' - dziesiętny system liczbowy,
		}
	}

	//	############################################################################################
	//	Number conversion to signs needed to display in hex representations and with no system info,
	else if( system == HEX )
	{
		uint8_t position_0 = 0;		//	Variable to storage ones from number in hex format,
		uint8_t position_1 = 0;		//	Variable to storage tens from number in hex format,

		position_0 = number & 0b00001111;			//	Counting ones,
		position_1 = (number >> 4) & 0b00001111;	//	Counting tens,

		number_arr[0] = ASCII_table[' '];			//	Empty field,
		number_arr[1] = ASCII_table[' '];			//	Empty field,
		number_arr[2] = HEX_table[position_1];		//	Writing to the appropriate position in the array,
		number_arr[3] = HEX_table[position_0];		//	Writing to the appropriate position in the array,
	}

	//	############################################################################################
	//	Number conversion to signs needed to display in hex representations and with system number info,
	else if( system == HEX_SYS )
	{
		uint8_t position_0 = 0;		//	Variable to storage ones from number in hex format,
		uint8_t position_1 = 0;		//	Variable to storage tens from number in hex format,

		position_0 = number & 0b00001111;			//	Counting ones,
		position_1 = (number >> 4) & 0b00001111;	//	Counting tens,

		number_arr[0] = ASCII_table[' '];			//	Empty field,
		number_arr[1] = HEX_table[position_1];		//	Writing to the appropriate position in the array,
		number_arr[2] = HEX_table[position_0];		//	Writing to the appropriate position in the array,
		number_arr[3] = ASCII_table['h'];			//	Writing 'h' sign, it means HEX format,
	}

	//	############################################################################################
	//	If in function call we set wrong system number, name we get error information,
	else
	{
		while(1)
		{
			//	Writing info about error:
			TM1637_text(ptr, "Eror", 1000);
			TM1637_text(ptr, " SYS", 500);
			TM1637_text(ptr, " nok", 500);
		}
	}
}

void TM1637_number(TM1637_t *ptr, const uint16_t number, const uint8_t system, const uint16_t duration)
{
	//	-function to show number on display in chosen system number,

	char buffer_arr[NUMBER_OF_DISPLAYS];

	TM1637_number_conversion(ptr, number, system, buffer_arr);	//	Number conversion to chosen system,
	TM1637_write_display(ptr, buffer_arr);

	timer_text = duration;
	while(timer_text);
}

/*
void TM1637_set_clock(TM1637_t *ptr, RTC_t *pRTC)
{
	//	-funkcja do ustawiania czasu pochodzącego z zegara DS3231,





}
*/

//	////////////////////////////////////////////////////////////////////////////
void TM1637_set_brightness(TM1637_t *ptr, uint8_t up_down)
{
	if( (ptr->brightness < DISP_ON_8) && (up_down == 1) )
	{
		ptr->brightness++;
		TM1637_cmd(ptr, ptr->brightness);	//	Selected display brightness,
	}
	else if( (ptr->brightness > DISP_ON_1) && (up_down == 0) )
	{
		ptr->brightness--;
		TM1637_cmd(ptr, ptr->brightness);	//	Selected display brightness,
	}
}

uint8_t TM1637_get_brightness(TM1637_t *ptr)
{
	return ptr->brightness;
}

void TM1637_show_brightness(TM1637_t *ptr)
{
	char buffer_arr[NUMBER_OF_DISPLAYS];

	buffer_arr[0] = 'L';
	buffer_arr[1] = 'E';
	buffer_arr[2] = 'v';
	buffer_arr[3] = '0' + ptr->brightness - DISP_ON_1 + 1;

	TM1637_text(ptr, buffer_arr, 0);
}


//	////////////////////////////////////////////////////////////////////////////
void TM1637_clock(TM1637_t *ptr, const uint8_t hours, const uint8_t minutes, const uint8_t dot)
{
	//	-display time parameters: hours:minutes and dot,

	char buffer_arr[NUMBER_OF_DISPLAYS];
	char buffer_arr_temp[NUMBER_OF_DISPLAYS] = {0};

	TM1637_number_conversion(ptr, hours, DEC, buffer_arr_temp);	//	Number conversion to chosen system,
	buffer_arr[0] = buffer_arr_temp[2];
	buffer_arr[1] = buffer_arr_temp[3];

	TM1637_number_conversion(ptr, minutes, DEC, buffer_arr_temp);	//	Number conversion to chosen system,
	buffer_arr[2] = buffer_arr_temp[2];
	buffer_arr[3] = buffer_arr_temp[3];

	//	If on third display we don't have a number to set:
	if(buffer_arr[2] == ASCII_table[' '])
	{
		buffer_arr[2] = ASCII_table['0'];
	}

	//	If dot is enable to show:
	if(dot)
	{
		buffer_arr[1] = buffer_arr[1] + 0b10000000;
	}

	TM1637_write_display(ptr, buffer_arr);
}

void TM1637_clock_H_M_dot(TM1637_t *ptr, const uint8_t hours, const uint8_t minutes, const uint8_t dot)
{
	//	-wyświetlenie wartości godzin i minut na wyświetlaczu LED,
	//	-na pozycjach 0 i 1 wartość godzin w systemie dziesiętnym,
	//	-na pozycjach 2 i 3 wartość minut w systemie dziesiętnym,
	//	-zapalenie dwukropka oznacza zmianę wskazania sekund,
	//	-przyjmujemy, że przez pierwsze 500 [ms] dwukropek jest zapalony, a przez kolejne 500 [ms] wygaszony,
	//	-argumenty wejściowe w postaci liczb w formacie dziesiętnym BCD,

	char buffer_arr[NUMBER_OF_DISPLAYS] = {0};	//	Tablica na skonwertowane znaki,
	char buffer_temp[NUMBER_OF_DISPLAYS] = {0};	//	Tablica na skonwertowane znaki,


	if(hours < 100)
	{
		TM1637_number_conversion(ptr, hours, DEC, buffer_temp);	//	Konwersja godzin na znaki do wyświetlenia,
		buffer_arr[0] = buffer_temp[2];
		buffer_arr[1] = buffer_temp[3];

		if(buffer_arr[0] == HEX_table[0])
			buffer_arr[0] = ASCII_table[' '];

	}
	if(minutes < 100)
	{
		TM1637_number_conversion(ptr, minutes, DEC, buffer_temp);	//	Konwersja minut na znaki do wyświetlenia,
		buffer_arr[2] = buffer_temp[2];
		buffer_arr[3] = buffer_temp[3];

		if(buffer_arr[2] == ASCII_table[' '])
			buffer_arr[2] = HEX_table[0];
	}

	if( dot )
		buffer_arr[1] = buffer_arr[1] + 0b10000000;		//	Zapal dwukropek na wyświetlaczu drugim od lewej,

	TM1637_write_display(ptr, buffer_arr);	//	Wyświetlenie znaków na wyświetlaczu,
}

void TM1637_RC5(TM1637_t *ptr, const uint8_t address, const uint8_t command, const uint8_t toggle)
{
	//	-wyświetlenie wartości godzin i minut na wyświetlaczu LED,
	//	-na pozycjach 0 i 1 wartość godzin w systemie dziesiętnym,
	//	-na pozycjach 2 i 3 wartość minut w systemie dziesiętnym,
	//	-zapalenie dwukropka oznacza zmianę wskazania sekund,
	//	-przyjmujemy, że przez pierwsze 500 [ms] dwukropek jest zapalony, a przez kolejne 500 [ms] wygaszony,
	//	-argumenty wejściowe w postaci liczb w formacie dziesiętnym BCD,

	char buffer_arr[NUMBER_OF_DISPLAYS] = {0};	//	Tablica na skonwertowane znaki,
	char buffer_temp[NUMBER_OF_DISPLAYS] = {0};	//	Tablica na skonwertowane znaki,


	if(address < 100)
	{
		TM1637_number_conversion(ptr, address, DEC, buffer_temp);	//	Konwersja godzin na znaki do wyświetlenia,
		buffer_arr[0] = buffer_temp[2];
		buffer_arr[1] = buffer_temp[3];

		if(buffer_arr[0] == HEX_table[0])
			buffer_arr[0] = ASCII_table[' '];

	}
	if(command < 100)
	{
		TM1637_number_conversion(ptr, command, DEC, buffer_temp);	//	Konwersja minut na znaki do wyświetlenia,
		buffer_arr[2] = buffer_temp[2];
		buffer_arr[3] = buffer_temp[3];

		if(buffer_arr[2] == ASCII_table[' '])
			buffer_arr[2] = HEX_table[0];
	}

	if( toggle )
		buffer_arr[1] = buffer_arr[1] + 0b10000000;		//	Zapal dwukropek na wyświetlaczu drugim od lewej,

	TM1637_write_display(ptr, buffer_arr);	//	Wyświetlenie znaków na wyświetlaczu,
}


//	////////////////////////////////////////////////////////////////////////////
void TM1637_show_temperature(TM1637_t *pTM1637, int8_t temperature)
{
	//	-funkcja służąca do wyświetlenia wartości temperatury,
	//	-do zastosowania na cztero segmentowym wyświetlaczu bez kropki,
	//	-zakres wyświetlanej temperatury to -9 do 99 stopnia celsjusza,

	char buffer_arr[NUMBER_OF_DISPLAYS];

	if(temperature > 99)
	{
		TM1637_text(pTM1637, " Hot", 0);
	}
	else if(temperature < -9)
	{
		TM1637_text(pTM1637, "CoLd", 0);
	}
	else
	{
		if(temperature < 0)
		{
			temperature = temperature & 0b01111111;
			temperature = (temperature ^ 0b01111111) + 1;
			TM1637_number_conversion(pTM1637, temperature, DEC, buffer_arr);	//	Konwersja minut na znaki do wyświetlenia,
			buffer_arr[0] = ASCII_table['-'];
			buffer_arr[1] = buffer_arr[3];
		}
		else
		{
			TM1637_number_conversion(pTM1637, temperature, DEC, buffer_arr);	//	Konwersja minut na znaki do wyświetlenia,
			buffer_arr[0] = buffer_arr[2];
			buffer_arr[1] = buffer_arr[3];
		}

		buffer_arr[2] = ASCII_table[' '];
		buffer_arr[3] = ASCII_table['C'];
		TM1637_write_display(pTM1637, buffer_arr);
	}
}



