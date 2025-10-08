/*
 * gps.h
 *
 *  Created on: 11 maj 2025
 *      Author: mm
 */

#ifndef GPS_H_
#define GPS_H_


#include "universal_lib.h"

#include "../../LIB/Inc/usart.h"
#include "../../LIB/Inc/terminal.h"

typedef struct
{
	USART_t *pUSARTx;	//	Wybór jednostki USART,
	char data[6][90];
	uint8_t flag_ready[6];


	char RMC[75];
	char VTG[40];
	char GGA[82];
	char GSA[67];
	char GSV[60];
	char GLL[51];
}GPS_t;


void GPS_receive(GPS_t *pGPS, char data);
void GPS_TERMINAL_UTC_time(TERMINAL_t *pTERMINAL, GPS_t *pGPS, const uint8_t line, const uint8_t row_1, const uint8_t row_2);
void GPS_TERMINAL_coordinates(TERMINAL_t *pTERMINAL, GPS_t *pGPS, const uint8_t line, const uint8_t row_1, const uint8_t row_2);
void GPS_TERMINAL_satelite_ID(TERMINAL_t *pTERMINAL, GPS_t *pGPS, const uint8_t line, const uint8_t row_1, const uint8_t row_2);
void GPS_TERMINAL_height_above_sea_level(TERMINAL_t *pTERMINAL, GPS_t *pGPS, const uint8_t line, const uint8_t row_1, const uint8_t row_2);

void GPS_UTC_time_string(GPS_t *pGPS, char data_UTC[]);
void GPS_coordinates_string(GPS_t *pGPS, char data[], const uint8_t type);
void GPS_coordinates_string_2(GPS_t *pGPS, char data[], const uint8_t type);

#endif /* GPS_H_ */


