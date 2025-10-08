/*
 * ws2812.h
 *
 *  Created on: 25 mar 2025
 *      Author: asmnop
 */

#ifndef WS2812_H_
#define WS2812_H_


#include "universal_lib.h"
#include "../../LIB/Inc/gpio.h"
#include "../../LIB/Inc/pwm.h"


#define WS2812_LEDS		4

#define WS2812_RESET		4



typedef struct
{
	uint8_t R;
	uint8_t G;
	uint8_t B;
}WS2812_LED_t;

typedef struct
{
	uint8_t G[8];
	uint8_t R[8];
	uint8_t B[8];
}WS2812_TO_SEND_t;

typedef struct
{
	CHANNEL_t *signal_line;		//	Linia sterująca wypełnieniem PWM,
	WS2812_LED_t *led;
}WS2812_t;


void WS2812_init(WS2812_t *pWS2812, const uint8_t number);

void WS2812_fill_line(WS2812_LED_t *pWS2812_arr, WS2812_TO_SEND_t *pWS2812, uint8_t number);
void WS2812_light(WS2812_LED_t *pWS2812_arr, WS2812_TO_SEND_t *pWS2812, uint8_t number);
void WS2812_rotate(WS2812_LED_t *pWS2812_LED, WS2812_TO_SEND_t *pWS2812_TO_SEND, const uint8_t number, const uint8_t dir);
void WS2812_brightness(WS2812_LED_t *pWS2812_LED, WS2812_TO_SEND_t *pWS2812_TO_SEND, uint8_t number, int8_t R_bright, int8_t G_bright, int8_t B_bright);

#endif /* WS2812_H_ */








