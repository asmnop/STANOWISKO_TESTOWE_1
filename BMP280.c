/*
 * BMP280.c
 *
 *  Created on: 14 maj 2020
 *      Author: M.Markowski - marecki
 */


#include "../../LIB/Inc/BMP280.h"

#include "..\..\LIB\Inc\i2c.h"
#include "software_timers.h"
#include "..\..\LIB\Inc\universal_data.h"
#include "..\..\LIB\Inc\universal_functions.h"

//	NAZWA OBSŁUGIWANEGO MODUŁU: GY-521

#define BMP280_TAB_LENGTH 26

const char BMP280_reg_136_name[] = "CALIB00";
const char BMP280_reg_137_name[] = "CALIB01";
const char BMP280_reg_138_name[] = "CALIB02";
const char BMP280_reg_139_name[] = "CALIB03";
const char BMP280_reg_140_name[] = "CALIB04";
const char BMP280_reg_141_name[] = "CALIB05";
const char BMP280_reg_142_name[] = "CALIB06";
const char BMP280_reg_143_name[] = "CALIB07";
const char BMP280_reg_144_name[] = "CALIB08";
const char BMP280_reg_145_name[] = "CALIB09";
const char BMP280_reg_146_name[] = "CALIB10";
const char BMP280_reg_147_name[] = "CALIB11";
const char BMP280_reg_148_name[] = "CALIB12";
const char BMP280_reg_149_name[] = "CALIB13";
const char BMP280_reg_150_name[] = "CALIB14";
const char BMP280_reg_151_name[] = "CALIB15";
const char BMP280_reg_152_name[] = "CALIB16";
const char BMP280_reg_153_name[] = "CALIB17";
const char BMP280_reg_154_name[] = "CALIB18";
const char BMP280_reg_155_name[] = "CALIB19";
const char BMP280_reg_156_name[] = "CALIB20";
const char BMP280_reg_157_name[] = "CALIB21";
const char BMP280_reg_158_name[] = "CALIB22";
const char BMP280_reg_159_name[] = "CALIB23";
const char BMP280_reg_160_name[] = "CALIB24";
const char BMP280_reg_161_name[] = "CALIB25";

const char BMP280_reg_208_name[] = "ID";
const char BMP280_reg_224_name[] = "RESET";
const char BMP280_reg_243_name[] = "STATUS";
const char BMP280_reg_244_name[] = "CTRL_MEAS";
const char BMP280_reg_245_name[] = "CONFIG";

const char BMP280_reg_247_name[] = "PRESS_MSB";
const char BMP280_reg_248_name[] = "PRESS_LSB";
const char BMP280_reg_249_name[] = "PRESS_XLSB";

const char BMP280_reg_250_name[] = "TEMP_MSB";
const char BMP280_reg_251_name[] = "TEMP_LSB";
const char BMP280_reg_252_name[] = "TEMP_XLSB";

const char* const BMP280_reg_control_ptr[5] =
{
		BMP280_reg_208_name,
		BMP280_reg_224_name,
		BMP280_reg_243_name,
		BMP280_reg_244_name,
		BMP280_reg_245_name
};

const char* const BMP280_reg_data_ptr[6] =
{
		BMP280_reg_247_name,
		BMP280_reg_248_name,
		BMP280_reg_249_name,
		BMP280_reg_250_name,
		BMP280_reg_251_name,
		BMP280_reg_252_name
};

const char* const BMP280_reg_calib_ptr[BMP280_TAB_LENGTH] =
{
		BMP280_reg_136_name,
		BMP280_reg_137_name,
		BMP280_reg_138_name,
		BMP280_reg_139_name,
		BMP280_reg_140_name,
		BMP280_reg_141_name,
		BMP280_reg_142_name,
		BMP280_reg_143_name,
		BMP280_reg_144_name,
		BMP280_reg_145_name,
		BMP280_reg_146_name,
		BMP280_reg_147_name,
		BMP280_reg_148_name,
		BMP280_reg_149_name,
		BMP280_reg_150_name,
		BMP280_reg_151_name,
		BMP280_reg_152_name,
		BMP280_reg_153_name,
		BMP280_reg_154_name,
		BMP280_reg_155_name,
		BMP280_reg_156_name,
		BMP280_reg_157_name,
		BMP280_reg_158_name,
		BMP280_reg_159_name,
		BMP280_reg_160_name,
		BMP280_reg_161_name
};

const uint8_t BMP280_reg_number[BMP280_TAB_LENGTH] =
{
		136,
		137,
		138,
		139,
		140,
		141,
		142,
		143,
		144,
		145,
		146,
		147,
		148,
		149,
		150,
		151,
		152,
		153,
		154,
		155,
		156,
		157,
		158,
		159,
		160,
		161
};




//	PARAMETRY KALIBRACYJNE:
typedef struct
{
	uint16_t dig_T1;
	int16_t dig_T2;
	int16_t dig_T3;
	uint16_t dig_P1;
	int16_t dig_P2;
	int16_t dig_P3;
	int16_t dig_P4;
	int16_t dig_P5;
	int16_t dig_P6;
	int16_t dig_P7;
	int16_t dig_P8;
	int16_t dig_P9;
	int32_t tfine;
}BMP280_calib_param_t;

typedef struct
{
	int32_t uncomp_temp;
	uint32_t uncomp_press;
}BMP280_uncomp_data_t;

BMP280_calib_param_t calib_param;
BMP280_uncomp_data_t uncomp_data;


//	##############################################################################################################################
void BMP280_init(BMP280_t *pBMP);
void BMP280_power_on_reset(BMP280_t *pBMP);
static void BMP280_compute_calib_param(BMP280_t *pBMP);

void BMP280_show_calib_reg(BMP280_t *pBMP);
void BMP280_show_control_reg(BMP280_t *pBMP);
void BMP280_show_data_reg(BMP280_t *pBMP);

static void BMP280_force_mode(BMP280_t *pBMP, const uint8_t os_t, const uint8_t os_p);
static void BMP280_get_uncomp_data(BMP280_t *pBMP);
static void BMP280_get_comp_temp(BMP280_t *pBMP, BMP280_uncomp_data_t *uncomp_data);
static void BMP280_get_comp_press(BMP280_t *pBMP, BMP280_uncomp_data_t *uncomp_data);
void BMP280_compute_data(BMP280_t *pBMP);


//	##############################################################################################################################
void BMP280_init(BMP280_t *pBMP)
{
	//	-procedura inicjalizacyjna czujnika ciśnienia i temperatury BOSCH BMP280,
	//	-każdorazowo po ponownym starcie uC należy wykonać reset układu,
	//	-po resecie potrzebne jest jednorazowe pobranie i obliczenie wartości danych z rejestrów kalibracyjnych,

	BMP280_power_on_reset(pBMP);
	BMP280_compute_calib_param(pBMP);
}

void BMP280_power_on_reset(BMP280_t *pBMP)
{
	//	-programowe wymuszenie resetu układu,
	//	-wymuszenie resetu poprzez wpisanie do rejestru 'BMP280_REG_RESET' wartości 0xB6,

	uint8_t data = BMP280_RESET_CODE;
	//I2C_write_byte(BMP280_ADDR, BMP280_REG_RESET, BMP280_RESET_CODE);	//	Wykonanie resetu programowo,
	I2C_write_bytes(pBMP->I2Cx, pBMP->address, BMP280_REG_RESET, &data, 1);

	timer_while = 20;
	while(I2C_read_byte(pBMP->I2Cx, pBMP->address, BMP280_REG_ID) != BMP280_CHIP_ID)	//	Czekaj tak długo jak długo bit w stanie wysokim,
	{
		if(timer_while == 0)
		{
			//ESP_LOGI(TAG, "ER 1");
			while(1);
		}
	}
	timer_while = 0;
}

void BMP280_compute_calib_param(BMP280_t *pBMP)
{
	//	-pobranie 24 bajtów danych z rejestrów kalibracyjnych,
	//	-rejestrów kalibracyjnych jest w sumie 26 ALE dwa najstarsze są zarezerwowane więc odczyt z nich jest pomijany,
	//	-zapis danych do tablicy,
	//	-obliczenie parametrów kalibracyjnych,
	//	-odczyt odbywa się dla rejestrów LSB oraz MSB,

	const uint8_t CALIB = 24;
	uint8_t arr_calib_data[CALIB];

	timer_while = 20;
	while(I2C_read_byte(pBMP->I2Cx, pBMP->address, BMP280_REG_STATUS) & BMP280_IM_UPDATE)
	{
		if(timer_while == 0)
		{
			//ESP_LOGI(TAG, "ER 2");
			while(1);
		}
	}
	timer_while = 0;

	//I2C_read_bytes(BMP280_ADDR, BMP280_REG_CALIB00, CALIB, arr_calib_data);	//	Odczyt danych z układu,
	I2C_read_bytes(pBMP->I2Cx, pBMP->address, BMP280_REG_CALIB00, arr_calib_data, CALIB);

	calib_param.dig_T1 = (uint16_t) (((uint16_t)arr_calib_data[1]<<8) | ((uint16_t)arr_calib_data[0]));		//	Obliczenie dig_T1,
	calib_param.dig_T2 = (int16_t) (((int16_t)arr_calib_data[3]<<8) | ((int16_t)arr_calib_data[2]));		//	Obliczenie dig_T2,
	calib_param.dig_T3 = (int16_t) (((int16_t)arr_calib_data[5]<<8) | ((int16_t)arr_calib_data[4]));		//	Obliczenie dig_T3,

	calib_param.dig_P1 = (uint16_t) (((uint16_t)arr_calib_data[7]<<8) | ((uint16_t)arr_calib_data[6]));		//	Obliczenie dig_P1,
	calib_param.dig_P2 = (int16_t) (((int16_t)arr_calib_data[9]<<8) | ((int16_t)arr_calib_data[8]));		//	Obliczenie dig_P2,
	calib_param.dig_P3 = (int16_t) (((int16_t)arr_calib_data[11]<<8) | ((int16_t)arr_calib_data[10]));		//	Obliczenie dig_P3,
	calib_param.dig_P4 = (int16_t) (((int16_t)arr_calib_data[13]<<8) | ((int16_t)arr_calib_data[12]));		//	Obliczenie dig_P4,
	calib_param.dig_P5 = (int16_t) (((int16_t)arr_calib_data[15]<<8) | ((int16_t)arr_calib_data[14]));		//	Obliczenie dig_P5,
	calib_param.dig_P6 = (int16_t) (((int16_t)arr_calib_data[17]<<8) | ((int16_t)arr_calib_data[16]));		//	Obliczenie dig_P6,
	calib_param.dig_P7 = (int16_t) (((int16_t)arr_calib_data[19]<<8) | ((int16_t)arr_calib_data[18]));		//	Obliczenie dig_P7,
	calib_param.dig_P8 = (int16_t) (((int16_t)arr_calib_data[21]<<8) | ((int16_t)arr_calib_data[20]));		//	Obliczenie dig_P8,
	calib_param.dig_P9 = (int16_t) (((int16_t)arr_calib_data[23]<<8) | ((int16_t)arr_calib_data[22]));		//	Obliczenie dig_P9,
}


//	##############################################################################################################################
void BMP280_show_calib_reg(BMP280_t *pBMP)
{
	//	-pobranie 26 bajtów danych kalibracyjnych,
	//	-zapis danych do tablicy,

	const uint8_t CALIB = 26;
	uint8_t arr_calib_data[CALIB];

	//I2C_read_bytes(BMP280_ADDR, BMP280_REG_CALIB00, CALIB, arr_calib_data);	//	Odczyt danych z układu,
	I2C_read_bytes(pBMP->I2Cx, pBMP->address, BMP280_REG_CALIB00, arr_calib_data, CALIB);

	//show_arr(arr_calib_data, CALIB, BMP280_reg_calib_ptr);					//	Przeglądanie danych,
	//LCD_show_array((char*)BMP280_reg_calib_ptr, arr_calib_data, CALIB);	//	Przeglądanie danych,
}

void BMP280_show_control_reg(BMP280_t *pBMP)
{
	//	-pobrane danych z rejestrów: Revision, Reset, Status i Control,

	const uint8_t ARR_DATA = 5;
	uint8_t arr_data[ARR_DATA];

	arr_data[0] = I2C_read_byte(pBMP->I2Cx, pBMP->address, BMP280_REG_ID);	//	Odczyt danych z układu,
	arr_data[1] = I2C_read_byte(pBMP->I2Cx, pBMP->address, BMP280_REG_RESET);	//	Odczyt danych z układu,
	//I2C_read_bytes(BMP280_ADDR, BMP280_REG_STATUS, 3, &arr_data[2]);	//	Odczyt danych z układu,
	I2C_read_bytes(pBMP->I2Cx, pBMP->address, BMP280_REG_STATUS, &arr_data[2], 3);

	//show_arr(arr_data, ARR_DATA, BMP280_reg_control_ptr);						//	Przeglądanie danych,
	//LCD_show_array((char*)&BMP280_reg_control_ptr[0], arr_data, ARR_DATA);	//	Przeglądanie danych,
}

void BMP280_show_data_reg(BMP280_t *pBMP)
{
	//	-pobranie 6 bajtów danych pomiarowych,
	//	-zapis danych do tablicy,

	const uint8_t ARR_DATA = 6;
	uint8_t arr_data[ARR_DATA];

	BMP280_force_mode(pBMP, BMP280_OS_X1, BMP280_OS_X1);						//	Wymuszenie jednorazowego pomiaru,
	//I2C_read_bytes(BMP280_ADDR, BMP280_REG_PRESS_MSB, ARR_DATA, arr_data);	//	Odczyt danych z układu,
	I2C_read_bytes(pBMP->I2Cx, pBMP->address, BMP280_REG_PRESS_MSB, arr_data, ARR_DATA);

	//show_arr(arr_data, ARR_DATA, BMP280_reg_data_ptr);						//	Przeglądanie danych,
	//LCD_show_array((char*)&BMP280_reg_data_ptr[0], arr_data, ARR_DATA);	//	Przeglądanie danych,
}

/*
void BMP280_show_data(void)
{
	//	-wyświetlenie obliczonych wartości temperatury i ciśnienia,
	//	-wartości przedstawione w postaci czytelnej dla użytkownika,

	uint8_t flag_exit = 0;		//	Flaga zezwolenia na wyjście z pętli,
	uint8_t first_enter = 1;

	do
	{
		if( flag_half || (first_enter == 1) )
		{
			first_enter = 0;
			flag_half = 0;
			//wait(WAIT_200MS);
			//BMP280_compute_data();	//	Obliczenie danych,

			//	WYŚWIETLENIE DANYCH:
			LCD_cls();
			LCD_temperature(comp_data.comp_temp);
			LCD_line_2();
			LCD_pressure(comp_data.comp_press/100);

			flag_exit = key_pressed_up(KEY_1, WAIT_1S, WAIT_1S, 0, 0);
		}

	}while(!flag_exit);

	end();
}
*/

//	##############################################################################################################################
static void BMP280_force_mode(BMP280_t *pBMP, const uint8_t os_t, const uint8_t os_p)
{
	//	-os_t --> OverSampling Temperature,
	//	-os_p --> OverSampling Pressure,
	//	-ustawienie układu do trybu pracy: forced mode,
	//	-ustawienie do pracy w tym trybie jest jednoznaczne z wykonaniem pomiaru czujnikiem,
	//	-zakończenie pomiaru jest sygnalizowane poprzez ustawienie w stan niski bitu 'measuring' w rejestrze 'status',

	//I2C_write_byte(BMP280_ADDR, BMP280_REG_CTRL_MEAS, (os_t<<5) | (os_p<<2) | BMP280_FORCED_MODE);	//	Ustawienie do trybu pracy forced,
	uint8_t data = (os_t<<5) | (os_p<<2) | BMP280_FORCED_MODE;
	I2C_write_bytes(pBMP->I2Cx, pBMP->address, BMP280_REG_CTRL_MEAS, &data, 1);

	timer_while = 1000;
	while(I2C_read_byte(pBMP->I2Cx, pBMP->address, BMP280_REG_STATUS) & BMP280_MEASURING)		//	Czekaj tak długo jak długo bit w stanie wysokim,
	{
		if(timer_while == 0)
		{
			//ESP_LOGI(TAG, "ER 3");
			while(1);
		}
	}
	timer_while = 0;
}

static void BMP280_get_uncomp_data(BMP280_t *pBMP)
{
	//	-obliczenie temperatury i ciśnienia nieskompensowanego,
	//	-dane są obliczane w jednej funkcji aby dwa razy nie dokonywać odczytu danych z czujnika,

	const uint8_t ARR_DATA = 6;
	uint8_t arr_data[ARR_DATA];

	//I2C_read_bytes(BMP280_ADDR, BMP280_REG_PRESS_MSB, ARR_DATA, arr_data);	//	Odczyt danych z układu,
	I2C_read_bytes(pBMP->I2Cx, pBMP->address, BMP280_REG_PRESS_MSB, arr_data, ARR_DATA);

	uncomp_data.uncomp_press = (int32_t) ((((uint32_t)arr_data[0])<<12) | (((uint32_t)arr_data[1])<<4) | (((uint32_t)arr_data[2])>>4));
	uncomp_data.uncomp_temp = (int32_t) ((((int32_t)arr_data[3])<<12) | (((int32_t)arr_data[4])<<4) | (((int32_t)arr_data[5])>>4));
}

static void BMP280_get_comp_temp(BMP280_t *pBMP, BMP280_uncomp_data_t *uncomp_data)
{
	//	-obliczenie wysokości temparatury po uwzględnieniu parametrów kalibracyjnych,

	int32_t var1, var2;

	var1 = ( ( ( (uncomp_data->uncomp_temp/8) - (((int32_t)calib_param.dig_T1) << 1)) ) * ( (int32_t)calib_param.dig_T2) ) / 2048;
	var2 = (((((uncomp_data->uncomp_temp / 16) - ((int32_t)calib_param.dig_T1)) *
		   ((uncomp_data->uncomp_temp / 16) - ((int32_t) calib_param.dig_T1))) / 4096) *
		((int32_t) calib_param.dig_T3)) / 16384;
	calib_param.tfine = var1 + var2;
	pBMP->comp_temp = (calib_param.tfine * 5 + 128) / 256;
}

static void BMP280_get_comp_press(BMP280_t *pBMP, BMP280_uncomp_data_t *uncomp_data)
{
	//	-obliczenie skompensowanego ciśnienia,
///*
	int32_t var1, var2;


    var1 = (((int32_t) calib_param.tfine) / 2) - (int32_t) 64000;
    var2 = (((var1 / 4) * (var1 / 4)) / 2048) * ((int32_t) calib_param.dig_P6);
    var2 = var2 + ((var1 * ((int32_t) calib_param.dig_P5)) * 2);
    var2 = (var2 / 4) + (((int32_t) calib_param.dig_P4) * 65536);
    var1 =
        (((calib_param.dig_P3 * (((var1 / 4) * (var1 / 4)) / 8192)) / 8) +
         ((((int32_t) calib_param.dig_P2) * var1) / 2)) / 262144;
    var1 = ((((32768 + var1)) * ((int32_t) calib_param.dig_P1)) / 32768);
    pBMP->comp_press = (uint32_t)(((int32_t)(1048576 - uncomp_data->uncomp_press) - (var2 / 4096)) * 3125);

	if (var1 != 0)
	{
		// Check for overflows against UINT32_MAX/2; if pres is left-shifted by 1
		if (pBMP->comp_press < 0x80000000)
		{
			pBMP->comp_press = (pBMP->comp_press << 1) / ((uint32_t) var1);
		}
		else
		{
			pBMP->comp_press = (pBMP->comp_press / (uint32_t) var1) * 2;
		}
		var1 = (((int32_t) calib_param.dig_P9) * ((int32_t) (((pBMP->comp_press / 8) * (pBMP->comp_press / 8)) / 8192))) /
			   4096;
		var2 = (((int32_t) (pBMP->comp_press / 4)) * ((int32_t) calib_param.dig_P8)) / 8192;
		pBMP->comp_press = (uint32_t) ((int32_t) pBMP->comp_press + ((var1 + var2 + calib_param.dig_P7) / 16));

	}
    else
    {
    	pBMP->comp_press = 0;
        //rslt = BMP280_E_32BIT_COMP_PRESS;
    }
//*/
/*
	int32_t var1, var2;
	  uint32_t p;
	  var1 = (((int32_t)calib_param.tfine)>>1) - (int32_t)64000;
	  var2 = (((var1>>2) * (var1>>2)) >> 11 ) * ((int32_t)calib_param.dig_P6);
	  var2 = var2 + ((var1*((int32_t)calib_param.dig_P5))<<1);
	  var2 = (var2>>2)+(((int32_t)calib_param.dig_P4)<<16);
	  var1 = ((((int32_t)calib_param.dig_P3 * (((var1>>2) * (var1>>2)) >> 13 )) >> 3) + ((((int32_t)calib_param.dig_P2) * var1)>>1))>>18;
	  var1 =((((32768+var1))*((int32_t)calib_param.dig_P1))>>15);
	  if (var1 == 0)
	  {
	    //return 0; // avoid exception caused by division by zero
	  }
	  p = (((uint32_t)(((int32_t)1048576)-uncomp_data->uncomp_press)-(var2>>12)))*3125;
	  if (p < 0x80000000)
	  {
	    p = (p << 1) / ((uint32_t)var1);
	  }
	  else
	  {
	    p = (p / (uint32_t)var1) * 2;
	  }
	  var1 = (((int32_t)calib_param.dig_P9) * ((int32_t)(((p>>3) * (p>>3))>>13)))>>12;
	  var2 = (((int32_t)(p>>2)) * ((int32_t)calib_param.dig_P8))>>13;
	  p = (uint32_t)((int32_t)p + ((var1 + var2 + calib_param.dig_P7) >> 4));
	  //return p;
	  comp_data.comp_press = p;
	  */
}

void BMP280_compute_data(BMP280_t *pBMP)
{
	//	-obliczenie danych wyjściowych,

	BMP280_force_mode(pBMP, BMP280_OS_X1, BMP280_OS_X1);	//	Wymuszenie jednorazowego pomiaru,
	BMP280_get_uncomp_data(pBMP);			//	Obliczenie temperatury i ciśnienia nieskompensowanego,
	BMP280_get_comp_temp(pBMP, &uncomp_data);
	BMP280_get_comp_press(pBMP, &uncomp_data);
}



/*

void BMP280_first_read(void)
{
	//	-pierwszy odczyt danych z czujnika BMP280_ADDR,
	//	-odczyt rejestrów od: 0xF7 do 0xFC,
	//	-zawartość pobrana do tablicy,

	//TWI_read_data(BMP280_ADDR, BMP280_REG_PRESS_MSB);
	LCD_clear_display();
	LCD_show_register(TWI_read_data(BMP280_ADDR, BMP280_REG_ID));

	wait(WAIT_2S);

	while(1)
	{
		TWI_write_data(BMP280_ADDR, BMP280_REG_CTRL_MEAS, 0b11100001);

		uint8_t temp[6];
		TWI_read_multiple_data(BMP280_ADDR, 0xF7, 6, temp);
		//uint8_t temp_H = TWI_read_data(BMP280_ADDR, BMP280_REG_TEMP_MSB);

		for(uint8_t i=0; i<6; i++)
		{
			LCD_clear_display();
			LCD_number(i, 10);
			LCD_line_2();
			LCD_show_register(temp[i]);
			wait(WAIT_1S);
		}

		uint16_t temperature = temp[3]*255 + temp[4];
		LCD_clear_display();
		LCD_number(temperature, 10);
		wait(WAIT_2S);
	}
}

*/












