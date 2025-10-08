/*
 * i2c.h
 *
 *  Created on: 21 sie 2024
 *      Author: asmnop
 */


#ifndef I2C_H_
#define I2C_H_


#include "universal_lib.h"

#include "..\..\LIB\Inc\gpio.h"


#define I2C_NUMBER_OF_SCAN_ADDRESSES	10

typedef struct
{
	LINE_t *line_SCL;
	LINE_t *line_SDA;
	I2C_TypeDef *I2Cx;
	uint32_t timing_register;
	uint32_t enable_periph;
	uint8_t address[I2C_NUMBER_OF_SCAN_ADDRESSES];
}I2C_t;


void I2C_init(I2C_t *pI2Cx);
uint8_t I2C_scan(I2C_t *pI2Cx, uint8_t address);
void I2C_scan_all(I2C_t *pI2Cx);

void I2C_write_byte(I2C_t *pI2C, uint8_t address, const uint8_t data);
uint8_t I2C_write_bytes(I2C_t *pI2C, uint8_t dev_address, const uint8_t mem_address, const uint8_t data[], const uint8_t number_of_data);
void I2C_write_byte_no_reg(I2C_t *pI2C, const uint8_t address, const uint8_t data);
void I2C_write_bytes_no_reg(I2C_t *pI2C, const uint8_t dev_address, const uint8_t data[], const uint8_t data_size);

uint8_t I2C_read_byte(I2C_t *pI2C, uint8_t address, const uint8_t start_register);
uint8_t I2C_read_bytes(I2C_t *pI2C, uint8_t dev_address, const uint8_t mem_address, uint8_t data[], const uint8_t data_size);


uint8_t I2C_read_current_address(I2C_t *pI2C, uint8_t address);
uint8_t I2C_read_random(I2C_t *pI2C, uint8_t address, const uint16_t start_register);
uint8_t I2C_read_sequential(I2C_t *pI2C, uint8_t address, const uint16_t start_register, const uint8_t transfer_size, uint8_t data[]);
void I2C_write_byte_EE(I2C_t *pI2C, uint8_t address, const uint16_t start_register, const uint8_t data);
void I2C_write_page(I2C_t *pI2C, uint8_t address, const uint16_t start_register, const uint8_t numberOfData, const uint8_t data[]);

void I2C_write(I2C_t *pI2C, uint8_t dev_address, const uint16_t mem_address, const uint8_t mem_address_size, uint8_t data[], const uint8_t size);
uint8_t I2C_read(I2C_t *pI2C, uint8_t dev_address, const uint16_t mem_address, const uint8_t mem_address_size, uint8_t data[], const uint8_t data_size);


void I2C_write_byte_EE(I2C_t *pI2C, uint8_t dev_address, const uint16_t mem_address, const uint8_t data);
void I2C_write_bytes_EE(I2C_t *pI2C, uint8_t dev_address, const uint16_t mem_address, const uint8_t data[], const uint8_t data_size);
uint8_t I2C_read_byte_EE(I2C_t *pI2C, uint8_t dev_address, const uint16_t mem_address);
uint8_t I2C_read_bytes_EE(I2C_t *pI2C, uint8_t dev_address, const uint16_t mem_address, uint8_t data[], const uint8_t data_size);

#endif /* I2C_H_ */








