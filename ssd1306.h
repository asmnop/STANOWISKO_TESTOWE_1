/*
 * ssd1306.h
 *
 *  Created on: 13 maj 2025
 *      Author: asmnop
 */

#ifndef SSD1306_H_
#define SSD1306_H_


#include "universal_lib.h"

#include "../../LIB/Inc/i2c.h"
#include "../../LIB/Inc/fonts.h"

#define OLED_COMMAND				0x00
#define OLED_DATA					0xC0
#define OLED_DATA_CONTINUE			0x40

//	1. FUNDAMENTAL COMMAND TABLE:
#define OLED_SET_CONTRAST_CONTROL	0x81
#define OLED_DISPLAY_ALL_ON_RESUME	0xA4
#define OLED_DISPLAY_ALL_ON			0xA5
#define OLED_NORMAL_DISPLAY			0xA6
#define OLED_INVERT_DISPLAY			0xA7
#define OLED_DISPLAY_OFF			0xAE
#define OLED_DISPLAY_ON				0xAF
#define OLED_NOP					0xE3

//	2. SCROLLING COMMAND TABLE:
#define OLED_HORIZONTAL_SCROLL_RIGHT				0x26
#define OLED_HORIZONTAL_SCROLL_LEFT					0x27
#define OLED_HORIZONTAL_SCROLL_VERTICAL_AND_RIGHT	0x29
#define OLED_HORIZONTAL_SCROLL_VERTICAL_AND_LEFT	0x2A
#define OLED_DEACTIVATE_SCROLL						0x2E
#define OLED_ACTIVATE_SCROLL						0x2F
#define OLED_SET_VERTICAL_SCROLL_AREA				0xA3

#define OLED_SET_LOWER_COLUMN		0x00
#define OLED_SET_HIGHER_COLUMN		0x10

//	3. ADDRESSING SETTING COMMAND TABLE:
#define OLED_MEMORY_ADDRESSING_MODE		0x20
#define OLED_HORIZONTAL_ADDRESSING_MODE	0x00
#define OLED_VERTICAL_ADDRESSING_MODE	0x01
#define OLED_PAGE_ADDRESSING_MODE		0x02

#define OLED_SET_COLUMN_ADDR			0x21
#define OLED_SET_PAGE_ADDR				0x22

//	4. HARDWARE CONFIGURATION (PANEL RESOLUTION & LAYOUT RELATED) COMMAND TABLE:
#define OLED_SET_START_LINE			0x40
#define OLED_SET_SEGMENT_REMAP		0xA0
#define OLED_SET_MULTIPLEX_RATIO	0xA8
#define OLED_COM_SCAN_DIR_INC		0xC0
#define OLED_COM_SCAN_DIR_DEC		0xC8
#define OLED_SET_DISPLAY_OFFSET		0xD3
#define OLED_SET_COM_PINS			0xDA
#define OLED_CHARGE_PUMP			0x8D

//	5. TIMING & DRIVING SCHEME SETTING COMMAND TABLE:
#define OLED_SET_DISPLAY_CLOCK_DIV_RATIO	0xD5
#define OLED_SET_PRECHARGE_PERIOD			0xD9
#define OLED_SET_VCOM_DESELECT				0xDB


typedef struct
{
	I2C_t *I2Cx;
	uint8_t address;
	uint8_t line;					//	Ilość linii w wyświetlaczu: 64
	uint8_t column;					//	Ilość kolumn w wyświetlaczu: 128,
	uint8_t memory_addressing_mode;
	uint8_t memory_buffer[8][128];
}OLED_t;



void SSD1306_init(OLED_t *pOLED);
void SSD1306_point(OLED_t *pOLED, uint8_t x, uint8_t y, uint8_t p);
void SSD1306_draw_string(OLED_t *pOLED, const uint8_t x, const uint8_t y, const FONT_t *pFONT, const char string[], const uint8_t invert);
void SSD1306_refresh(OLED_t *pOLED);
void SSD1306_clear_display(OLED_t *pOLED);


void SSD1306_show_info(OLED_t *pOLED);
void SSD1306_show_GPS(OLED_t *pOLED);


#endif /* SSD1306_H_ */








