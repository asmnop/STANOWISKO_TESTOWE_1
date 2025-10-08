/*
 * RDA5807M.c
 *
 *  Created on: 15 lis 2023
 *      Author: asmnop
 */


#include "../../LIB/Inc/RDA5807M.h"

#include "../../LIB/Inc/VT100.h"
#include "../../LIB/Inc/universal_data.h"
#include "../../LIB/Inc/universal_functions.h"


#define RDA5807M_MEM_LENGTH_WRITE		6
#define RDA5807M_MEM_LENGTH_READ		6


//	 90.0 [MHz], chan =  30, --> Radio Złote Przeboje,
//	 92.7 [MHz], chan =  57,
//	 96.1 [MHz], chan =  91, --> RMF/FM
//	103.3 [MHz], chan = 163,
//	105.3 [MHz], chan = 183,

/*
#define A	0
#define B	1
#define C	2
#define D	3
uint16_t block[4] = {0, 0, 0, 0};
*/

RDS_t RDS =
{
	.block_A = 0,
	.block_B = 0,
	.block_C = 0,
	.block_D = 0
};


const uint16_t radio_stations[6] = {30, 57, 91, 163, 183};	//	Znalezione stacje radiowe,

const char RDA5807M_reg_0x00_name[] = "CHIP ID";		//	0x00,
const char RDA5807M_reg_0x01_name[] = "REZ 0x01";		//	0x01,
const char RDA5807M_reg_0x02_name[] = "USTAWIENIA 1";	//	0x02,
const char RDA5807M_reg_0x03_name[] = "STROJENIE";		//	0x03,
const char RDA5807M_reg_0x04_name[] = "USTAWIENIA 3";	//	0x04,
const char RDA5807M_reg_0x05_name[] = "GLOSNOSC";		//	0x05,
const char RDA5807M_reg_0x06_name[] = "USTAWIENIA 5";	//	0x06,
const char RDA5807M_reg_0x07_name[] = "USTAWIENIA 6";	//	0x07,
const char RDA5807M_reg_0x08_name[] = "REZ 0x08";		//	0x08,
const char RDA5807M_reg_0x09_name[] = "REZ 0x09";		//	0x09,
const char RDA5807M_reg_0x0A_name[] = "RDS DANE 1";		//	0x0A,
const char RDA5807M_reg_0x0B_name[] = "RDS DANE 2";		//	0x0B,
const char RDA5807M_reg_0x0C_name[] = "RDSA - BLOCK A";	//	0x0C,
const char RDA5807M_reg_0x0D_name[] = "RDSB - BLOCK B";	//	0x0D,
const char RDA5807M_reg_0x0E_name[] = "RDSC - BLOCK C";	//	0x0E,
const char RDA5807M_reg_0x0F_name[] = "RDSD - BLOCK D";	//	0x0F,

const char* const RDA5807M_reg_ptr[16] =
{
	&RDA5807M_reg_0x00_name[0],
	RDA5807M_reg_0x01_name,
	RDA5807M_reg_0x02_name,
	RDA5807M_reg_0x03_name,
	RDA5807M_reg_0x04_name,
	RDA5807M_reg_0x05_name,
	RDA5807M_reg_0x06_name,
	RDA5807M_reg_0x07_name,
	RDA5807M_reg_0x08_name,
	RDA5807M_reg_0x09_name,
	RDA5807M_reg_0x0A_name,
	RDA5807M_reg_0x0B_name,
	RDA5807M_reg_0x0C_name,
	RDA5807M_reg_0x0D_name,
	RDA5807M_reg_0x0E_name,
	RDA5807M_reg_0x0F_name,
};

//	##############################################################################################################################
const char RDA5807M_reg_0x0A_15_0[] = "0 - Brak nowych danych RDS";			//	0x00,
const char RDA5807M_reg_0x0A_15_1[] = "1 - Nowe dane RDS";					//	0x00,
const char RDA5807M_reg_0x0A_14_0[] = "0 - Szukanie nie zakonczone";		//	0x00,
const char RDA5807M_reg_0x0A_14_1[] = "1 - Szukanie zakonczone";			//	0x00,
const char RDA5807M_reg_0x0A_13_0[] = "0 - Poszukiwanie z sukcesem";		//	0x00,
const char RDA5807M_reg_0x0A_13_1[] = "1 - Szukanie zakończone sukcesem";	//	0x00,
const char RDA5807M_reg_0x0A_12_0[] = "0 - RDS niezsynronizowany";			//	0x00,
const char RDA5807M_reg_0x0A_12_1[] = "1 - Dekoder RDS zsynchronizowany";	//	0x00,
const char RDA5807M_reg_0x0A_11_0[] = "0 - Blok E nie zostal znaleziony";	//	0x00,
const char RDA5807M_reg_0x0A_11_1[] = "1 - Blok E zostal znaleziony";		//	0x00,
const char RDA5807M_reg_0x0A_10_0[] = "0 - Mono";							//	0x00,
const char RDA5807M_reg_0x0A_10_1[] = "1 - Stereo";							//	0x00,


const char* const RDA5807M_reg_0x0A_ptr[12] =
{
	RDA5807M_reg_0x0A_15_0,
	RDA5807M_reg_0x0A_15_1,
	RDA5807M_reg_0x0A_14_0,
	RDA5807M_reg_0x0A_14_1,
	RDA5807M_reg_0x0A_13_0,
	RDA5807M_reg_0x0A_13_1,
	RDA5807M_reg_0x0A_12_0,
	RDA5807M_reg_0x0A_12_1,
	RDA5807M_reg_0x0A_11_0,
	RDA5807M_reg_0x0A_11_1,
	RDA5807M_reg_0x0A_10_0,
	RDA5807M_reg_0x0A_10_1,
};



//	##############################################################################################################################
const char RDA5807M_reg_0x0B_15[] = "RSSI";								//	0x00,
const char RDA5807M_reg_0x0B_8_0[] = "Obecny kanal nie jest stacja";	//	0x00,
const char RDA5807M_reg_0x0B_8_1[] = "Obecny kanal jest stacja";		//	0x00,
const char RDA5807M_reg_0x0B_7_0[] = "Radio FM nie jest gotowe";		//	0x00,
const char RDA5807M_reg_0x0B_7_1[] = "Radio FM jest gotowe";			//	0x00,


const char RDA5807M_reg_0x02_15[] = "DHIZ - Wyjscie audio: ";
const char RDA5807M_reg_0x02_14[] = "DMUTE - Mute disable: ";
const char RDA5807M_reg_0x02_13[] = "MONO - Mute disable: ";

const char RDA5807M_reg_0x02_15_0[] = "0 - wysoka impedancja";
const char RDA5807M_reg_0x02_15_1[] = "1 - normalna operacja";

RDA5807M_REG_0x02 reg2 =
{
	//	0x02 HIGH BYTE:
	.dhiz = NORMAL_OPERATION,			//	Audio Output High-Z Disable. 0 = High impedance; 1 = Normal operation,
	.dmute = MUTE_OFF,					//	Mute Disable. 0 = Mute; 1 = Normal operation,
	.mono = STEREO,						//	Mono Select. 0 = Stereo; 1 = Force mono,
	.bass = BASS_ON,					//	Bass Boost. BASS_OFF, BASS_ON,
	.rclk_ncm = CLK_ALWAYS_SUPPLY,		//	RCLK NON-CALIBRATE MODE: CLK_ALWAYS_SUPPLY, CLK_NOT_ALWAYS_SUPPLY
	.rclk_dim = DIRECT_INPUT_MODE_OFF,	//	RCLK DIRECT INPUT MODE: DIRECT_INPUT_MODE_OFF, DIRECT_INPUT_MODE_ON,
	.seekup = SEEK_UP,					//	Seek Up. SEEK_DOWN, SEEK_UP,
	.seek = STOP_SEEK,					//	Seek. STOP_SEEK, ENABLE_SEEK,

	//	0x02 LOW BYTE:
	.skmode = SKMODE_WRAP,			//	0 = wrap at the upper or lower band limit and continue seeking; 1 = stop seeking at the upper or lower band limit,
	.clk_mode = CLK_32_768,			//	000=32.768kHz; 001=12Mhz; 101=24Mhz; 010=13Mhz; 110=26Mhz; 011=19.2Mhz; 111=38.4Mhz,
	.rds_en = RDS_OFF,				//	RDS/RBDS enable; If 1, rds/rbds enable,
	.new_method = NEW_METHOD_OFF,	//	New Demodulate Method Enable, can improve the receive sensitivity about 1dB.
	.soft_reset = RESET_OFF,		//	Soft reset. If 0, not reset; If 1, reset.
	.enable = POWER_ON,				//	Power Up Enable. 0 = Disabled; 1 = Enabled
};

RDA5807M_REG_0x03 reg3 =
{
	//	0x03:
	.chan = 0,				//	Channel Select.

	.direct_mode = DIRECTLY_CONTROL_MODE,	//	Directly Control Mode, Only used when test.
	.tune = TUNE_DISABLE,	//	Tune: 0 = Disable, 1 = Enable,
	.band = BAND_87_108,	//	Band Select: 00 = 87–108 MHz (US/Europe) 01 = 76–91 MHz (Japan) 10 = 76–108 MHz (world wide) 111 = 65 –76 MHz （East Europe）  or 50-65MHz
	.space = SPACE_100_kHz,	//	Channel Spacing: 00 = 100 kHz, 01 = 200 kHz, 10 = 50kHz, 11 = 25KHz
};

RDA5807M_REG_0x04 reg4 =
{
	//	0x04:
	.rsvd_1 = 0,					//	Reserved,
	.rsvd_2 = 0,					//	Reserved,
	.de = DE_EMPHASIS_50_US,		//	De-emphasis. 0 = 75 µs; 1 = 50 µs;
	//.rsvd_10 = 0,					//	Reserved,
	.softmute_en = SOFTMUTE_ON,		//	If 1, softmute enable,
	.afcd = AFC_ON,					//	AFC disable. If 0, afc work; If 1, afc disabled,
};
RDA5807M_REG_0x05 reg5 =
{
	//	0x05:
	.int_mode = INT_LAST,	//	If 0, generate 5ms interrupt; If 1, interrupt last until read reg0CH action occurs.
	.rsvd_1 = 0,			//	Reserved,
	.seekth = 0b1000,		//	Seek SNR threshold value
	.lna_icsel_bit=0,
	.lna_port_sel=0,

	//.rsvd_2 = 0,		//	Reserved,
	.volume = 0b0000,	//	DAC Gain Control Bits (Volume). 0000=min; 1111=max Volume scale is logarithmic.
};

RDA5807M_REG_0x06 reg6 =
{
	//	0x06:
	.rsvd_1 = 0,		//	Reserved,
	.rsvd_1_ = 0,		//	Reserved,
	.rsvd_2 = 0,
	.open_mode = OPEN_MODE_OFF,	//	Open reserved register mode. 11=open behind registers writing function others: only open behind registers reading function,
};


RDA5807M_REG_0x07 reg7 =
{
	//	0x07:
	.rsvd_2 = 0,					//	Reserved,
	.th_sofrblend = 0b10000,		//	Threshold for noise soft blend setting, unit 2dB,
	.mode_65m_50m = MODE_65_76,		//	Valid when band[1:0] = 2’b11 (0x03H_bit<3:2>) 1 = 65~76 MHz; 0 = 50~76 MHz.
	.rsvd_1 = 0,					//	Reserved,

	.seek_th_old = 0b000000,		//	Seek threshold for old seek mode, Valid when Seek_Mode=001
	.softblend_en = SOFTBLEND_ON,	//	If 1, Softblend enable
	.freq_mode = FREQ_MODE_OFF,		//	If 1, then freq setting changed. Freq = 76000(or 87000) kHz + freq_direct (08H) kHz.
};


uint16_t *reg_ptr[6] = {&reg2.all, &reg3.all, &reg4.all, &reg5.all, &reg6.all, &reg7.all};

void RDA5807M_set_reg(I2C_t *pI2C, const uint8_t reg);




//	UWAGI !!!,
//	-podczas wykonywania skanowania adresów I2C zaszły następujące wyniki,
//	-układ RDA5807M posiada trzy różne adresy slave:
//	-0x10 --> używamy jeśli chcemy wykorzystać dostęp do odczytu od rejestru 0x0A, nie musimy podawać wtedy numeru rejestru, autoinkrementacja,
//	-0x11 --> używamy jeśli chcemy wybrać odczyt od konkretnego rejestru, autoinkrementacja,
//	-0x60 --> ????????????
//	https://oshwlab.com/wagiminator/attiny13-tinyradio

//	############################################################################
void RDA5807M_init(I2C_t *pI2C)
{
	//	-procedura inicjalizacyjna modułu radiowego RDA5807M,
/*
	delay_ms(2000);
	//ESP_LOGI(TAG, "00000000000000000000000000000000000000000");
	RDA5807M_get_mem(pI2C);	//	Odczyt danych po włączeniu zasilania lub po resecie
	RDA5807M_set_reg(pI2C, REG_02);	//	RESET ONNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
	//RDA5807M_set_mem();	//	RESET ONNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
	//ESP_LOGI(TAG, "11111111111111111111111111111111111111111");
	RDA5807M_get_mem(pI2C);	//	Odczyt danych po włączeniu zasilania lub po resecie
	//ESP_LOGI(TAG, "22222222222222222222222222222222222222222");
	RDA5807M_set_reg(pI2C, REG_03);	//	RESET ONNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
	RDA5807M_get_mem(pI2C);	//	Odczyt danych po włączeniu zasilania lub po resecie,
	//ESP_LOGI(TAG, "33333333333333333333333333333333333333333");
	RDA5807M_get_mem(pI2C);	//	Odczyt danych po włączeniu zasilania lub po resecie,
	while(1);
*/

	VT100_number(I2C_scan(pI2C, 0x11));
	VT100_EOL();


	if(RD5807M_get_power_on(pI2C) == 0)	//	Jeśli uruchomienie po uC power up,
	{
		/*
		RDA5807M_show_reg(pI2C, REG_02, "Odczyt po resecie");
		reg2.soft_reset = RESET_OFF;
		reg2.enable = POWER_ON;
		RDA5807M_set_reg(pI2C, REG_02);
		RDA5807M_show_reg(pI2C, REG_02, "Odczyt po: RESET_OFF");

		RDA5807M_set_mute(pI2C);
		*/
	}
	else	//	Jeśli uruchomienie po uC reset,
	{
		//RDA5807M_set_vol(&I2C_3, 0);
		/*
		RDA5807M_show_reg(pI2C, REG_02, "Odczyt po resecie");
		reg2.soft_reset = RESET_OFF;
		reg2.enable = POWER_ON;
		RDA5807M_set_reg(pI2C, REG_02);
		RDA5807M_show_reg(pI2C, REG_02, "Odczyt po: RESET_OFF");

		reg3.chan = RMF_FM;
		reg3.tune = TUNE_ENABLE;
		RDA5807M_set_reg(pI2C, REG_03);
		RDA5807M_show_reg(pI2C, REG_03, "radio gra:)");
		*/
	}

	/*


	RDA5807M_get_mem(pI2C);
	//RDA5807M_get_mem(pI2C);	//	Odczyt danych po włączeniu zasilania lub po resecie,
	reg2.soft_reset = RESET_ON;
	reg2.enable = POWER_ON;
	RDA5807M_set_reg(pI2C, REG_02);
	//RDA5807M_set_mem(pI2C);
	delay_ms(500);

	reg2.new_method = NEW_METHOD_ON;
	reg2.rds_en = RDS_ON;
	reg2.soft_reset = RESET_OFF;
	reg2.enable = POWER_ON;
	RDA5807M_set_reg(pI2C, REG_02);
	//RDA5807M_set_mem(pI2C);
	delay_ms(500);


	reg3.chan = 91;
	reg3.tune = TUNE_ENABLE;
	RDA5807M_set_reg(pI2C, REG_03);
	//RDA5807M_set_mem(pI2C);


	reg2.seek = ENABLE_SEEK;
	//reg2.enable = POWER_OFF;
	RDA5807M_set_reg(pI2C, REG_02);
	//RDA5807M_set_mem(pI2C);
	delay_ms(500);

	RDA5807M_set_vol(pI2C, 5);
*/
	/*
	reg5.volume = 7;
	RDA5807M_set_mem(pI2C);
	delay_ms(500);
	reg2.soft_reset = RESET_OFF;
	reg2.dmute = MUTE_OFF;
	reg2.enable = POWER_ON;
	RDA5807M_set_mem(pI2C);
	*/
	//RDA5807M_set_reg(pI2C, REG_02);
	//RDA5807M_set_mem(pI2C);
	//delay_ms(500);
	//RDA5807M_get_mem(pI2C);
	//RDA5807M_get_mem(pI2C);	//	Odczyt danych po włączeniu zasilania lub po resecie,
	//reg2.soft_reset = RESET_OFF;
	//RDA5807M_set_mem(pI2C);

	/*
	reg3.chan = 30;
	reg3.tune = TUNE_ENABLE;
	reg2.dhiz = NORMAL_OPERATION;
	reg2.dmute = MUTE_OFF;
	RDA5807M_set_reg(pI2C, REG_02);
	RDA5807M_set_reg(pI2C, REG_03);
*/
	//RDA5807M_get_mem(pI2C);	//	Odczyt danych po włączeniu zasilania lub po resecie,
	//reg2.enable = POWER_ON;
	//RDA5807M_set_reg(pI2C, REG_02);
	//reg2.enable = RESET_OFF;
	//RDA5807M_set_reg(pI2C, REG_02);

/*
	if(RD5807M_get_power_on(pI2C) == 0)
	{
		//ESP_LOGI(TAG, "power on wlaczenie zasilania");
		reg3.chan = 30;
		reg3.tune = TUNE_ENABLE;
		RDA5807M_set_reg(pI2C, REG_03);
		//RDA5807M_set_mem(pI2C);
	}
	else
	{
		reg3.chan = 30;
		reg3.tune = TUNE_ENABLE;
		//RDA5807M_set_mem(pI2C);
		RDA5807M_set_reg(pI2C, REG_03);
		//RDA5807M_set_reg(pI2C, REG_05);
	}
*/
	//RDA5807M_set_vol(pI2C, 0);
	//RDA5807M_get_mem(pI2C);
}


//	$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
void RDA5807M_set_vol(I2C_t *pI2C, uint8_t volume)
{
	//	-utawienie głośności poprzez wpisanie konkretnej wartości do rejestru,
	//	-zakres wpisywanych wartości: 0..15,
	//	-jeśli zostanie wpisana wartość z poza zakresu to ustawiona zostaje maksymalna wartość głośności,

	if(volume>0b1111)
		volume = 0b1111;

	reg5.volume = volume;
	RDA5807M_set_reg(pI2C, REG_05);
}


void RDA5807M_set_vol_up(I2C_t *pI2C)
{
	//	-zwiększenie głośności o jedną wartość w górę,

	if(reg5.volume<0b1111)
	{
		reg5.volume++;
		RDA5807M_set_reg(pI2C, REG_05);
	}
}


void RDA5807M_set_vol_down(I2C_t *pI2C)
{
	//	-zmniejszenie głośności o jedną wartość w dół,

	if(reg5.volume>0)
	{
		reg5.volume--;
		RDA5807M_set_reg(pI2C, REG_05);
	}
}


uint8_t RDA5807M_get_vol(I2C_t *pI2C)
{
	//	-pobranie wartości głośności,

	return reg5.volume;
}


//	$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
void RDA5807M_set_bass(I2C_t *pI2C)
{
	//	-włączenie basów,

	reg2.bass = BASS_ON;
	RDA5807M_set_reg(pI2C, REG_02);
}


void RDA5807M_reset_bass(I2C_t *pI2C)
{
	//	-wyłączenie basów,

	reg2.bass = BASS_OFF;
	RDA5807M_set_reg(pI2C, REG_02);
}


uint8_t RDA5807M_get_bass(I2C_t *pI2C)
{
	//	-pobranie informacji na temat basów,

	return reg2.bass;
}


//	$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
void RDA5807M_set_chan(I2C_t *pI2C, uint16_t chan)
{
	if(chan>0b1111111111)
		chan = 0b1111111111;

	if((reg3.chan >= 0) && (reg3.chan <= 210))
	{
		reg3.chan = chan;
		reg3.tune = TUNE_ENABLE;
		RDA5807M_set_reg(pI2C, REG_03);

		while(RDA5807M_get_STC(pI2C) == 0);	//	Oczekiwanie na zakończenie operacji szukania/strojenia,
	}
}

void RDA5807M_set_chan_up(I2C_t *pI2C)
{
	//	-zwiększenie wartości kanału o jedną wartość w górę,

	if(reg3.chan<0b1111111111 && reg3.chan < 210)
	{
		reg3.chan = RDA5807M_get_chan(pI2C) + 1;
		reg3.tune = TUNE_ENABLE;
		RDA5807M_set_reg(pI2C, REG_03);

		while(RDA5807M_get_STC(pI2C) == 0);	//	Oczekiwanie na zakończenie operacji szukania/strojenia,
	}
}


void RDA5807M_set_chan_down(I2C_t *pI2C)
{
	//	-zmniejszenie wartości kanału o jedną wartość w dół,

	if(reg3.chan > 0)
	{
		reg3.chan = RDA5807M_get_chan(pI2C) - 1;
		reg3.tune = TUNE_ENABLE;
		RDA5807M_set_reg(pI2C, REG_03);

		while(RDA5807M_get_STC(pI2C) == 0);	//	Oczekiwanie na zakończenie operacji szukania/strojenia,
	}
}




uint16_t RDA5807M_get_chan(I2C_t *pI2C)
{
	//	-pobranie wartości kanału z pamięci układu,
	//	-odczyt rejestru 0x0A,

	uint8_t data[2];	//	Zmienna na pobrane dane z rejestru,
	uint16_t data_16;

	I2C_read_bytes(pI2C, RDA5807M_DEV_RAN, 0x0A, data, 2);

	data_16 = ((uint16_t)((data[0] & 0b00000011)<<8)) + data[1];

	return data_16;
}

uint8_t RDA5807M_get_STC(I2C_t *pI2C)
{
	//	-odczyt bitu STC z rejestru 0x0A,
	//	-STC - Seek/Tune Complete,

	uint8_t data[2];	//	Zmienna na pobrane dane z rejestru,

	I2C_read_bytes(pI2C, RDA5807M_DEV_RAN, 0x0A, data, 2);

	return (data[0] & 0b01000000) >> 6;
}


void RDA5807M_set_reg(I2C_t *pI2C, const uint8_t reg)
{
	//	-zapis danych do jednego z rejestrów układu,
	//	-zapis w trybie RANDOM,

	uint16_t reg_data = *reg_ptr[reg-2];
	uint8_t data[2];

	data[0] = reg_data >> 8;
	data[1] = reg_data & 0xFF;

	I2C_write_bytes(pI2C, RDA5807M_DEV_RAN, reg, data, 2);
}


uint16_t RDA5807M_get_reg(I2C_t *pI2C, const uint8_t reg)
{
	//	-pobranie wartości z jednego rejestru układu,
	//	-pobranie danych w trybie RANDOM,

	uint8_t data[2];	//	Zmienna na pobrane dane z rejestru,

	I2C_read_bytes(pI2C, RDA5807M_DEV_RAN, reg, data, 2);
	return (data[0]<<8) | (data[1] & 0xFF);
}


void RDA5807M_show_reg(I2C_t *pI2C, const uint8_t reg, char* description)
{
	//	-pobranie wartości z jednego rejestru układu oraz wyświetlenie wraz z komentarzem,

	VT100_EOL();
	VT100_text(description);
	VT100_EOL();
	VT100_show_data_16(RDA5807M_get_reg(pI2C, reg), *(RDA5807M_reg_ptr + reg));
}


void RDA5807M_set_mute(I2C_t *pI2C)
{
	//	-włączenie wyciszenia radia,

	reg2.dmute = MUTE_ON;
	RDA5807M_set_reg(pI2C, REG_02);
}

void RDA5807M_reset_mute(I2C_t *pI2C)
{
	//	-wyłączenie wyciszenia radia,

	reg2.dmute = MUTE_OFF;
	RDA5807M_set_reg(pI2C, REG_02);
}


uint8_t RDA5807M_get_mute(I2C_t *pI2C)
{
	//	-pobranie informacji na czy wyciszenie jest włączone,

	return reg2.dmute;
}


//	$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
uint16_t RD5807M_get_power_on(I2C_t *pI2C)
{
	//	-uzyskanie informacji na temat czy radio jest uruchomione,
	//	-zawsze po włączeniu zasilania bit 'enable' przyjmuje wartość '0', czyli radio nie jest uruchomione,
	//	-po resecie uC, jeśli radio było uruchomione to bit 'enable' przyjmuje wartość '1',
	//	-odczytu dokonujemy aby sprawdzić czy po wykonaniu resetu uC nie wyłączać radia jeśli było
	//	wcześniej uruchomione,

	uint16_t data = RDA5807M_get_reg(pI2C, REG_02);

	return (data & 0x0001);
}


//	$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

void RDA5807M_set_mem(I2C_t *pI2C)
{
	//	-zapis danych do wszystkich rejestrów układu,

	/*
	RDA5807M_set_reg(REG_02);
	RDA5807M_set_reg(REG_03);
	RDA5807M_set_reg(REG_04);
	RDA5807M_set_reg(REG_05);
	RDA5807M_set_reg(REG_06);
	RDA5807M_set_reg(REG_07);
*/

	uint8_t data[12];	//	Pojemnik na dane do przekazania do wysłania,
	uint16_t reg_data;	//	Zmienna na wartość pobraną ze struktury,
	uint16_t data_16[6];

	for(uint8_t i=0; i<6; i++)
	{
		reg_data = *reg_ptr[i];				//	Pobór wartości ze struktury,
		data[i<<1] = reg_data>>8;			//	Zapis górnych bitów,
		data[(i<<1)+1] = reg_data & 0xFF;	//	Zapis dolnych bitów,
	}


	//	/////////////////////////////////////////
	VT100_EOL();
	VT100_text("DANE DO ZAPISU:");
	VT100_EOL();

	for(uint8_t i=0; i<6; i++)
	{
		data_16[i] = ((uint16_t)data[(i<<1)]<<8) + data[(i<<1) + 1];			//	Zapis górnych bitów,
		//data[(i<<1)+1] = reg_data & 0xFF;	//	Zapis dolnych bitów,
		//data[i] = I2C_read_current_address(pI2C, RDA5807M_DEV_SEQ);
		//VT100_show_register(data[i], "data");
		VT100_show_data_16(data_16[i], RDA5807M_reg_ptr[i+2]);
	}
	VT100_EOL();
	//	/////////////////////////////////////////


	I2C_write_bytes(pI2C, RDA5807M_DEV_RAN, 0x02, data, 12);
	//I2C_write_bytes_no_reg(pI2C, RDA5807M_DEV_RAN, data, 12);
}

void RDA5807M_get_mem(I2C_t *pI2C)
{
	//	-pobranie wartości z rejestrów konfiguracyjnych,
	//	-odczyt 6 rejestrów, od adresu 0x02 do 0x07,

	uint8_t data[12];	//	Zmienna na pobrane dane z rejestru,
	uint16_t data_16[6];

	I2C_read_bytes(pI2C, 0x60, 0x02, data, 12);


	//	/////////////////////////////////////////
	VT100_EOL();
	//VT100_text("DANE POBRANE:");
	VT100_EOL();

	for(uint8_t i=0; i<12; i++)
	{
		//VT100_show_register(data[i], "data");
	}
	VT100_EOL();
	//	/////////////////////////////////////////


	//	/////////////////////////////////////////
	VT100_EOL();
	VT100_text("DANE POBRANE:");
	VT100_EOL();

	for(uint8_t i=0; i<6; i++)
	{
		data_16[i] = ((uint16_t)data[(i<<1)]<<8) + data[(i<<1) + 1];			//	Zapis górnych bitów,
		//data[(i<<1)+1] = reg_data & 0xFF;	//	Zapis dolnych bitów,
		//data[i] = I2C_read_current_address(pI2C, RDA5807M_DEV_SEQ);
		//VT100_show_register(data[i], "data");
		VT100_show_data_16(data_16[i], RDA5807M_reg_ptr[i+2]);
	}
	VT100_EOL();
	//	/////////////////////////////////////////



	/*
	//	##########################################################################################################################
	next_line();
	ESP_LOGI(TAG, "ZAWARTOSC REJESTROW KONFIGURACYJNYCH UKLADU RDA5807M:");
	for(uint8_t i=0; i<12; i++)
	{
		if(i%2 == 0)
		{
			next_line();
			ESP_LOGI(TAG, "Rejestr nr: 0x%02X, nazwa: %s", (i>>1)+REG_02, RDA5807M_reg_ptr[(i>>1)+REG_02]);
		}
		to_bytes(data[i]);
	}
	//	##########################################################################################################################
	 *
	 */
}


void RDA5807M_get_RDS(I2C_t *pI2C)
{
	//	-pobranie wartości z rejestrów tylko do odczytu,
	//	-odczyt 6 rejestrów, od adresu 0x0A do 0x0F,

	uint8_t data[12];	//	Zmienna na pobrane dane z rejestru,

	I2C_read_bytes(pI2C, RDA5807M_DEV_RAN, REG_0A, data, 12);
	///////I2C_read(RDA5807M_DEV_RAN, REG_0A, 12, data);

	/*
	//	##########################################################################################################################
	next_line();
	ESP_LOGI(TAG, "ZAWARTOSC REJESTROW RDS UKLADU RDA5807M:");
	for(uint8_t i=0; i<12; i++)
	{
		if(i%2 == 0)
		{
			next_line();
			ESP_LOGI(TAG, "Rejestr nr: 0x%02X, nazwa: %s", (i>>1)+REG_0A, RDA5807M_reg_ptr[(i>>1)+REG_0A]);
		}
		to_bytes(data[i]);
	}
	//	##########################################################################################################################
*/

	/*
	uint16_t RDS_1 = (data[0]<<8) | (data[1]);
	uint16_t RDS_2 = (data[2]<<8) | (data[3]);
	uint16_t block_A = (data[4]<<8) | (data[5]);
	uint16_t block_B = (data[6]<<8) | (data[7]);
	uint16_t block_C = (data[8]<<8) | (data[9]);
	uint16_t block_D = (data[10]<<8) | (data[11]);
*/
	/*
	show_word(RDS_1);
	show_word(RDS_2);
	show_word(block_A);
	show_word(block_B);
	show_word(block_C);
	show_word(block_D);
*/
	//if((data[0] & 0b10000000))	ESP_LOGI(TAG, "Nowe dane RDS gotowe");	else	ESP_LOGI(TAG, "Dane nie sa gotowe");
	//if((data[0] & 0b01000000))	ESP_LOGI(TAG, "Strojenia zakonczone");	else	ESP_LOGI(TAG, "Blad w strojeniu");

	uint8_t index = 0;
	/*
	for(uint8_t i=5; i!=255; i--)
	{
		if((data[0] & (1<<(i+2)) ))	ESP_LOGI(TAG, "%s", RDA5807M_reg_0x0A_ptr[(index<<1)+1]);	else	ESP_LOGI(TAG, "%s", RDA5807M_reg_0x0A_ptr[(index<<1)]);
		index++;
	}
	*/
	for(uint8_t i=7; i>1; i--)
	{
		////////if((data[0] & (1<<i) ))	ESP_LOGI(TAG, "%s", RDA5807M_reg_0x0A_ptr[(index << 1)+1]);	else	ESP_LOGI(TAG, "%s", RDA5807M_reg_0x0A_ptr[(index<<1)]);
		index++;
	}

	/*
	ESP_LOGI(TAG, "DEKODOWANIE RDS:");
	uint8_t group_type = 0x0A | ((block_B & 0xF000) >> 8) | ((block_B & 0x0800) >> 11);	//	0b00001010 | 0b11110000 | 0b00000001,
	ESP_LOGI(TAG, "TYP GRUPY: 0x%02X", group_type);
	*/
}

void RDA5807M_get_RDS_loop(I2C_t *pI2C)
{
	//	-pobranie wartości z rejestrów tylko do odczytu,
	//	-odczyt 6 rejestrów, od adresu 0x0A do 0x0F,

	uint8_t data[12];	//	Zmienna na pobrane dane z rejestru,

	while(1)
	{
		I2C_read_bytes(pI2C, RDA5807M_DEV_RAN, REG_0A, data, 12);
		//////I2C_read(RDA5807M_DEV_RAN, REG_0A, 12, data);

		/*
		//	##########################################################################################################################
		next_line();
		ESP_LOGI(TAG, "ZAWARTOSC REJESTROW RDS UKLADU RDA5807M:");
		for(uint8_t i=0; i<12; i++)
		{
			if(i%2 == 0)
			{
				next_line();
				ESP_LOGI(TAG, "Rejestr nr: 0x%02X, nazwa: %s", (i>>1)+REG_0A, RDA5807M_reg_ptr[(i>>1)+REG_0A]);
			}
			to_bytes(data[i]);
		}
		//	##########################################################################################################################
	*/

		uint16_t RDS_1 = (data[0]<<8) | (data[1]);
		uint16_t RDS_2 = (data[2]<<8) | (data[3]);
		uint16_t block_A = (data[4]<<8) | (data[5]);
		uint16_t block_B = (data[6]<<8) | (data[7]);
		uint16_t block_C = (data[8]<<8) | (data[9]);
		uint16_t block_D = (data[10]<<8) | (data[11]);

		RDS.block_B = (data[6]<<8) | (data[7]);

		//show_word(RDS_1);
		//show_word(RDS_2);
		//show_word(block_A);
		//show_word(block_B);
		//show_word(block_C);
		//show_word(block_D);

		//if((data[0] & 0b10000000))	ESP_LOGI(TAG, "Nowe dane RDS gotowe");	else	ESP_LOGI(TAG, "Dane nie sa gotowe");
		//if((data[0] & 0b01000000))	ESP_LOGI(TAG, "Strojenia zakonczone");	else	ESP_LOGI(TAG, "Blad w strojeniu");

		/*
		uint8_t index = 0;

		for(uint8_t i=7; i>1; i--)
		{
			if((data[0] & (1<<i) ))	ESP_LOGI(TAG, "%s", RDA5807M_reg_0x0A_ptr[(index << 1)+1]);	else	ESP_LOGI(TAG, "%s", RDA5807M_reg_0x0A_ptr[(index<<1)]);
			index++;
		}
		*/

		//	######################################################################################################################
		//ESP_LOGI(TAG, "DEKODOWANIE RDS:");
		//uint8_t group_type = 0x0A | ((block_B & 0xF000) >> 8) | ((block_B & 0x0800) >> 11);	//	0b00001010 | 0b11110000 | 0b00000001,
		//ESP_LOGI(TAG, "TYP GRUPY: 0x%02X", group_type);
		//next_line();
		//next_line();
		//	######################################################################################################################


		//	######################################################################################################################
		//	-odczyt trwania stanów w bit 15, reg 0x0A,

		static uint8_t state = 0;

		if((data[0] & 0b10000000) && state == 0)
		{
			//ets_printf("%lld", time_from_loop());		//	Postać szesnastkowa,
			//next_line();
			state = 1;
			//ESP_LOGI(TAG, "DEKODOWANIE RDS:");
			uint8_t group_type = 0x0A | ((block_B & 0xF000) >> 8) | ((block_B & 0x0800) >> 11);	//	0b00001010 | 0b11110000 | 0b00000001,
			//ESP_LOGI(TAG, "TYP GRUPY: 0x%02X", group_type);


			uint8_t station_name[8];	//	Tablica na zapis nazwy stacji,

			static uint8_t i=0;

			if(group_type==0x0A)
			{

				//show_word(block_A);
				//show_word(block_B);
				//show_word(block_C);
				//show_word(block_D)


				////////ESP_LOGI(TAG, "%c", data[10]);
			////////////	ESP_LOGI(TAG, "%c", data[11]);
				//////////next_line();

				if((block_B & 0x0003) == 0)
					i=0;

				station_name[i] = data[10];
				i++;
				station_name[i] = data[11];
				i++;

				if(i==8)
				{
					i=0;
					//next_line();
					for(uint8_t j=0; j<8; j++)
					{
						/////////////ets_printf("%c", station_name[j]);
						//ESP_LOGI(TAG, "%c", station_name[j]);
					}
					//next_line();
				}

			}

			if(group_type==0x2A)
			{/*
				ESP_LOGI(TAG, "%c", data[8]);
				ESP_LOGI(TAG, "%c", data[9]);
				ESP_LOGI(TAG, "%c", data[10]);
				ESP_LOGI(TAG, "%c", data[11]);
				next_line();
*/

			}


		}
		if((data[0] & 0b10000000) == 0 && state == 1)
		{
			//ets_printf("%lld", time_from_loop());		//	Postać szesnastkowa,
			//next_line();
			state = 0;
		}


	}
}




/*
void RDA5807M_get_reg(const uint8_t reg)
{
	//	-procedura inicjalizacyjna modułu radiowego RDA5807M,

	uint8_t data[2];	//	Zmienna na pobrane dane z rejestru,

	I2C_read(RDA5807M_DEV_RAN, reg, 2, data);

	ESP_LOGI(TAG, "ZAWARTOSC POJEDYNCZEGO REJESTRU:");
	for(uint8_t i=0; i<2; i++)
	{
		if(i%2 == 0)
		{
			next_line();
			ESP_LOGI(TAG, "Rejestr nr: %i, nazwa: %s", reg, "xxx");
		}
		to_bytes(data[i]);
		//ESP_LOGI(TAG, "------------------------------------------------------");
	}
}

void RDA5807M_set_reg_(const uint8_t reg)
{
	//	-zapis danych do jednego z rejestrów układu,

	uint8_t data[12];

	//	0x02:
	data[0] = FM.dhiz | FM.dmute | FM.mono | FM.bass | FM.rclk_ncm | FM.rclk_dim | FM.seekup | FM.seek;
	data[1] = FM.skmode | FM.clk_mode | FM.rds_en | FM.new_method | FM.soft_reset | FM.enable;

	//	0x03:
	data[2] = (FM.chan)>>2;
	data[3] = (FM.chan)<<6 | FM.direct_mode | FM.tune | FM.band | FM.space;

	//	0x04:
	data[4] = FM.de | FM.softmute_en | FM.afcd;
	data[5] = 0;

	//	0x05:
	data[6] = FM.int_mode | FM.seekth;
	data[7] = FM.volume;

	//	0x06:
	data[8] = FM.open_mode;
	data[9] = 0;

	//	0x07:
	data[10] = ((FM.th_sofrblend)<<2) | FM.mode_65m_50m;
	data[11] = ((FM.seek_th_old)<<2) | FM.softblend_en | FM.freq_mode;


	next_line();
	ESP_LOGI(TAG, "KONWERSJA DANYCH");
	for(uint8_t i=0; i<12; i++)
	{
		to_bytes(data[i]);
	}
	next_line();

	//I2C_write_radio(RDA5807M_DEV_SEQ, 12, data);
	//I2C_write_radio(RDA5807M_DEV_SEQ, 12, data);

	I2C_write(RDA5807M_DEV_RAN, reg, 2, &data[(reg-2)<<1]);
	I2C_write(RDA5807M_DEV_RAN, reg, 2, &data[(reg-2)<<1]);

}

void RDA5807M_set_byte(const uint8_t reg, const uint8_t byte, const uint8_t state)
{
	//	-ustawienie jednego bitu w dowolnym rejestrze,

	uint8_t data[2];	//	Zmienna na pobrane dane z rejestru,
	uint16_t data_16;

	I2C_read(RDA5807M_DEV_RAN, reg, 2, data);
	data_16 = (data[0]<<8) + data[1];

	to_bytes(data[0]);
	to_bytes(data[1]);


	data_16 = data_16 & (~(1<<byte));
	data_16 = data_16 | (state << byte);

	to_bytes(data_16>>8);
	to_bytes(data_16);

	data[0] = data_16>>8;
	data[1] = data_16 & 0xFF;

	to_bytes(data[0]);
	to_bytes(data[1]);

	I2C_write(RDA5807M_DEV_RAN, reg, 2, data);
	//I2C_write_byte_no_reg(RDA5807M_DEV_SEQ, data[0]);
}




 *
 *
 *
 *void RDA5807M_set_mem__(RDA5807M *ptr)
{
	uint8_t data[12];

	//	0x02:
	data[0] = ptr->dhiz | ptr->dmute | ptr->mono | ptr->bass | ptr->rclk_ncm | ptr->rclk_dim | ptr->seekup | ptr->seek;
	data[1] = ptr->skmode | ptr->clk_mode | ptr->rds_en | ptr->new_method | ptr->soft_reset | ptr->enable;

	//	0x03:
	data[2] = (ptr->chan)>>2;
	data[3] = (ptr->chan)<<6 | ptr->direct_mode | ptr->tune | ptr->band | ptr->space;

	//	0x04:
	data[4] = ptr->de | ptr->softmute_en | ptr->afcd;
	data[5] = 0;

	//	0x05:
	data[6] = ptr->int_mode | ptr->seekth;
	data[7] = ptr->volume;

	//	0x06:
	data[8] = ptr->open_mode;
	data[9] = 0;

	//	0x07:
	data[10] = ((ptr->th_sofrblend)<<2) | ptr->mode_65m_50m;
	data[11] = ((ptr->seek_th_old)<<2) | ptr->softblend_en | ptr->freq_mode;


	next_line();
	ESP_LOGI(TAG, "KONWERSJA DANYCH");
	for(uint8_t i=0; i<12; i++)
	{
		to_bytes(data[i]);
	}
	next_line();

	//I2C_write_radio(RDA5807M_DEV_SEQ, 12, data);
	//I2C_write_radio(RDA5807M_DEV_SEQ, 12, data);

	I2C_write(RDA5807M_DEV_RAN, 0x02, 12, data);
	I2C_write(RDA5807M_DEV_RAN, 0x02, 12, data);
}
 *
 *
 *
 *
 *
void RDA5807M_write_reg(void)
{
	//	-zapis rejestrów od adresu 0x02 do 0x07,



	uint8_t write_reg[12] =
	{
		0b11000001,
		0b00000011,

		0x00,
		0x00,

		0b00001010,
		0b00000000,

		0b10001000,
		0b00001111,

		0x00,
		0x00,

		0b01000010,
		0b00000010
	};

	//I2C_write_radio(RDA5807M_DEV_SEQ, 12, write_reg);
	I2C_write(RDA5807M_DEV_RAN, 0x02, 12, write_reg);
}


void RDA5807M_write_reg_go(void)
{
	//	-zapis rejestrów od adresu 0x02 do 0x07,

	uint8_t channel = 91;

	uint8_t tune_config[4] = {
	  // register 0x02
	  0b11000000,
	   0b00000001,
	   // register 0x03
	   (channel >> 2),
	   ((channel & 0b11) << 6 ) | 0b00010000
	};



	//I2C_write_radio(RDA5807M_DEV_SEQ, 12, write_reg);
	I2C_write(RDA5807M_DEV_RAN, 0x02, 4, tune_config);
}




void RDA5807M_init_2(void)
{
	//	-procedura inicjalizacyjna modułu radiowego RDA5807M,

	uint8_t data[RDA5807M_MEM_LENGTH_READ<<1];	//	Zmienna na pobrane dane z rejestru,

	I2C_read_no_reg(RDA5807M_DEV_SEQ, RDA5807M_MEM_LENGTH_READ<<1, data);
	//I2C_read(RDA5807M_DEV, 0x00, 250, data);
	//data = I2C_read_byte(RDA5807M_DEV, 0x0A);	//	Odczyt danych z rejestru,

	ESP_LOGI(TAG, "ZAWARTOSC REJESTROW UKLADU RDA5807M:");
	for(uint16_t i=0; i<(RDA5807M_MEM_LENGTH_READ<<1); i++)
	{
		if(i%2 == 0)
		{
			next_line();
			ESP_LOGI(TAG, "Rejestr nr: %i, nazwa: %s", i>>1, RDA5807M_reg_read_ptr[i>>i]);
		}
		to_bytes(data[i]);
		//ESP_LOGI(TAG, "------------------------------------------------------");
	}


	RDA5807M_conv(&FM);
}




void RDA5807M_init_random(void)
{
	//	-procedura inicjalizacyjna modułu radiowego RDA5807M,

	uint8_t data[RDA5807M_MEM_LENGTH_WRITE<<1];	//	Zmienna na pobrane dane z rejestru,

	I2C_read(RDA5807M_DEV_RAN, 0x02, RDA5807M_MEM_LENGTH_WRITE<<1, data);
	//I2C_read(RDA5807M_DEV, 0x00, 250, data);
	//data = I2C_read_byte(RDA5807M_DEV, 0x0A);	//	Odczyt danych z rejestru,

	ESP_LOGI(TAG, "ZAWARTOSC REJESTROW UKLADU RDA5807M:");
	for(uint16_t i=0; i<(RDA5807M_MEM_LENGTH_WRITE<<1); i++)
	{
		if(i%2 == 0)
		{
			next_line();
			ESP_LOGI(TAG, "Rejestr nr: %i, nazwa: %s", i>>1, RDA5807M_reg_write_ptr[(i>>1)]);
		}
		to_bytes(data[i]);
		//ESP_LOGI(TAG, "------------------------------------------------------");
	}
}


uint8_t RDA5807M_vol(const uint8_t vol)
{
	//uint8_t data[2];

	if(vol == 255)
		return 0;
	if(vol == 16)
		return 15;

	FM.volume = vol;

	RDA5807M_set_reg(0x05);


	//data[0] = FM.int_mode | FM.seekth;
	//data[1] = FM.volume;

	//I2C_write(RDA5807M_DEV_RAN, 0x05, 2, data);
	//I2C_write(RDA5807M_DEV_RAN, 0x05, 2, data);

	//RDA5807M_conv(&FM);
	return vol;
}



 */



