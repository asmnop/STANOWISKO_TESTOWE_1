/*
 * gpio.h
 *
 *  Created on: 21 sie 2024
 *      Author: asmnop
 */

#ifndef GPIO_H_
#define GPIO_H_


#include "universal_lib.h"


typedef struct
{
	GPIO_TypeDef* port;
	uint16_t pin;
	uint32_t pull;
	uint32_t speed;
	uint32_t mode;
	uint32_t output_type;
	uint32_t AF;
	uint8_t number;
	uint8_t level;
	uint8_t key_state;
}LINE_t;


void GPIO_set_pin(LINE_t *pLine);
void GPIO_reset_pin(LINE_t *pLine);
void GPIO_toggle_pin(LINE_t *pLine);
void GPIO_set_pin_as_input(LINE_t *pLINE);
void GPIO_set_pin_as_output(LINE_t *pLine);
void GPIO_set_pin_as_analog(LINE_t *pLine);
uint32_t GPIO_get_input_pin(LINE_t *pLine);
uint8_t GPIO_get_output_pin(LINE_t *pLine);


#endif /* GPIO_H_ */




