/*
 * BMP280.h
 *
 *  Created on: 14 maj 2020
 *      Author: M.Markowski - marecki
 */
#ifndef BMP280_H_
#define BMP280_H_

#include "universal_lib.h"

#include "..\..\LIB\Inc\i2c.h"

#define BMP280_ADDR			0b01110110	//	0x76
//#define BMP280_ADDR		0b01110111	//	0x77

typedef struct
{
	I2C_t *I2Cx;
	uint8_t address;
	int32_t comp_temp;
	uint32_t comp_press;
}BMP280_t;


#define BMP280_CHIP_ID		0x58
#define BMP280_RESET_CODE	0xB6	//	Kod wymuszenia resetu układu,


//	NAZWY REJESTRÓW:
#define BMP280_REG_CALIB00        0x88   //   136
#define BMP280_REG_CALIB01        0x89   //   137
#define BMP280_REG_CALIB02        0x8A   //   138
#define BMP280_REG_CALIB03        0x8B   //   139
#define BMP280_REG_CALIB04        0x8C   //   140
#define BMP280_REG_CALIB05        0x8D   //   141
#define BMP280_REG_CALIB06        0x8E   //   142
#define BMP280_REG_CALIB07        0x8F   //   143
#define BMP280_REG_CALIB08        0x90   //   144
#define BMP280_REG_CALIB09        0x91   //   145
#define BMP280_REG_CALIB10        0x92   //   146
#define BMP280_REG_CALIB11        0x93   //   147
#define BMP280_REG_CALIB12        0x94   //   148
#define BMP280_REG_CALIB13        0x95   //   149
#define BMP280_REG_CALIB14        0x96   //   150
#define BMP280_REG_CALIB15        0x97   //   151
#define BMP280_REG_CALIB16        0x98   //   152
#define BMP280_REG_CALIB17        0x99   //   153
#define BMP280_REG_CALIB18        0x9A   //   154
#define BMP280_REG_CALIB19        0x9B   //   155
#define BMP280_REG_CALIB20        0x9C   //   156
#define BMP280_REG_CALIB21        0x9D   //   157
#define BMP280_REG_CALIB22        0x9E   //   158
#define BMP280_REG_CALIB23        0x9F   //   159
#define BMP280_REG_CALIB24        0xA0   //   160
#define BMP280_REG_CALIB25        0xA1   //   161
#define BMP280_REG_ID             0xD0   //   208
#define BMP280_REG_RESET          0xE0   //   224
#define BMP280_REG_STATUS         0xF3   //   243
#define BMP280_REG_CTRL_MEAS      0xF4   //   244
#define BMP280_REG_CONFIG         0xF5   //   245
#define BMP280_REG_PRESS_MSB      0xF7   //   247
#define BMP280_REG_PRESS_LSB      0xF8   //   248
#define BMP280_REG_PRESS_XLSB     0xF9   //   249
#define BMP280_REG_TEMP_MSB       0xFA   //   250
#define BMP280_REG_TEMP_LSB       0xFB   //   251
#define BMP280_REG_TEMP_XLSB      0xFC   //   252

//	BITY REJESTRU CTRL_MEAS, nr 244:
#define BMP280_SLEEP_MODE         0x00
#define BMP280_FORCED_MODE        0x01
#define BMP280_NORMAL_MODE        0x11

#define BMP280_OS_SKIPPED         0x00
#define BMP280_OS_X1              0x01
#define BMP280_OS_X2              0x02
#define BMP280_OS_X4              0x03
#define BMP280_OS_X8              0x04
#define BMP280_OS_X16             0x05

#define BMP280_IM_UPDATE		0x01
#define BMP280_MEASURING		0x08


void BMP280_init(BMP280_t *pBMP);
void BMP280_show_calib_reg(BMP280_t *pBMP);
void BMP280_show_control_reg(BMP280_t *pBMP);
void BMP280_show_data_reg(BMP280_t *pBMP);
void BMP280_compute_data(BMP280_t *pBMP);








#endif /* BMP280_H_ */
