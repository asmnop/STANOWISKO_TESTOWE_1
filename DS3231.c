/*
 * DS3231.c
 *
 *  Created on: 17 maj 2024
 *      Author: asmnop
 */


#include "..\..\LIB\Inc\DS3231.h"

#include "..\..\LIB\Inc\i2c.h"
#include "..\..\LIB\Inc\universal_data.h"
#include "..\..\LIB\Inc\universal_functions.h"

#define DS3231_MEM_LENGTH	19

TEMP_t DS_temp;
DATE_TIME_t RTC_now;
DATE_TIME_t RTC_start =
{
	.sec = 40,
	.min = 30,
	.hour = 21,
	.day = 1,
	.date = 26,
	.mon = 8,
	.year = 24,
	.dot = 1
};




//	##############################################################################################################################
volatile uint8_t flag_DS3231_new = 0;	//	Flaga informująca o zajściu przerwania, pojawiły się nowe dane dotyczące czasu i daty,


const char DS3231_reg_0_name[] = "SECONDS";
const char DS3231_reg_1_name[] = "MINUTES";
const char DS3231_reg_2_name[] = "HOURS";
const char DS3231_reg_3_name[] = "DAY";
const char DS3231_reg_4_name[] = "DATE";
const char DS3231_reg_5_name[] = "MONTH/CENTURY";
const char DS3231_reg_6_name[] = "YEAR";
const char DS3231_reg_7_name[] = "ALARM 1 SECONDS";
const char DS3231_reg_8_name[] = "ALARM 1 MINUTES";
const char DS3231_reg_9_name[] = "ALARM 1 HOURS";
const char DS3231_reg_10_name[] = "ALARM 1 DAY/DATE";
const char DS3231_reg_11_name[] = "ALARM 2 MINUTES";
const char DS3231_reg_12_name[] = "ALARM 2 HOURS";
const char DS3231_reg_13_name[] = "ALARM 2 DAY/DATE";
const char DS3231_reg_14_name[] = "CONTROL";
const char DS3231_reg_15_name[] = "CONTROL/STATUS";
const char DS3231_reg_16_name[] = "AGING OFFSET";
const char DS3231_reg_17_name[] = "MSB OF TEMP";
const char DS3231_reg_18_name[] = "LSB OF TEMP";

const char* const DS3231_reg_ptr[DS3231_MEM_LENGTH] =
{
	&DS3231_reg_0_name[0],
	DS3231_reg_1_name,
	DS3231_reg_2_name,
	DS3231_reg_3_name,
	DS3231_reg_4_name,
	DS3231_reg_5_name,
	DS3231_reg_6_name,
	DS3231_reg_7_name,
	DS3231_reg_8_name,
	DS3231_reg_9_name,
	DS3231_reg_10_name,
	DS3231_reg_11_name,
	DS3231_reg_12_name,
	DS3231_reg_13_name,
	DS3231_reg_14_name,
	DS3231_reg_15_name,
	DS3231_reg_16_name,
	DS3231_reg_17_name,
	DS3231_reg_18_name
};

void DS3231_init(RTC_t *pRTC);

void DS3231_get_time(RTC_t *pRTC);
void DS3231_get_date(RTC_t *pRTC);
void DS3231_get_date_time(RTC_t *pRTC);

void DS3231_get_raw_reg(RTC_t *pRTC);
void DS3231_set_freq(const uint8_t rate, RTC_t *pRTC);
void DS3231_set_INT_SQW(const uint8_t option, RTC_t *pRTC);
void DS3231_set_enable_OSC(RTC_t *pRTC);
void DS3231_clr_OSF(RTC_t *pRTC);

void DS3231_get_temp(RTC_t *pRTC);
void DS3231_force_temp(RTC_t *pRTC);
uint8_t DS3231_is_set_local_alarm_flag(RTC_t *pRTC, ALARM_t *alarm);

void DS3231_set_time_(const uint8_t hour, const uint8_t min, const uint8_t sec, RTC_t *pRTC);
void DS3231_set_date_(const uint8_t date, const uint8_t mon, const uint8_t year, RTC_t *pRTC);

void DS3231_set_time(RTC_t *pRTC);
void DS3231_set_date(RTC_t *pRTC);

void DS3231_set_new_data_flag(RTC_t *pRTC);
void DS3231_reset_new_data_flag(RTC_t *pRTC);
uint8_t DS3231_is_active_new_data_flag(RTC_t *pRTC);

//	############################################################################
void DS3231_init(RTC_t *pRTC)
{
	//	-procedura inicjalizacyjna,
	//	-konfiguracja pinu wejściowego, na który zostanie podana fala prostokątna,
	//	-wybór częstotliwości fali prostokątnej,
	//	-podpięcie na wyjście linii INT_SQW fali prostokątnej,

	//	KONFIGURACJA PINU Z DOŁĄCZONYM SYGNAŁEM PROSTOKĄTNYM:
	//gpio_reset_pin(DS3231_INT_SQW);						//	Reset linii,
	//gpio_set_direction(DS3231_INT_SQW, GPIO_MODE_INPUT);	//	Wejście,
	//gpio_set_level(DS3231_INT_SQW, 1);					//	Stan wysoki,

	//	PONIŻSZE FUNKCJE ODPALAĆ JEDNOKROTNIE !!!
	//DS3231_set_time_(RTC_start.hour, RTC_start.min, RTC_start.sec);	//	Ustawienie wartości początkowych czasu,
	//DS3231_set_date_(RTC_start.date, RTC_start.mon, RTC_start.year);	//	Ustawienie wartości początkowych daty,

	//	PONIŻSZE FUNKCJE ODPALAĆ JEDNOKROTNIE !!!
	//DS3231_set_time(&RTC_start, pI2C);	//	Ustawienie wartości początkowych czasu,
	//DS3231_set_date(&RTC_start, pI2C);

	DS3231_set_freq(DS3231_RS_1HZ, pRTC);			//	Ustawienie częstotliwości na wyjściu INT/SQW,
	DS3231_set_INT_SQW(DS3231_SQW_OUT, pRTC);		//	Wybór trybu pracy pinu, 'DS3231_SQW_OUT' lub 'DS3231_INT_OUT',

	DS3231_set_enable_OSC(pRTC);
	DS3231_clr_OSF(pRTC);
}

void DS3231_get_time(RTC_t *pRTC)
{
	//	-pobranie wartości czasu z rejestrów układu,
	//	-zostają pobrane wartości sekund, minut i godzin,
	//	-konwersja danych na postać czytelną dla człowieka,
	//	-zapis skonwertowanych danych następuje do struktury,

	uint8_t RTC_time_arr[3];	//	Tablica na surowe informacje o czasie pobrane z układu,

	//HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c1, (DS3231_DEV<<1), DS3231_REG_SECONDS, 1, RTC_time_arr, 3, HAL_MAX_DELAY);
	uint8_t status = I2C_read_bytes(pRTC->I2Cx, pRTC->address, DS3231_REG_SECONDS, RTC_time_arr, 3);

	if(status == 0)
	{
		pRTC->date_time->sec = bcd_to_int(RTC_time_arr[0]);
		pRTC->date_time->min = bcd_to_int(RTC_time_arr[1]);
		pRTC->date_time->hour = bcd_to_int(RTC_time_arr[2]);

		//printf("%d:%d:%d\n", pRTC->date_time->hour, pRTC->date_time->min, pRTC->date_time->sec);
	}
}

void DS3231_get_date(RTC_t *pRTC)
{
	//	-pobranie wartości daty z rejestrów układu,
	//	-zostają pobrane wartości dnia miesiąca, miesiąca i roku,
	//	-konwersja danych na postać czytelną dla człowieka,
	//	-zapis skonwertowanych danych następuje do struktury,

	uint8_t RTC_date_arr[3];	//	Tablica na surowe informacje o dacie pobrane z układu,

	//HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c1, (DS3231_DEV<<1), DS3231_REG_DATE, 1, RTC_date_arr, 3, HAL_MAX_DELAY);
	uint8_t status = I2C_read_bytes(pRTC->I2Cx, pRTC->address, DS3231_REG_DATE, RTC_date_arr, 3);

	if(status == 0)
	{
		pRTC->date_time->date = bcd_to_int(RTC_date_arr[0]);	//	Standardowa konwersja dnia miesiąca,
		pRTC->date_time->mon = bcd_to_int(RTC_date_arr[1]);		//	Standardowa konwersja miesiąca,
		pRTC->date_time->year = bcd_to_int(RTC_date_arr[2]);	//	Standardowa konwersja roku,

		//printf("%d-%d-%d\n", ptr->year+2000, ptr->mon, ptr->date);
	}
}

void DS3231_get_date_time(RTC_t *pRTC)
{
	//	-pobranie wartości czasu i daty z rejestrów układu,
	//	-konwersja danych na postać czytelną dla człowieka,
	//	-zapis skonwertowanych danych do struktury,

	uint8_t RTC_date_time_arr[7];		//	Tablica na surowe dane pobrane z układu,

	//HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c1, (DS3231_DEV<<1), DS3231_REG_SECONDS, 1, RTC_date_time_arr, 7, HAL_MAX_DELAY);
	uint8_t status = I2C_read_bytes(pRTC->I2Cx, pRTC->address, DS3231_REG_SECONDS, RTC_date_time_arr, 7);

	if(status == 0)
	{
		pRTC->date_time->sec = bcd_to_int(RTC_date_time_arr[0]);		//	Standardowa konwersja sekund,
		pRTC->date_time->min = bcd_to_int(RTC_date_time_arr[1]);		//	Standardowa konwersja minut,
		pRTC->date_time->hour = bcd_to_int(RTC_date_time_arr[2]);		//	Zakładamy 24 godzinny format czasu,
		pRTC->date_time->day = bcd_to_int(RTC_date_time_arr[3]);		//	Standardowa konwersja dnia tygodnia,
		pRTC->date_time->date = bcd_to_int(RTC_date_time_arr[4]);		//	Standardowa konwersja dnia miesiąca,
		pRTC->date_time->mon = bcd_to_int(RTC_date_time_arr[5] & 0x7F);	//	Eliminacja sygnalizacji przekroczenia stulecia,
		pRTC->date_time->year = bcd_to_int(RTC_date_time_arr[6]);		//	Zakładamy, że żyjemy w XXI wieku :),

		//printf("%d:%d:%d\n", ptr->hour, ptr->min, ptr->sec);
		//printf("%d-%d-%d\n", ptr->year+2000, ptr->mon, ptr->date);
	}
}

void DS3231_get_raw_reg(RTC_t *pRTC)
{
	//	-pobranie i wyświetlenie wszystkich rejestrów z układu DS3231,
	//	-liczba rejestrów: 19,
	//	-adresy rejestrów z zakresu: 0 - 18,
	//	-wszystkie rejestry są ułożone w pamięci po kolei,

	uint8_t DS3231_raw_arr[DS3231_MEM_LENGTH];	//	Tablica na pobrane dane z rejestrów DS3231,

	//HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c1, (DS3231_DEV<<1), DS3231_REG_SECONDS, 1, DS3231_raw_arr, DS3231_MEM_LENGTH, HAL_MAX_DELAY);
	uint8_t status = I2C_read_bytes(pRTC->I2Cx, pRTC->address, DS3231_REG_SECONDS, DS3231_raw_arr, DS3231_MEM_LENGTH);

	if(status == 0)
	{
		printf("\n");
		printf("ZAWARTOSC REJESTROW UKLADU DS3231:\n");
		for(uint8_t i=0; i<DS3231_MEM_LENGTH; i++)
		{
			printf("Rejestr nr: %i, nazwa: %s\n", i, DS3231_reg_ptr[i]);
			//show_byte_raw(DS3231_raw_arr[i]);
		}
	}
	else if(status == 1)
	{
		//printf("Nie ma takiego urzadzenia - %d\n", address);
	}





/*
	ESP_LOGI(TAG, "ODEBRANE DANE W POSTACI INT:");
	for(uint8_t i=0; i<DS3231_MEM_LENGTH; i++)
	{
		uint8_t temp_h = 0;
		uint8_t temp_l = 0;
		temp_h = (DS3231_raw_arr[i] & 0b11110000) >> 4;
		temp_l = (DS3231_raw_arr[i] & 0b00001111);

		//data_arr[i] = bcd_to_int(data_arr[i]);
		ESP_LOGI(TAG, "Rejestr nr %i wynosi: %i -- %i", i, temp_h, temp_l);
		char temp[10] = { *(DS3231_reg_ptr[0]), *((DS3231_reg_ptr[0])+1), };
		//ESP_LOGI(TAG, "NAZWA %s", temp);
		ESP_LOGI(TAG, "NAZWA %s", DS3231_reg_ptr[i]);
	}
*/

}

void DS3231_set_time_(const uint8_t hour, const uint8_t min, const uint8_t sec, RTC_t *pRTC)
{
	uint8_t arr_RTC_time[3];

	arr_RTC_time[0] = dec_to_bcd(sec);
	arr_RTC_time[1] = dec_to_bcd(min);
	arr_RTC_time[2] = dec_to_bcd(hour);

	//HAL_StatusTypeDef status = HAL_I2C_Mem_Write(&hi2c1, (DS3231_DEV<<1), DS3231_REG_SECONDS, 1, arr_RTC_time, 3, HAL_MAX_DELAY);
	uint8_t status = I2C_write_bytes(pRTC->I2Cx, pRTC->address, DS3231_REG_SECONDS, arr_RTC_time, 3);

	if(status == 1)
	{
		printf("Nie mozna ustawic czasu!!!\n");
	}
	else if(status == 0)
	{
		printf("USTAWIONO NOWY CZAS!\n");
		printf("%d:%d:%d\n", hour, min, sec);
	}
}

void DS3231_set_date_(const uint8_t date, const uint8_t mon, const uint8_t year, RTC_t *pRTC)
{
	//	-ustawienie daty,

	uint8_t RTC_date_arr[3];

	RTC_date_arr[0] = dec_to_bcd(date);		//	Standardowa konwersja dnia miesiąca,
	RTC_date_arr[1] = dec_to_bcd(mon);		//	Standardowa konwersja miesiąca,
	RTC_date_arr[2] = dec_to_bcd(year);		//	Standardowa konwersja roku,

	//HAL_StatusTypeDef status = HAL_I2C_Mem_Write(&hi2c1, (DS3231_DEV<<1), DS3231_REG_DATE, 1, RTC_date_arr, 3, HAL_MAX_DELAY);
	uint8_t status = I2C_write_bytes(pRTC->I2Cx, pRTC->address, DS3231_REG_DATE, RTC_date_arr, 3);

	if(status == 1)
	{
		printf("Nie mozna ustawic daty!!!\n");
	}
	else if(status == 0)
	{
		printf("USTAWIONO NOWA DATE!\n");
		printf("%d-%d-%d\n", year+2000, mon, date);
	}
}

void DS3231_set_time(RTC_t *pRTC)
{
	uint8_t arr_RTC_time[3];

	arr_RTC_time[0] = dec_to_bcd(pRTC->date_time->sec);
	arr_RTC_time[1] = dec_to_bcd(pRTC->date_time->min);
	arr_RTC_time[2] = dec_to_bcd(pRTC->date_time->hour);

	//HAL_StatusTypeDef status = HAL_I2C_Mem_Write(&hi2c1, (DS3231_DEV<<1), DS3231_REG_SECONDS, 1, arr_RTC_time, 3, HAL_MAX_DELAY);
	uint8_t status = I2C_write_bytes(pRTC->I2Cx, pRTC->address, DS3231_REG_SECONDS, arr_RTC_time, 3);

	if(status == 1)
	{
		printf("Nie mozna ustawic czasu!!!\n");
	}
	else if(status == 0)
	{
		printf("USTAWIONO NOWY CZAS!\n");
		//printf("%d:%d:%02d\n", ptr->hour, ptr->min, ptr->sec);
	}
}

void DS3231_set_date(RTC_t *pRTC)
{
	//	-ustawienie daty,

	uint8_t RTC_date_arr[3];

	RTC_date_arr[0] = dec_to_bcd(pRTC->date_time->date);	//	Standardowa konwersja dnia miesiąca,
	RTC_date_arr[1] = dec_to_bcd(pRTC->date_time->mon);		//	Standardowa konwersja miesiąca,
	RTC_date_arr[2] = dec_to_bcd(pRTC->date_time->year);	//	Standardowa konwersja roku,

	//HAL_StatusTypeDef status = HAL_I2C_Mem_Write(&hi2c1, (DS3231_DEV<<1), DS3231_REG_DATE, 1, RTC_date_arr, 3, HAL_MAX_DELAY);
	uint8_t status = I2C_write_bytes(pRTC->I2Cx, pRTC->address, DS3231_REG_DATE, RTC_date_arr, 3);

	if(status == 1)
	{
		printf("Nie mozna ustawic daty!!!\n");
	}
	else if(status == 0)
	{
		printf("USTAWIONO NOWA DATE!\n");
		//printf("%d-%d-%d\n", ptr->year+2000, ptr->mon, ptr->date);
	}
}


void DS3231_set_freq(const uint8_t rate, RTC_t *pRTC)
{
	//	-wybór częstotliwości sygnału prostokątnego na wyjściu INT/SQW,
	//	-podana funkcja nie uaktywnia wyjścia sygnału na linie INT/SQW,
	//	-po pierwszym podaniu zasilania częstotliwość wynosi 8192 [kHz],
	//	-aktywacja wyjścia sygnału na linię odbywa się za pomocą funkcji ...,

	uint8_t data = 0;	//	Zmienna na pobrane dane z rejestru,

	//HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c1, (DS3231_DEV<<1), DS3231_REG_CONTROL, 1, &data, 1, HAL_MAX_DELAY);
	I2C_read_bytes(pRTC->I2Cx, pRTC->address, DS3231_REG_CONTROL, &data, 1);
	//data = I2C_read_byte(DS3231_DEV, DS3231_REG_CONTROL);	//	Odczyt danych z rejestru,

	data = (data & 0b11100111) | rate;
	//IIC_set_byte(&DS3231_LINK, DS3231_REG_CONTROL, data);
	I2C_write_bytes(pRTC->I2Cx, pRTC->address, DS3231_REG_CONTROL, &data, 1);

	//status = HAL_I2C_Mem_Write(&hi2c1, (DS3231_DEV<<1), DS3231_REG_CONTROL, 1, &data, 1, HAL_MAX_DELAY);
	//status = HAL_I2C_Master_Transmit(&hi2c1, (DS3231_DEV<<1), &data, 1, 1000);
}

void DS3231_set_INT_SQW(const uint8_t option, RTC_t *pRTC)
{
	//	-ustawienie sygnału generowanego na linii INT_SQW,
	//	-mamy do wyboru dwie opcje: 'DS3231_SQW_OUT' lub 'DS3231_INT_OUT',
	//	-na linii sygnał prostokątny o wybranej f,
	//	-na linii sygnał informujący o zajściu sgodnego porównania z rejestrem alarmu,

	uint8_t data = 0;	//	Zmienna na pobrane dane z rejestru,

	//HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c1, (DS3231_DEV<<1), DS3231_REG_CONTROL, 1, &data, 1, HAL_MAX_DELAY);
	uint8_t status = I2C_read_bytes(pRTC->I2Cx, pRTC->address, DS3231_REG_CONTROL, &data, 1);

	if(status == 0)
	{
		data = (data & 0b11111011) | option;					//	Wpisanie danych dotyczących wyboru trybu pracy,
		//status = HAL_I2C_Mem_Write(&hi2c1, (DS3231_DEV<<1), DS3231_REG_CONTROL, 1, &data, 1, HAL_MAX_DELAY);
		status = I2C_write_bytes(pRTC->I2Cx, pRTC->address, DS3231_REG_CONTROL, &data, 1);
	}
}

void DS3231_set_enable_OSC(RTC_t *pRTC)
{
	uint8_t data = 0;	//	Zmienna na pobrane dane z rejestru,

	//HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c1, (DS3231_DEV<<1), DS3231_REG_CONTROL, 1, &data, 1, HAL_MAX_DELAY);
	I2C_read_bytes(pRTC->I2Cx, pRTC->address, DS3231_REG_CONTROL, &data, 1);
	//data = I2C_read_byte(DS3231_DEV, DS3231_REG_CONTROL);	//	Odczyt danych z rejestru,
	data = (data & 0b01111111);						//	Wpisanie danych dotyczących częstotliwości,
	//status = HAL_I2C_Mem_Write(&hi2c1, (DS3231_DEV<<1), DS3231_REG_CONTROL, 1, &data, 1, HAL_MAX_DELAY);
	I2C_write_bytes(pRTC->I2Cx, pRTC->address, DS3231_REG_CONTROL, &data, 1);
	//I2C_write_byte(DS3231_DEV, DS3231_REG_CONTROL, data);	//	Zapis danych do rejestru,

}

void DS3231_clr_OSF(RTC_t *pRTC)
{
	uint8_t data = 0;	//	Zmienna na pobrane dane z rejestru,

	//HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c1, (DS3231_DEV<<1), DS3231_REG_CONTROL_STATUS, 1, &data, 1, HAL_MAX_DELAY);
	I2C_read_bytes(pRTC->I2Cx, pRTC->address, DS3231_REG_CONTROL, &data, 1);

	data = (data & 0b01111111);						//	Wpisanie danych dotyczących częstotliwości,
	//status = HAL_I2C_Mem_Write(&hi2c1, (DS3231_DEV<<1), DS3231_REG_CONTROL_STATUS, 1, &data, 1, HAL_MAX_DELAY);
	I2C_write_bytes(pRTC->I2Cx, pRTC->address, DS3231_REG_CONTROL, &data, 1);
}


//	####################################################################################################################
void DS3231_get_temp(RTC_t *pRTC)
{
	//	-pobranie wartości temperatury z układu,
	//	-dane temperatury są kodowane w systemie uzupełnienie do dwóch,
	//	-rejestry temperatury są uaktualniane po każdej konwersji zadanej przez użytkownika i co 64 sekundy,
	//	-wywołanie konwersji przez użytkownika nie wpływa na konwersję co 64 sekundy,

	uint8_t arr_temp[2];	//	Tablica na surowe informacje o temperaturze pobrane z układu,

	//HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c1, (DS3231_DEV<<1), DS3231_REG_MSB_OF_TEMP, 1, arr_temp, 2, HAL_MAX_DELAY);
	uint8_t status = I2C_read_bytes(pRTC->I2Cx, pRTC->address, DS3231_REG_MSB_OF_TEMP, arr_temp, 2);

	if(status == 0)
	{
		pRTC->temp->temp_h = arr_temp[0];
		pRTC->temp->temp_l = arr_temp[1] >> 6;
	}

/*
	uint8_t temp_l = 0;		//	Zmienna do której zapisujemy wartość temperatury po przecinku,

	if(ptr->temp_l == 0b00000001)
		temp_l = 25;
	else if(ptr->temp_l == 0b00000010)
		temp_l = 50;
	else
		temp_l = 75;

	ESP_LOGI(TAG, "Temperatura ukladu: %i.%i ['C]", ptr->temp_h, temp_l);
*/
}

void DS3231_force_temp(RTC_t *pRTC)
{
	//	-wymuszenie konwersji temperatury,
	//	-użytkownik powinien sprawdzić status bitu BSY, zanim wymusi konwersję,

	uint8_t data = 0;	//	Zmienna na pobrane dane z rejestru,

	//HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c1, (DS3231_DEV<<1), DS3231_REG_CONTROL, 1, &data, 1, HAL_MAX_DELAY);
	uint8_t status = I2C_read_bytes(pRTC->I2Cx, pRTC->address, DS3231_REG_CONTROL, &data, 1);

	if(status == 0)
	{
		data = (data | 0b00100000);
		//IIC_set_byte(&DS3231_LINK, DS3231_REG_CONTROL, data);
		status = I2C_write_bytes(pRTC->I2Cx, pRTC->address, DS3231_REG_CONTROL, &data, 1);
	}
}

void DS3231_get_alarm_(RTC_t *pRTC, const uint8_t hour, const uint8_t min, const uint8_t sec)
{
	//	-sprawdzenie czy wartość godziny, minuty i sekundy pobrane z RTC
	//	są zgodne z naszą godziną, minutą i sekundą poszukiwaną,

	if((pRTC->date_time->hour == hour) && (pRTC->date_time->min == min) && (pRTC->date_time->sec == sec))
	{
		pRTC->global_alarm_off_on = 1;
	}
}



//	####################################################################################################################
void DS3231_set_new_data_flag(RTC_t *pRTC)
{
	pRTC->new_data_flag = 1;
}

void DS3231_reset_new_data_flag(RTC_t *pRTC)
{
	pRTC->new_data_flag = 0;
}

uint8_t DS3231_is_active_new_data_flag(RTC_t *pRTC)
{
	return pRTC->new_data_flag;
}


void DS3231_set_dot(RTC_t *pRTC)
{
	pRTC->date_time->dot = 1;
}

void DS3231_reset_dot(RTC_t *pRTC)
{
	pRTC->date_time->dot = 0;
}

uint8_t DS3231_get_dot(RTC_t *pRTC)
{
	return pRTC->date_time->dot;
}


void DS3231_set_global_alarm(RTC_t *pRTC)
{
	pRTC->global_alarm_off_on = 1;
}

void DS3231_reset_global_alarm(RTC_t *pRTC)
{
	pRTC->global_alarm_off_on = 0;
}

uint8_t DS3231_is_active_global_alarm(RTC_t *pRTC)
{
	return pRTC->global_alarm_off_on;
}


void DS3231_set_local_alarm(ALARM_t *alarm)
{
	alarm->on_off = 1;
}

void DS3231_reset_local_alarm(ALARM_t *alarm)
{
	alarm->on_off = 0;
}

uint8_t DS3231_is_active_local_alarm(ALARM_t *alarm)
{
	return alarm->on_off;
}



uint8_t DS3231_is_set_local_alarm_flag(RTC_t *pRTC, ALARM_t *alarm)
{
	//	-sprawdzenie czy wartość godziny, minuty i sekundy pobrane z RTC
	//	są zgodne z naszą godziną, minutą i sekundą poszukiwaną,

	if((pRTC->date_time->hour == alarm->hour) && (pRTC->date_time->min == alarm->min) && (pRTC->date_time->sec == alarm->sec))
	{
		alarm->flag = 1;
		return 1;
	}

	return 0;
}

void DS3231_reset_local_alarm_flag(ALARM_t *alarm)
{
	alarm->flag = 0;
}






