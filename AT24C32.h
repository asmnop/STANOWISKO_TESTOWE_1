/*
 * AT24C32.h
 *
 *  Created on: 14 maj 2024
 *      Author: asmnop
 */


#ifndef INC_AT24C32_H_
#define INC_AT24C32_H_


#include "universal_lib.h"

#include "..\..\LIB\Inc\i2c.h"

//	PARAMETRY UKŁADY AT24C32:
//	ADRES:						0b01010111
//	ILOŚĆ STRON:				128
//	ILOŚĆ BAJTÓW NA STRONIE:	32


#define AT24C32_ADDR				0b01010111
#define AT24C32_NUMBER_OF_PAGES		128
#define AT24C32_BYTES_ON_PAGE		32


typedef struct
{
	I2C_t *I2Cx;			//	Instacja I2C,
	uint8_t address;		//	Adres układu,
	uint8_t pages;			//	Ilość stron danych.
	const uint8_t bytes_on_page;	//	Ilość bajtów na stronie,
}EEPROM_t;


uint8_t AT24C32_write_byte(EEPROM_t *pEEPROM, const uint8_t page, const uint8_t byte, uint8_t data);
uint8_t AT24C32_read_byte(EEPROM_t *pEEPROM, const uint8_t page, const uint8_t byte);
void AT24C32_write_page(EEPROM_t *pEEPROM, const uint8_t page, uint8_t data[]);
void AT24C32_read_page(EEPROM_t *pEEPROM, const uint8_t page);
void AT24C32_read_all(void);
void AT24C32_erase_page(EEPROM_t *pEEPROM, const uint8_t page);


uint8_t AT24C32_current_address_read(EEPROM_t *pEEPROM);
uint8_t AT24C32_random_read(EEPROM_t *pEEPROM, const uint8_t page, const uint8_t byte);


#endif /* INC_AT24C32_H_ */




