/*
 * VT100.h
 *
 *  Created on: 15 wrz 2024
 *      Author: asmnop
 */

#ifndef VT100_H_
#define VT100_H_

#include "universal_lib.h"
#include "..\..\LIB\Inc\usart.h"
#include "..\..\LIB\Inc\universal_data.h"


//	ATRYBUTY ZNAKU:
#define RESET		0
#define BOLD 		1
#define DIM			2
#define UNDERLINE 	3
#define BLINK		4
#define REVERSE		7
#define HIDDEN		8


//	KOLORY CZCIONKI LUB TŁA:
#define BLACK		0
#define RED			1
#define GREEN		2
#define YELLOW		3
#define BLUE		4
#define MAGENTA		5
#define CYAN		6
#define WHITE		7

//	NAZWY PRZYCISKÓW KLAWIATURY:
#define ENTER		'\x0D'
#define BACKSPACE	'\x08'








void VT100_cursor_hide(const uint8_t cur_on_off);
void VT100_cls(const uint8_t cur_on_off);
void VT100_fill_line(const uint8_t ascii, const uint8_t length);
void VT100_next_line(const uint8_t lines);
void VT100_move_cursor_left(const uint8_t lines);
void VT100_move_cursor_right(const uint8_t lines);
void VT100_EOL(void);
void VT100_location(const uint8_t row, const uint8_t column);
void VT100_text(const char *string);
void VT100_number(const uint16_t number);
void VT100_unumber(const uint16_t number);
void VT100_float(const float number, const uint8_t decimal);
void VT100_pen_color(const uint8_t color);
void VT100_back_color(const uint8_t color);
void VT100_set_tab(void);
void VT100_main_title(const char *string);
void VT100_title(const char *string);
void VT100_show_data(const char *string, const int16_t number);
void VT100_show_data_16(const uint16_t reg, const char *name);
void VT100_show_register(const uint32_t reg, const char *name);
uint16_t VT100_create_number(void);
void VT100_send_USART(void);
void VT100_init_connection(void);
uint8_t VT100_cmd(const char command[], const uint8_t length);
uint8_t VT100_command(const char command[], const uint8_t length);
uint8_t VT100_init_connection_2(void);
uint8_t VT100_init_connection_3(void);
void VT100_erase_line(void);
void VT100_data_arr(uint8_t *data, uint8_t number, const char* const names[]);

void USART_arr(USART_t *pUSART, uint8_t *data, uint8_t number, const char* const names[]);

extern USART_t USART_2;
extern const char* const pGPIO[];
extern const char* const pTIM[];
void DES(void);
void VT100_show_periph(uint32_t reg, const char* const names[], const char *string);






#endif /* VT100_H_ */





