/*
 * one_wire.h
 *
 *  Created on: 22 wrz 2024
 *      Author: asmnop
 */


#ifndef ONE_WIRE_H_
#define ONE_WIRE_H_


#include "universal_lib.h"
#include "../../LIB/Inc/gpio.h"
#include "../../LIB/Inc/usart.h"


void OW_init(LINE_t *line, LINE_t *line2);
uint8_t OW_reset(USART_t *pUSART);
void OW_write(USART_t *pUSART, const uint8_t data);
uint8_t OW_read(USART_t *pUSART);
uint8_t OW_time_slot(USART_t *pUSART);


#endif /* ONE_WIRE_H_ */








