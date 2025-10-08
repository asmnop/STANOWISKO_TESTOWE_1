/*
 * DS3231.h
 *
 *  Created on: 17 maj 2024
 *      Author: asmnop
 */

#ifndef INC_DS3231_H_
#define INC_DS3231_H_


#include "universal_lib.h"

#include "..\..\LIB\Inc\i2c.h"


#define DS3231_DEV	0x68	//	Adres slave układu,

//	NAZWY REJESTRÓW:
#define DS3231_REG_SECONDS				0x00	//	0
#define DS3231_REG_MINUTES				0x01	//	1
#define DS3231_REG_HOURS				0x02	//	2
#define DS3231_REG_DAY					0x03	//	3
#define DS3231_REG_DATE					0x04	//	4
#define DS3231_REG_MONTH_CENTURY		0x05	//	5
#define DS3231_REG_YEAR					0x06	//	6
#define DS3231_REG_ALARM_1_SECONDS		0x07	//	7
#define DS3231_REG_ALARM_1_MINUTES		0x08	//	8
#define DS3231_REG_ALARM_1_HOURS		0x09	//	9
#define DS3231_REG_ALARM_1_DAY_DATE		0x0A	//	10
#define DS3231_REG_ALARM_2_MINUTES		0x0B	//	11
#define DS3231_REG_ALARM_2_HOURS		0x0C	//	12
#define DS3231_REG_ALARM_2_DAY_DATE		0x0D	//	13
#define DS3231_REG_CONTROL				0x0E	//	14
#define DS3231_REG_CONTROL_STATUS		0x0F	//	15
#define DS3231_REG_AGING_OFFSET			0x10	//	16
#define DS3231_REG_MSB_OF_TEMP			0x11	//	17
#define DS3231_REG_LSB_OF_TEMP			0x12	//	18

#define DS3231_RS_1HZ		0b00000000	//	Rate Select - 1 [Hz],
#define DS3231_RS_1KHZ		0b00001000	//	Rate Select - 1,024 [kHz],
#define DS3231_RS_4KHZ		0b00010000	//	Rate Select - 4,096 [kHz],
#define DS3231_RS_8KHZ		0b00011000	//	Rate Select - 8,192 [kHz],

#define DS3231_SQW_OUT		0b00000000	//	Na linii wyjściowej przebieg prostokątny,
#define DS3231_INT_OUT		0b00000100	//	Na linii wyjściowej sygnał alarmu,

#define DS3231_32KHZ_OUT_ON		0b00001000	//	Sygnał 32 [kHz] na linię wyjściową 32 kHz,
#define DS3231_32KHZ_OUT_OFF	0b00000000	//	Brak sygnału 32 [kHz] na linii wyjściowej,


//	STRUKTURA NA DANE O DACIE I CZASIE:
typedef struct
{
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t day;
	uint8_t date;
	uint8_t mon;
	uint8_t year;
	uint8_t on_off;
	uint8_t flag;
}ALARM_t;

typedef struct
{
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t day;
	uint8_t date;
	uint8_t mon;
	uint8_t year;
	uint8_t dot;
}DATE_TIME_t;

typedef struct
{
	int8_t temp_h;
	uint8_t temp_l;
}TEMP_t;

typedef struct
{
	I2C_t *I2Cx;
	uint8_t address;
	ALARM_t *alarm[5];
	DATE_TIME_t *date_time;
	TEMP_t *temp;
	uint8_t new_data_flag;
	uint8_t global_alarm_off_on;
}RTC_t;


extern DATE_TIME_t RTC_now;

extern TEMP_t DS_temp;

void DS3231_init(RTC_t *pRTC);
void DS3231_get_time(RTC_t *pRTC);
void DS3231_get_date(RTC_t *pRTC);
void DS3231_get_date_time(RTC_t *pRTC);
void DS3231_get_raw_reg(RTC_t *pRTC);
void DS3231_set_freq(const uint8_t rate, RTC_t *pRTC);
void DS3231_set_time_(const uint8_t hour, const uint8_t min, const uint8_t sec, RTC_t *pRTC);
void DS3231_set_date_(const uint8_t date, const uint8_t mon, const uint8_t year, RTC_t *pRTC);

void DS3231_set_time(RTC_t *pRTC);
void DS3231_set_date(RTC_t *pRTC);

void DS3231_get_temp(RTC_t *pRTC);
void DS3231_force_temp(RTC_t *pRTC);


void DS3231_set_new_data_flag(RTC_t *pRTC);
void DS3231_reset_new_data_flag(RTC_t *pRTC);
uint8_t DS3231_is_active_new_data_flag(RTC_t *pRTC);

void DS3231_set_dot(RTC_t *pRTC);
void DS3231_reset_dot(RTC_t *ptr);
uint8_t DS3231_get_dot(RTC_t *ptr);

void DS3231_set_global_alarm(RTC_t *pRTC);
void DS3231_reset_global_alarm(RTC_t *pRTC);
uint8_t DS3231_is_active_global_alarm(RTC_t *pRTC);

void DS3231_set_local_alarm(ALARM_t *alarm);
void DS3231_reset_local_alarm(ALARM_t *alarm);
uint8_t DS3231_is_active_local_alarm(ALARM_t *alarm);

uint8_t DS3231_is_set_local_alarm_flag(RTC_t *pRTC, ALARM_t *alarm);
void DS3231_reset_local_alarm_flag(ALARM_t *alarm);


#endif /* INC_DS3231_H_ */





