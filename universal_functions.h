/*
 * universal_functions.h
 *
 *  Created on: 25 sie 2024
 *      Author: asmnop
 */

#ifndef UNIVERSAL_FUNCTIONS_H_
#define UNIVERSAL_FUNCTIONS_H_

#include "universal_lib.h"

void delay_ms(const uint16_t delay_ms);
uint8_t dec_to_bcd(const uint8_t num);
uint8_t bcd_to_int(const uint8_t bcd);
uint8_t inc(uint8_t variable, const uint8_t max_value);
uint8_t dec(uint8_t variable, const uint8_t max_value);
uint16_t measure_time(void);
char* ftoa(float n, char *res, int afterpoint);
char* itoa_space(int number, char *ptr, int system, uint8_t space);

#endif /* UNIVERSAL_FUNCTIONS_H_ */
