/*
 * usart.h
 *
 *  Created on: 15 wrz 2024
 *      Author: asmnop
 */

#ifndef USART_H_
#define USART_H_


#include "universal_lib.h"


typedef struct
{
	USART_TypeDef *pUSARTx;	//	Wybór jednostki USART,
	uint32_t clock_source;	//	Wybór zegara do peryferium USART,
	uint32_t periphs;		//	Włączenie zegara peryferyjnego APB1, Enable clock,
	uint32_t data_width;	//	Długość ramki danych,
	uint32_t parity;		//	Bity parzystości,
	uint32_t stop_bits;		//	Stop bity,
	uint32_t over_sampling;
	uint32_t baud_rate;		//	Prędkość komunikacji: 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600,
	volatile uint8_t flag;	//	Flaga nowo odebranych danych,
	char *ptr;				//	Wskaźnik na znak do wysłania w trybie przerwaniowym,
	uint8_t buf;			//	Wielkość buforu na dane odebrane,
	char RX_data[10];		//	Bufor na dane odebrane,
}USART_t;


void USART_init(USART_t *pUSARTx);
void USART_start(const USART_t *pUSARTx);
void USART_stop(const USART_t *pUSARTx);
void USART_IT_conf(const USART_t *pUSARTx);

void USART_RX_new_data(USART_t *pUSART, const uint8_t data);
uint8_t USART_get_data(const USART_t *pUSART);
uint8_t USART_check_command(const USART_t *pUSART, const char command[], const uint8_t length);
uint8_t USART_check_string(USART_t *pUSART, const char command[], const uint8_t length);

void USART_set_flag_new_data(USART_t *pUSART);
void USART_reset_flag_new_data(USART_t *pUSART);
uint8_t USART_get_flag_new_data(const USART_t *pUSART);

void USART_TX_char(const USART_t *pUSART, const char data);
void USART_TX_string(const USART_t *pUSART, const char *string);
void USART_TX_number(const USART_t *pUSART, const int16_t number);
void USART_TX_unumber(const USART_t *pUSART, const uint16_t number);

void USART_TX_char_IT(const USART_t *pUSART, const char data);
void USART_TX_string_IT(USART_t *pUSART, char *string);
void USART_TX_number_IT(USART_t *pUSART, const int16_t number);

void USART_change_ptr(USART_t *pUSART);
void USART_control_panel(USART_t *pUSART, void (*foo_1)(void), void (*foo_2)(void), void (*foo_3)(void), void (*foo_4)(void));

void USART3_init(uint32_t baud_rate);
void USART3_start(void);


#endif /* USART_H_ */









