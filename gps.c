/*
 * gps.c
 *
 *  Created on: 11 maj 2025
 *      Author: mm
 */


#include "../../LIB/Inc/gps.h"



#define GPS_NUMBER_OF_SIGNS		6
#define GPS_NUMBER_OF_SENTENCES	6

#define GPS_RMC	0
#define GPS_VTG	1
#define GPS_GGA	2
#define GPS_GSA	3
#define GPS_GSV	4
#define GPS_GLL	5


void GPS_receive(GPS_t *pGPS, char data)
{
	enum GPS_work_mode
	{
		GPS_WAIT_FOR_$,
		GPS_CHECKING,
		GPS_RECEIVE,
		GPS_END,
	};

	static uint8_t show_line = 0;
	static uint8_t pointer = 0;
	static uint8_t work_mode = 0;

	char sentence[GPS_NUMBER_OF_SENTENCES][GPS_NUMBER_OF_SIGNS] = {"$GPRMC", "$GPVTG", "$GPGGA", "$GPGSA", "$GPGSV", "$GPGLL"};

	if((data == '$') && (work_mode == GPS_WAIT_FOR_$))
	{
		work_mode = GPS_CHECKING;
	}

	if((data == 10) || (data == 13))
	{
		work_mode = GPS_WAIT_FOR_$;
	}

	if(work_mode == GPS_RECEIVE)
	{
		pGPS->data[show_line][pointer] = data;
		pointer++;
	}

	if(work_mode == GPS_CHECKING)
	{
		for(uint8_t i=0; i<GPS_NUMBER_OF_SENTENCES; i++)
		{
			if(USART_check_string(pGPS->pUSARTx, &sentence[i][0], GPS_NUMBER_OF_SIGNS) == 0)
			{
				show_line = i;

				for(uint8_t j=0; j<90; j++)
				{
					pGPS->data[show_line][j] = '\0';
				}

				pointer = 6;

				pGPS->data[show_line][0] = sentence[i][0];
				pGPS->data[show_line][1] = sentence[i][1];
				pGPS->data[show_line][2] = sentence[i][2];
				pGPS->data[show_line][3] = sentence[i][3];
				pGPS->data[show_line][4] = sentence[i][4];
				pGPS->data[show_line][5] = sentence[i][5];

				work_mode = GPS_RECEIVE;
			}
		}
	}

	if((data == '*') && (work_mode == GPS_RECEIVE))
	{
		work_mode = GPS_END;
		pGPS->flag_ready[show_line] = 1;
	}


}

void GPS_convert(GPS_t *pGPS)
{
	//	-konwersja danych odebranym z GPS na dane czytelne dla człowieka,



}


static uint8_t GPS_field_position(GPS_t *pGPS, const uint8_t sentence, const uint8_t field_number)
{
	uint8_t match = 0;
	uint8_t counter = 0;

	do
	{
		if(pGPS->data[sentence][counter] == ',')
		{
			match++;
		}

		counter++;
	}while(match != (field_number - 1));

	return counter;
}

static uint8_t GPS_field_length(GPS_t *pGPS, const uint8_t sentence, const uint8_t field_number)
{
	//	-obliczenie wielkości danego pola w ilości znaków,


	return GPS_field_position(pGPS, sentence, field_number + 1) - GPS_field_position(pGPS, sentence, field_number) - 1;

	uint8_t match = 0;
	uint8_t counter = 0;
	uint8_t counter_2 = 0;

	do
	{
		if(pGPS->data[sentence][counter] == ',')
		{
			match++;
		}

		counter++;
	}while(match != (field_number - 1));

	match = 0;
	do
	{
		if(pGPS->data[sentence][counter_2] == ',')
		{
			match++;
		}

		counter_2++;
	}while(match != field_number);

	return counter_2 - counter - 1;

}



void GPS_TERMINAL_UTC_time(TERMINAL_t *pTERMINAL, GPS_t *pGPS, const uint8_t line, const uint8_t row_1, const uint8_t row_2)
{
	TERMINAL_line(pTERMINAL, line, row_1, "GPS_TIME [hh:mm:ss:msms] - ");

	//	GODZINY:
	TERMINAL_number(pTERMINAL, line, row_2, pGPS->data[0][7] - 48, 1);
	TERMINAL_number(pTERMINAL, line, row_2 + 1, pGPS->data[0][8] - 48, 1);
	TERMINAL_line(pTERMINAL, line, row_2 + 2, ":");

	//	MINUTY:
	TERMINAL_number(pTERMINAL, line, row_2 + 3, pGPS->data[0][9] - 48, 1);
	TERMINAL_number(pTERMINAL, line, row_2 + 4, pGPS->data[0][10] - 48, 1);
	TERMINAL_line(pTERMINAL, line, row_2 + 5, ":");

	//	SEKUNDY:
	TERMINAL_number(pTERMINAL, line, row_2 + 6, pGPS->data[0][11] - 48, 1);
	TERMINAL_number(pTERMINAL, line, row_2 + 7, pGPS->data[0][12] - 48, 1);
	TERMINAL_line(pTERMINAL, line, row_2 + 8, ":");

	//	MILISEKUNDY:
	TERMINAL_number(pTERMINAL, line, row_2 + 9, pGPS->data[0][14] - 48, 1);
	TERMINAL_number(pTERMINAL, line, row_2 + 10, pGPS->data[0][15] - 48, 1);
}

void GPS_TERMINAL_coordinates(TERMINAL_t *pTERMINAL, GPS_t *pGPS, const uint8_t line, const uint8_t row_1, const uint8_t row_2)
{
	const uint8_t offset = 14;

	for(uint8_t i=0; i<2; i++)
	{
		if(i==0)
		{
			TERMINAL_line(pTERMINAL, line, row_1, "SZEROKOSC GEOGRAFICZNA:");
		}
		else
		{
			TERMINAL_line(pTERMINAL, line + i, row_1, "DLUGOSC GEOGRAFICZNA:");
		}


		//	STOPNIE:
		TERMINAL_number(pTERMINAL, line + i, row_2, pGPS->data[GPS_RMC][19 + i*offset] - 48, 1);
		TERMINAL_number(pTERMINAL, line + i, row_2 + 1, pGPS->data[GPS_RMC][20 + i*offset] - 48, 1);
		TERMINAL_line(pTERMINAL, line + i, row_2 + 2, "*");

		//	MINUTY:
		TERMINAL_number(pTERMINAL, line + i, row_2 + 3, pGPS->data[GPS_RMC][21 + i*offset] - 48, 1);
		TERMINAL_number(pTERMINAL, line + i, row_2 + 4, pGPS->data[GPS_RMC][22 + i*offset] - 48, 1);
		TERMINAL_line(pTERMINAL, line + i, row_2 + 5, "'");

		//	SEKUNDY:
		uint32_t seconds = 0;

		seconds = (pGPS->data[GPS_RMC][24 + i*offset] - 48) * 10000 +
				  (pGPS->data[GPS_RMC][25 + i*offset] - 48) * 1000 +
				  (pGPS->data[GPS_RMC][26 + i*offset] - 48) * 100 +
				  (pGPS->data[GPS_RMC][27 + i*offset] - 48) * 10 +
				  (pGPS->data[GPS_RMC][28 + i*offset] - 48);

		seconds = (seconds * 60);

		TERMINAL_line(pTERMINAL, line + i, row_2 + 6, "     ");

		if(seconds < 1000000)
		{
			TERMINAL_number(pTERMINAL, line + i, row_2 + 6, 0, 1);
			TERMINAL_number(pTERMINAL, line + i, row_2 + 7, seconds, 1);
		}
		else
		{
			TERMINAL_number(pTERMINAL, line + i, row_2 + 6, seconds, 2);
		}

		TERMINAL_line(pTERMINAL, line + i, row_2 + 8, ".");


		while(seconds > 999999)
		{
			seconds = seconds - 1000000;
		}

		while(seconds > 99999)
		{
			seconds = seconds - 100000;
		}

		TERMINAL_number(pTERMINAL, line + i, row_2 + 9, seconds, 3);

		TERMINAL_line(pTERMINAL, line + i, row_2 + 12, "\"");
	}
}

void GPS_TERMINAL_satelite_ID(TERMINAL_t *pTERMINAL, GPS_t *pGPS, const uint8_t line, const uint8_t row_1, const uint8_t row_2)
{
	const uint8_t offset = 3;

	TERMINAL_line(pTERMINAL, line, row_1, "ID SATELITOW:");

	for(uint8_t i=0; i<6; i++)
	{
		TERMINAL_number(pTERMINAL, line, row_2  + i*offset, pGPS->data[GPS_GSA][11 + i*offset] - 48, 1);
		TERMINAL_number(pTERMINAL, line, row_2 + 1 + i*offset, pGPS->data[GPS_GSA][12 + i*offset] - 48, 1);
		TERMINAL_line(pTERMINAL, line, row_2 + 2 + i*offset, " ");
	}
}

void GPS_TERMINAL_height_above_sea_level(TERMINAL_t *pTERMINAL, GPS_t *pGPS, const uint8_t line, const uint8_t row_1, const uint8_t row_2)
{
	const uint8_t height_field = 9;
	uint8_t match = 0;
	uint8_t counter = 0;
	uint8_t counter_2 = 0;

	if(pGPS->flag_ready[GPS_GGA] == 0)
	{
		return;
	}

	pGPS->flag_ready[GPS_GGA] = 0;

	/*
	//	Obliczenie komórki, od której zaczyna się informacja o wysokości nad poziomem morza,
	do
	{
		if(pGPS->data[GPS_GGA][counter] == ',')
		{
			match++;
		}

		counter++;
	}while(match != height_field);


	match = 0;
	do
	{
		if(pGPS->data[GPS_GGA][counter_2] == ',')
		{
			match++;
		}

		counter_2++;
	}while(match != (height_field + 1));

*/

	TERMINAL_line(pTERMINAL, line, row_1, "WYSOKOSC NAD POZIOMEM MORZA:");

	TERMINAL_arr(pTERMINAL, line, row_2, &pGPS->data[GPS_GGA][GPS_field_position(pGPS, GPS_GGA, 10)], GPS_field_length(pGPS, GPS_GGA, 10));
	//TERMINAL_number(pTERMINAL, line, row_2, counter_2 - counter - 1, 10);

	/*
	for(uint8_t i=0; i<counter_2 - counter; i++)
	{
		if(pGPS->data[GPS_GGA][counter + i] == '.')
		{
			TERMINAL_line(pTERMINAL, line, row_2 + i, ".");
			//TERMINAL_line(pTERMINAL, line, row_2 + i + 1, "=");
			TERMINAL_number(pTERMINAL, line, row_2 + i + 1, pGPS->data[GPS_GGA][counter + i + 1] - 48, 1);
			return;
			//TERMINAL_line(pTERMINAL, line, row_2 + i, "@@");
		}
		else
		{
			TERMINAL_number(pTERMINAL, line, row_2 + i, pGPS->data[GPS_GGA][counter + i] - 48, 1);
		}
	}
	*/
}


void GPS_UTC_time_string(GPS_t *pGPS, char data_UTC[])
{
	//	-create string made by signs,
	//	-string is information about UTC time,

	data_UTC[0] = pGPS->data[GPS_RMC][7];
	data_UTC[1] = pGPS->data[GPS_RMC][8];
	data_UTC[2] = ':';
	data_UTC[3] = pGPS->data[GPS_RMC][9];
	data_UTC[4] = pGPS->data[GPS_RMC][10];
	data_UTC[5] = ':';
	data_UTC[6] = pGPS->data[GPS_RMC][11];
	data_UTC[7] = pGPS->data[GPS_RMC][12];
	data_UTC[8] = '\0';
}

void GPS_coordinates_string(GPS_t *pGPS, char data[], const uint8_t type)
{
	uint8_t offset = 0;

	if(type == 1)
	{
		offset = 14;
		data[0] = pGPS->data[GPS_RMC][32];
	}
	else
	{
		data[0] = ' ';
	}

	data[1] = pGPS->data[GPS_RMC][19 + offset];
	data[2] = pGPS->data[GPS_RMC][20 + offset];
	data[3] = '*';
	data[4] = pGPS->data[GPS_RMC][21 + offset];
	data[5] = pGPS->data[GPS_RMC][22 + offset];
	data[6] = '\'';
	data[7] = pGPS->data[GPS_RMC][24 + offset];
	data[8] = pGPS->data[GPS_RMC][25 + offset];
	data[9] = pGPS->data[GPS_RMC][26 + offset];
	data[10] = pGPS->data[GPS_RMC][27 + offset];
	data[11] = pGPS->data[GPS_RMC][28 + offset];
	data[12] = '\0';
}

void GPS_coordinates_string_2(GPS_t *pGPS, char data[], const uint8_t type)
{
	uint8_t offset = 0;

	if(type == 1)
	{
		offset = 14;
		data[0] = pGPS->data[GPS_RMC][32];
	}
	else
	{
		data[0] = ' ';
	}

	data[1] = pGPS->data[GPS_RMC][19 + offset];
	data[2] = pGPS->data[GPS_RMC][20 + offset];
	data[3] = '*';
	data[4] = pGPS->data[GPS_RMC][21 + offset];
	data[5] = pGPS->data[GPS_RMC][22 + offset];
	data[6] = '\'';

	//	SEKUNDY:
	uint32_t seconds = 0;

	seconds = (pGPS->data[GPS_RMC][24 + offset] - 48) * 10000 +
			  (pGPS->data[GPS_RMC][25 + offset] - 48) * 1000 +
			  (pGPS->data[GPS_RMC][26 + offset] - 48) * 100 +
			  (pGPS->data[GPS_RMC][27 + offset] - 48) * 10 +
			  (pGPS->data[GPS_RMC][28 + offset] - 48);

	seconds = (seconds * 60);

	uint8_t counter = 0;

	//	5
	while(seconds > 999999)
	{
		seconds = seconds - 1000000;
		counter++;
	}
	data[7] = '0' + counter;
	counter = 0;

	//	4
	while(seconds > 99999)
	{
		seconds = seconds - 100000;
		counter++;
	}
	data[8] = '0' + counter;
	counter = 0;

	data[9] = '.';

	//	3
	while(seconds > 9999)
	{
		seconds = seconds - 10000;
		counter++;
	}
	data[10] = '0' + counter;
	counter = 0;

	//	2
	while(seconds > 999)
	{
		seconds = seconds - 1000;
		counter++;
	}
	data[11] = '0' + counter;
	counter = 0;

	//	1
	while(seconds > 99)
	{
		seconds = seconds - 100;
		counter++;
	}
	data[12] = '0' + counter;
	counter = 0;

	data[13] = '"';

	data[14] = '\0';
}



