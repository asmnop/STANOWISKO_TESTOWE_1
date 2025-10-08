/*
 * spi.h
 *
 *  Created on: 25 maj 2025
 *      Author: asmnop
 */

#ifndef SPI_H_
#define SPI_H_


#include "universal_lib.h"


typedef struct
{
	uint8_t R1;
	uint8_t R7[4];
}SPI_t;


void SPI_init(void);
uint8_t SPI_send(uint8_t data);
uint8_t SPI_receive(void);
void SPI_set_speed(uint32_t speed);


#endif /* SPI_H_ */





