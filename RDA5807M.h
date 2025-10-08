/*
 * RDA5807M.h
 *
 *  Created on: 15 lis 2023
 *      Author: asmnop
 */

#ifndef BUILD_IDE_ESP_IDF_COMPONENTS_ESP32_LIB_RDA5807M_H_
#define BUILD_IDE_ESP_IDF_COMPONENTS_ESP32_LIB_RDA5807M_H_


#include "universal_lib.h"

#include "../../LIB/Inc/i2c.h"
#include "RDA5807M_reg.h"

#define RDA5807M_DEV_SEQ		0b10	//	Adres układu, dostęp sekwencyjny z autoinkrementacją,
#define RDA5807M_DEV_RAN		0x11	//	Adres układu, dostęp losowy,


#define REG_00	0x00
#define REG_01	0x01
#define REG_02	0x02
#define REG_03	0x03
#define REG_04	0x04
#define REG_05	0x05
#define REG_06	0x06
#define REG_07	0x07
#define REG_08	0x08
#define REG_09	0x09
#define REG_0A	0x0A
#define REG_0B	0x0B
#define REG_0C	0x0C
#define REG_0D	0x0D
#define REG_0E	0x0E
#define REG_0F	0x0F


//	##############################################################################################################################
typedef union
{
	struct
	{
		//	0x02:
		enable_t enable:1;			//	Power Up Enable. 0 = Disabled; 1 = Enabled
		soft_reset_t soft_reset:1;	//	Soft reset. If 0, not reset; If 1, reset.
		new_method_t new_method:1;	//	New Demodulate Method Enable, can improve the receive sensitivity about 1dB.
		rds_en_t rds_en:1;			//	RDS/RBDS enable; If 1, rds/rbds enable,
		clk_mode_t clk_mode:3;		//	000=32.768kHz; 001=12Mhz; 101=24Mhz; 010=13Mhz; 110=26Mhz; 011=19.2Mhz; 111=38.4Mhz,
		skmode_t skmode:1;			//	0 = wrap at the upper or lower band limit and continue seeking; 1 = stop seeking at the upper or lower band limit,

		seek_t seek:1;				//	Seek: 0 = Disable stop seek; 1 = Enable;
		seekup_t seekup:1;			//	Seek Up: 0 = Seek down; 1 = Seek up;
		rclk_dim_t rclk_dim:1;		//	RCLK DIRECT INPUT MODE: 1 = RCLK clock use the directly input mode;
		rclk_ncm_t rclk_ncm:1;		//	RCLK NON-CALIBRATE MODE: 0 = RCLK clock is always supply; 1 = RCLK clock is not always supply when FM work;
		bass_t bass:1;				//	Bass Boost: 0 = Disabled; 1 = Bass boost enabled;
		mono_t mono:1;				//	Mono Select: 0 = Stereo; 1 = Force mono;
		dmute_t dmute:1;			//	Mute Disable: 0 = Mute; 1 = Normal operation;
		dhiz_t dhiz:1;				//	Audio Output High-Z Disable: 0 = High impedance; 1 = Normal operation;
	};
	uint16_t all;
}RDA5807M_REG_0x02;

//	##############################################################################################################################
typedef union
{
	struct
	{
		//	0x03:
		space_t space:2;				//	Channel Spacing: 00 = 100 kHz, 01 = 200 kHz, 10 = 50kHz, 11 = 25KHz
		band_t band:2;					//	Band Select: 00 = 87–108 MHz (US/Europe) 01 = 76–91 MHz (Japan) 10 = 76–108 MHz (world wide) 111 = 65 –76 MHz （East Europe）  or 50-65MHz
		tune_t tune:1;					//	Tune: 0 = Disable, 1 = Enable,
		direct_mode_t direct_mode:1;	//	Directly Control Mode, Only used when test.
		uint16_t chan:10;				//	Channel Select.
	};
	uint16_t all;
}RDA5807M_REG_0x03;

//	##############################################################################################################################
typedef union
{
	struct
	{
		//	0x04:
		uint8_t gpio1:2;
		uint8_t gpio2:2;
		uint8_t gpio3:2;
		uint8_t i2s_enable:1;
		uint8_t rsvd_1:1;
		afcd_t afcd:1;					//	AFC disable. If 0, afc work; If 1, afc disabled,
		softmute_en_t softmute_en:1;	//	If 1, softmute enable,
		uint8_t rds_fifo_clr:1;			//	Reserved,
		de_t de:1;						//	De-emphasis. 0 = 75 µs; 1 = 50 µs
		uint8_t rds_fifo_en:1;
		uint8_t rbds:1;
		uint8_t stcien:1;
		uint8_t rsvd_2:1;
	};
	uint16_t all;
}RDA5807M_REG_0x04;

//	##############################################################################################################################
typedef union
{
	struct
	{
		//	0x05:
		uint8_t volume:4;			//	DAC Gain Control Bits (Volume). 0000=min; 1111=max Volume scale is logarithmic. When 0000, output mute and output impedance is very large,
		uint8_t lna_icsel_bit:2;
		uint8_t lna_port_sel:2;
		uint8_t seekth:4;			//	Seek SNR threshold value
		uint8_t rsvd_1:1;
		uint8_t seek_mode:2;		//	Reserved,
		int_mode_t int_mode:1;		//	If 0, generate 5ms interrupt; If 1, interrupt last until read reg0CH action occurs.
	};
	uint16_t all;
}RDA5807M_REG_0x05;

//	##############################################################################################################################
typedef union
{
	struct
	{
		//	0x06:
		uint8_t rsvd_1:7;			//	Reserved,
		uint8_t rsvd_1_:7;			//	Reserved,
		open_mode_t open_mode:1;	//	Open reserved register mode. 11=open behind registers writing function others: only open behind registers reading function,
		uint8_t rsvd_2:1;			//	Reserved,
	};
	uint16_t all;
}RDA5807M_REG_0x06;

//	##############################################################################################################################
typedef union
{
	struct
	{
		//	0x07:
		freq_mode_t freq_mode:1;		//	If 1, then freq setting changed. Freq = 76000(or 87000) kHz + freq_direct (08H) kHz.
		softblend_en_t softblend_en:1;	//	If 1, Softblend enable
		uint8_t seek_th_old:6;			//	Seek threshold for old seek mode, Valid when Seek_Mode=001

		uint8_t rsvd_1:1;				//	Reserved,
		mode_65m_50m_t mode_65m_50m:1;	//	Valid when band[1:0] = 2’b11 (0x03H_bit<3:2>) 1 = 65~76 MHz; 0 = 50~76 MHz.
		uint8_t th_sofrblend:5;			//	Threshold for noise soft blend setting, unit 2dB,
		uint8_t rsvd_2:1;				//	Reserved,
	};
	uint16_t all;
}RDA5807M_REG_0x07;


typedef struct RDS_STRUCT
{
	uint16_t block_A;
	uint16_t block_B;
	uint16_t block_C;
	uint16_t block_D;
}RDS_t;


void RDA5807M_init(I2C_t *pI2C);

void RDA5807M_set_vol(I2C_t *pI2C, uint8_t vol);
void RDA5807M_set_vol_up(I2C_t *pI2C);
void RDA5807M_set_vol_down(I2C_t *pI2C);
uint8_t RDA5807M_get_vol(I2C_t *pI2C);

void RDA5807M_set_bass(I2C_t *pI2C);
void RDA5807M_reset_bass(I2C_t *pI2C);
uint8_t RDA5807M_get_bass(I2C_t *pI2C);

void RDA5807M_set_chan(I2C_t *pI2C, uint16_t chan);
void RDA5807M_set_chan_up(I2C_t *pI2C);
void RDA5807M_set_chan_down(I2C_t *pI2C);
uint16_t RDA5807M_get_chan(I2C_t *pI2C);
uint8_t RDA5807M_get_STC(I2C_t *pI2C);

void RDA5807M_set_reg(I2C_t *pI2C, const uint8_t reg);
uint16_t RDA5807M_get_reg(I2C_t *pI2C, const uint8_t reg);
void RDA5807M_show_reg(I2C_t *pI2C, const uint8_t reg, char* description);

void RDA5807M_set_mute(I2C_t *pI2C);
void RDA5807M_reset_mute(I2C_t *pI2C);
uint8_t RDA5807M_get_mute(I2C_t *pI2C);

uint16_t RD5807M_get_power_on(I2C_t *pI2C);

void RDA5807M_set_mem(I2C_t *pI2C);
void RDA5807M_set_mem_play(void);

void RDA5807M_get_mem(I2C_t *pI2C);
void RDA5807M_get_RDS(I2C_t *pI2C);







#endif /* BUILD_IDE_ESP_IDF_COMPONENTS_ESP32_LIB_RDA5807M_H_ */




