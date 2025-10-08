/*
 * EEPEM.h
 *
 *  Created on: 14 maj 2024
 *      Author: asmnop
 */


#ifndef INC_EEPROM_H_
#define INC_EEPROM_H_


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
	uint8_t dev_address;	//	Adres układu,
	uint8_t pages;			//	Ilość stron danych.
	const uint8_t bytes_on_page;	//	Ilość bajtów na stronie
	uint16_t address_register;
}EEPROM_t_new;


uint8_t EEPROM_write_byte(EEPROM_t_new *pEEPROM, const uint16_t mem_address, uint8_t data);
uint8_t EEPROM_read_byte(EEPROM_t_new *pEEPROM, const uint16_t mem_address);
void EEPROM_write_page(EEPROM_t_new *pEEPROM, const uint8_t page, uint8_t data[]);
void EEPROM_read_page(EEPROM_t_new *pEEPROM, const uint8_t page);
void EEPROM_fill_page(EEPROM_t_new *pEEPROM, const uint8_t page, const char sign);
void EEPROM_erase_page(EEPROM_t_new *pEEPROM, const uint8_t page);
void EEPROM_erase_memory(EEPROM_t_new *pEEPROM);

uint8_t EEPROM_write(EEPROM_t_new *pEEPROM, const uint16_t mem_address, uint8_t data[], const uint8_t data_size);
uint8_t EEPROM_read(EEPROM_t_new *pEEPROM, const uint16_t mem_address, uint8_t data[], const uint8_t data_size);

#endif /* INC_EEPROM_H_ */




