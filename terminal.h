/*
 * terminal.h
 *
 *  Created on: 17 lis 2024
 *      Author: asmnop
 */

#ifndef TERMINAL_H_
#define TERMINAL_H_


#include "universal_lib.h"

#include "../../LIB/Inc/usart.h"
#include "../../LIB/Inc/dmamux.h"
#include "../../LIB/Inc/VT100.h"
#include "../../LIB/Inc/pid.h"



typedef struct
{
	uint8_t text_position;
	char text[20];
	uint8_t number_1_position;
	uint16_t* number_1;
	uint8_t number_2_position;
	void *number_2;
}TERMINAL_LINE_t;


typedef struct TERMINAL_t
{
	const uint8_t row;				//	Ilość wierszy,
	const uint8_t column;			//	Ilość kolumn, do których można wpisać znak do wyświetlenia,
	const uint8_t command_column;	//	Miejsce na przejście do nowej linii i powrót karetki,
	const uint8_t total_column;		//	Całkowita długość kolumny wraz z ewentualnymi komendami,
	const uint16_t total_size;		//	Całkowita wielkość tablicy,
	char *menu;						//	Wskaźnik na pierwszy element tablicy,
	uint8_t place[5];
	uint8_t field[10];
	//uint8_t instance;				//	Numer sekwencji danych wprowadzanych do terminala,
	char *name;						//	Nazwa menu terminala,
}TERMINAL_t;


typedef struct
{
	TERMINAL_t *terminal[9];		//	Wybór terminala, chodzi o jego rozmiar,
	void (*foo_term[9])(void);		//	Wskaźnik na funkcję, która obsługuje transfer przez DMA,
	void (*foo_menu[9])(void);		//	Wskaźnik na menu, które obsługuje wbrany terminal,
	uint8_t instance[9];			//	Numer instancji, jeśli dany terminal wypełniamy różnymi danymi,
	char sign[9];					//	Znak przełączający,
	uint8_t terminal_ptr;			//	Zmienna wyboru aktywnego terminala,
}TERMINAL_DISPLAY_t;






void TERMINAL_line(TERMINAL_t *pTERMINAL, const uint8_t row, const uint8_t column, const char *string);
void TERMINAL_number(TERMINAL_t *pTERMINAL, const uint8_t row, const uint8_t column, uint32_t number, uint8_t space);
void TERMINAL_float(TERMINAL_t *pTERMINAL, const uint8_t row, const uint8_t column, float number, uint8_t space);
void TERMINAL_arr(TERMINAL_t *pTERMINAL, const uint8_t row, const uint8_t column, char *string, const uint8_t number_of_data);
void TERMINAL_init(TERMINAL_t *pTERMINAL);
uint16_t TERMINAL_create_number(TERMINAL_t *pTERMINAL, const uint8_t row, const uint8_t column);
uint16_t TERMINAL_create_number_(TERMINAL_t *pTERMINAL, const uint8_t row, const uint8_t column);
float TERMINAL_create_float(TERMINAL_t *pTERMINAL, const uint8_t row, const uint8_t column);


void TERMINAL_enable(TERMINAL_t *pTERMINAL);
void TERMINAL_disable(TERMINAL_t *pTERMINAL);
void TERMINAL_change(TERMINAL_t *pTERMINAL_out, TERMINAL_t *pTERMINAL_in);

uint8_t TERMINAL_column(TERMINAL_t *pTERMINAL, const uint8_t field);

void TERMINAL_main_title(TERMINAL_t *pTERMINAL, char *string);
/*
extern const char* TERMINAL_MENU_PID_TEST[];
extern const char* TERMINAL_MENU_PID[];
extern const char* TERMINAL_SM_MENU[];
extern const char* TERMINAL_BASE_MENU[];
extern USART_t USART_2;

extern PID_t MOTOR_1_PID;
*/

extern DMA_t DMA_USART2_TX;
extern DMA_t DMA_USART2_TX;

void USART_terminal_info(TERMINAL_t *pTERMINAL, const uint8_t line, const uint8_t row_1, const uint8_t row_2);


uint8_t TERMINAL_get_terminal(TERMINAL_DISPLAY_t *pTERMINAL_DISPLAY);
uint8_t TERMINAL_set_terminal(TERMINAL_DISPLAY_t *pTERMINAL_DISPLAY, uint8_t terminal_number);


#endif /* TERMINAL_H_ */













