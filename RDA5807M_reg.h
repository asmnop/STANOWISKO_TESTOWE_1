/*
 * RDA5807M_reg.h
 *
 *  Created on: 16 lis 2023
 *      Author: asmnop
 */


#ifndef RDA5807M_REG_H_
#define RDA5807M_REG_H_


//	ROZPOZNANE STACJE RADIOWE:
//	 90.0 [MHz], chan =  30, --> Radio Złote Przeboje,
//	 92.7 [MHz], chan =  57,
//	 96.1 [MHz], chan =  91, --> RMF/FM
//	103.3 [MHz], chan = 163,
//	105.3 [MHz], chan = 183,


typedef enum
{
    RADIO_ZLOTE_PRZEBOJE = 30,
	RMF_FM = 91,
}RADIO_STATIONS_t;



//	######################################################### 0x02 H #############################################################
typedef enum
{
    HIGH_IMPEDANCE,
	NORMAL_OPERATION,
}dhiz_t;

typedef enum
{
    MUTE_ON,
	MUTE_OFF,
}dmute_t;

typedef enum
{
    STEREO,
	MONO,
}mono_t;

typedef enum
{
    BASS_OFF,
	BASS_ON,
}bass_t;

typedef enum
{
    CLK_ALWAYS_SUPPLY,
	CLK_NOT_ALWAYS_SUPPLY,
}rclk_ncm_t;

typedef enum
{
	DIRECT_INPUT_MODE_OFF,
	DIRECT_INPUT_MODE_ON,
}rclk_dim_t;

typedef enum
{
    SEEK_DOWN,
	SEEK_UP,
}seekup_t;

typedef enum
{
    STOP_SEEK,
	ENABLE_SEEK,
}seek_t;


//	######################################################### 0x02 L #############################################################
typedef enum
{
    SKMODE_WRAP,
	SKMODE_STOP,
}skmode_t;

typedef enum
{
    CLK_32_768,
	CLK_12,
	CLK_24,
	CLK_13,
	CLK_26,
	CLK_19,
	CLK_38,
}clk_mode_t;

typedef enum
{
    RDS_OFF,
	RDS_ON,
}rds_en_t;

typedef enum
{
    NEW_METHOD_OFF,
	NEW_METHOD_ON,
}new_method_t;

typedef enum
{
    RESET_OFF,
	RESET_ON,
}soft_reset_t;

typedef enum
{
    POWER_OFF,
	POWER_ON,
}enable_t;


//	######################################################### 0x03 L #############################################################
typedef enum
{
    DIRECTLY_CONTROL_MODE,
}direct_mode_t;

typedef enum
{
	TUNE_DISABLE,
	TUNE_ENABLE,
}tune_t;

typedef enum
{
	BAND_87_108,
	BAND_76_91,
	BAND_76_108,
	BAND_65_76,
}band_t;

typedef enum
{
	SPACE_100_kHz,
	SPACE_200_kHz,
	SPACE_50_kHz,
	SPACE_25_kHz,
}space_t;


//	######################################################### 0x04 H #############################################################
typedef enum
{
	DE_EMPHASIS_75_US,
	DE_EMPHASIS_50_US,
}de_t;

typedef enum
{
	SOFTMUTE_OFF,
	SOFTMUTE_ON,
}softmute_en_t;

typedef enum
{
	AFC_ON,
	AFC_OFF,
}afcd_t;


//	######################################################### 0x05 H #############################################################
typedef enum
{
	INT_TIME_5_MS,
	INT_LAST,
}int_mode_t;


//	######################################################### 0x06 H #############################################################
typedef enum
{
	OPEN_MODE_OFF,
	OPEN_MODE_ON,
}open_mode_t;


//	######################################################### 0x07 H #############################################################
typedef enum
{
	MODE_50_76,
	MODE_65_76,
}mode_65m_50m_t;


//	######################################################### 0x07 L #############################################################
typedef enum
{
	SOFTBLEND_OFF,
	SOFTBLEND_ON,
}softblend_en_t;

typedef enum
{
	FREQ_MODE_OFF,
	FREQ_MODE_ON,
}freq_mode_t;


#endif /* RDA5807M_REG_H_ */





