/*
 * DS18B20.h
 *
 *  Created on: 24 wrz 2024
 *      Author: asmnop
 */


#ifndef DS18B20_H_
#define DS18B20_H_


#include "universal_lib.h"
#include "..\..\LIB\Inc\usart.h"


#define DS18B20_RES_9	0b00011111
#define DS18B20_RES_10	0b00111111
#define DS18B20_RES_11	0b01011111
#define DS18B20_RES_12	0b01111111


typedef struct
{
	USART_t *pUSARTx;
	uint8_t ROM[8];			//	Zawartość kodu ROM,
	uint8_t scratchpad[9];	//	Zawartość scratchpada,
	uint8_t temp_total;		//	Wartość temperatury w wartościach całkowitych,
	uint16_t temp_decimal;	//	Wartość temperatury po przecinku,
	uint8_t flag;			//	Flaga mówiąca o zakończeniu odświerzania rejestrów temperatury,
}DS18B20_t;


uint8_t DS18B20_read_rom_code(DS18B20_t *pDS18B20);
uint8_t DS18B20_read_scratchpad(DS18B20_t *pDS18B20);
uint8_t DS18B20_read_temperature(DS18B20_t *pDS18B20);
uint8_t DS18B20_convert_T(DS18B20_t *pDS18B20);
uint8_t DS18B20_compute_temperature(DS18B20_t *pDS18B20);
uint8_t DS18B20_get_temperature_total(DS18B20_t *pDS18B20);
uint16_t DS18B20_get_temperature_decimal(DS18B20_t *pDS18B20);
void DS18B20_set_resolution(DS18B20_t *pDS18B20, const uint8_t resolution);
uint8_t DS18B20_get_resolution(DS18B20_t *pDS18B20);
uint8_t DS18B20_get_alarm_H(DS18B20_t *pDS18B20);
uint8_t DS18B20_get_alarm_L(DS18B20_t *pDS18B20);
void DS18B20_set_alarm(DS18B20_t *pDS18B20, const uint8_t H, const uint8_t L);

int8_t DS18B20_total_number_temperature(DS18B20_t *pDS18B20);




#endif /* DS18B20_H_ */






