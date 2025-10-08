/*
 * i2c.c
 *
 *  Created on: 21 sie 2024
 *      Author: asmnop
 */


#include "..\..\LIB\Inc\i2c.h"

#include "software_timers.h"


#include "../../LIB/Inc/universal_functions.h"
#include "../../LIB/Inc/VT100.h"

#define NUMBER_OF_DEVICES	7

const char I2C_addr_0x10[] = "???";
const char I2C_addr_0x11[] = "Radio z RDS";
const char I2C_addr_0x3C[] = "OLED";
const char I2C_addr_0x57[] = "Pamiec EEPROM z serii 24LCxx";
const char I2C_addr_0x60[] = "TEA5767 Radio Receiver";
const char I2C_addr_0x68[] = "RTC";
const char I2C_addr_0x76[] = "BMP280 - czujnik";

const char* const I2C_addr_arr[NUMBER_OF_DEVICES] =
{
	I2C_addr_0x10,
	I2C_addr_0x11,
	I2C_addr_0x3C,
	I2C_addr_0x57,
	I2C_addr_0x60,
	I2C_addr_0x68,
	I2C_addr_0x76
};

const uint8_t name_arr[NUMBER_OF_DEVICES+1] = {0x10, 0x11, 0x3C, 0x57, 0x60, 0x68, 0x76, 0};

//	https://learn.adafruit.com/i2c-addresses/the-list


//	UWAGI I SPOSTRZEŻENIA:
//	-patrząc na budowę poniższych funkcji można zauważyć, że dzielą się na następujące grupy:
//	-odczyt/zapis bez podawania adresu początkowego,
//	-odczyt/zapis z adresem początkowym o długości jednego bajtu,
//	-odczyt/zapis z adresem początkowym o długości dwóch bajtów,
//	-odczyt/zapis jednego bajtu,
//	-odczyt/zapis tablicy bajtów,
//	-'I2C_read_current_address' --> odczyt bez podawania adresu początkowego,
//	-'I2C_read_random' --> odczyt jednego bajtu z adresem początkowym dwu bajtowym,
//	-'I2C_read_sequential' --> odczyt tablicy bajtów z adresem początkowym dwu bajtowym,

//	Z powyższych spostrzeżeń można wykonać następujące grupy funckji:
//	I2C_write_byte_no_reg(I2C_t *pI2C, const uint8_t dev_address, const uint8_t data);
//	I2C_read_byte_no_reg(I2C_t *pI2C, const uint8_t dev_address, const uint8_t data);
//	I2C_write_byte(I2C_t *pI2C, uint8_t dev_address, const uint8_t data);
//	I2C_write_bytes(I2C_t *pI2C, uint8_t dev_address, const uint8_t start_address, const uint8_t numberOfData, uint8_t data[]);
//	I2C_read_byte
//	I2C_read_bytes
//	I2C_write_byte_EE
//	I2C_write_bytes_EE
//	I2C_read_byte_EE
//	I2C_read_bytes_EE

//	I2C_write_byte_no_reg
//	I2C_read_byte_no_reg
//	I2C_write_bytes
//	I2C_read_bytes
//	I2C_write_bytes_EE
//	I2C_read_bytes_EE

//	I2C_write_bytes(I2C_t *pI2C, uint8_t dev_address, const uint8_t mem_address, const uint8_t mem_address_size, uint8_t data[], const uint8_t size);
//	I2C_read_bytes

/*
//	REJESTRY:
//	############################################################################
//	CR1 -->
//	ANFOFF - Analog noise filter OFF - włączenie filtru analogowego, '1' - filtr wyłączony
void LL_I2C_EnableAnalogFilter(I2C_TypeDef *I2Cx);

//	DNF[3:0] - Digital Noise Filter - wielkość filtru cyfrowego,
void LL_I2C_ConfigFilters(I2C_TypeDef *I2Cx, uint32_t AnalogFilter, uint32_t DigitalFilter);
void LL_I2C_SetDigitalFilter(I2C_TypeDef *I2Cx, uint32_t DigitalFilter);

//	PE - Peripheral Enable - włączenie magistrali - '1',
void LL_I2C_Enable(I2C_TypeDef *I2Cx);

//	############################################################################
//	CR2 === Control Register
//	AUTOEND - Automatic end mode (master mode), bit ustawiany i zdejmowany programowo
//	0: Programowy tryb końca: flaga TC jest ustawiana kiedy NBYTES dane są przesłane
//	rozszerzanie stanu niskiego SCL,
//	1: Automatyczny tryb końca: warunek STOP jest automatycznie wysyłany kiedy dane NBYTES
//	są wysłane,
void LL_I2C_EnableAutoEndMode(I2C_TypeDef *I2Cx);

//	NBYTES[7:0] - liczba bajtów,
void LL_I2C_SetTransferSize(I2C_TypeDef *I2Cx, uint32_t TransferSize);

//	STOP: generowanie stop
//	Bit ustawiany programowo, zdejmowany sprzętowo kiedy warunek STOP jest wykryty lub kiedy
//	PE = 0,
void LL_I2C_GenerateStopCondition(I2C_TypeDef *I2Cx);

//	START: generowanie START
void LL_I2C_GenerateStartCondition(I2C_TypeDef *I2Cx);

//	ADD10: tryb adresowania 10-bitowego
//	0: master używa trybu adresowania 7-bitowego,
//	1: master używa trybu adresowania 10-bitowego,
//	void LL_I2C_SetMasterAddressingMode(I2C_TypeDef *I2Cx, uint32_t AddressingMode)
void LL_I2C_SetMasterAddressingMode(I2C_TypeDef *I2Cx, uint32_t AddressingMode);


//	RD_WRN: kierunek transmisji
//	0: master zadaje transfer zapisu
//	1: master zadaje transfer odczytu,
void LL_I2C_SetTransferRequest(I2C_TypeDef *I2Cx, uint32_t TransferRequest);

//	SADD[9:0] - Slave address - adres układu slave,
//	W trybie adresowania 7-bitowego:
void LL_I2C_SetSlaveAddr(I2C_TypeDef *I2Cx, uint32_t SlaveAddr);

//	############################################################################
//	TIMINGR - rejestr dotyczący ustawień czasowych
void LL_I2C_SetTiming(I2C_TypeDef *I2Cx, uint32_t Timing);

//	############################################################################
//	RXDR - rejestr danych odebranych
//	RXDATA[7:0] - 8 bitów odebranych danych, dane odebrane z szyny I2C,
uint8_t LL_I2C_ReceiveData8(I2C_TypeDef *I2Cx);

//	############################################################################
//	TXDR - Transmit Data Register - Rejestr danych wysyłanych
//	TXDATA[7:0] - 8 bitów wysłanych danych,
//	Bity mogą być zapisane tylko wtedy gdy TXE=1,
void LL_I2C_TransmitData8(I2C_TypeDef *I2Cx, uint8_t Data);
*/

//	////////////////////////////////////////////////////////////////////////////
void I2C_init(I2C_t *pI2Cx)
{
	LL_APB1_GRP1_EnableClock(pI2Cx->enable_periph);

	//	Dla pewności wyłączamy interfejs I2C:
	LL_I2C_Disable(pI2Cx->I2Cx);

	//	Konfiguracja filtrów:
	LL_I2C_ConfigFilters(pI2Cx->I2Cx, LL_I2C_ANALOGFILTER_ENABLE, 0x00);

	//	Wartość timingów:
	LL_I2C_SetTiming(pI2Cx->I2Cx, pI2Cx->timing_register);

	//	Dla trybu master wymagane jest włączenie rozszerzania zegara:
	LL_I2C_EnableClockStretching(pI2Cx->I2Cx);

	//	Wybór komunikacji - I2C:
	LL_I2C_SetMode(pI2Cx->I2Cx, LL_I2C_MODE_I2C);

	//	Włączenie interfejsu po zakończonej konfiguracji:
	LL_I2C_Enable(pI2Cx->I2Cx);
}

uint8_t I2C_scan(I2C_t *pI2Cx, uint8_t address)
{
	LL_I2C_ClearFlag_STOP(pI2Cx->I2Cx);
	LL_I2C_ClearFlag_NACK(pI2Cx->I2Cx);

	//	Wysłanie pod adres 'address' jednego bajtu danych z opcją automatycznego generowania STOP, zapis do SLAVE,
	LL_I2C_HandleTransfer(pI2Cx->I2Cx, address<<1, LL_I2C_ADDRSLAVE_7BIT, (uint32_t)0, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);

	//	Oczekiwanie na zdjęcie flagi TXIS,
	//	Flaga jest zdejmowana przy zapisie rejestru TXDR kolejnymi danymi do wysyłu,
	//while(LL_I2C_IsActiveFlag_TXIS(pI2Cx->I2Cx) == 0);

	timer_1 = 2;
	while(LL_I2C_IsActiveFlag_STOP(pI2Cx->I2Cx) == 0 && timer_1);	//	Oczekiwanie na ustawienie flagi STOP,
	LL_I2C_ClearFlag_STOP(pI2Cx->I2Cx);
	//delay_ms(100);

	if(LL_I2C_IsActiveFlag_NACK(pI2Cx->I2Cx) == 1 || timer_1 == 0)
	{
		LL_I2C_ClearFlag_NACK(pI2Cx->I2Cx);
		return 200;
	}

	timer_1 = 0;

	return address;
}

void I2C_scan_all(I2C_t *pI2Cx)
{

	uint8_t counter_temp = 0;
	uint8_t is_exist = 0;

	for(uint8_t i=0; i<I2C_NUMBER_OF_SCAN_ADDRESSES; i++)
	{
		pI2Cx->address[i] = 222;
	}

	for(uint8_t i=1; i<128; i++)
	{
		is_exist = I2C_scan(pI2Cx, i);
		if(is_exist != 200)
		{
			pI2Cx->address[counter_temp] = is_exist;
			counter_temp++;
		}
	}
}




//	////////////////////////////////////////////////////////////////////////////
void I2C_write_byte_no_reg(I2C_t *pI2C, const uint8_t dev_address, const uint8_t data)
{
	LL_I2C_ClearFlag_STOP(pI2C->I2Cx);
	LL_I2C_ClearFlag_NACK(pI2C->I2Cx);


	//	Wysłanie pod adres 'address' jednego bajtu danych z opcją automatycznego generowania STOP, zapis do SLAVE,
	LL_I2C_HandleTransfer(pI2C->I2Cx, dev_address<<1, LL_I2C_ADDRSLAVE_7BIT, (uint32_t)1, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);

	//	Oczekiwanie na zdjęcie flagi TXIS,
	//	Flaga jest zdejmowana przy zapisie rejestru TXDR kolejnymi danymi do wysyłu,
	while(LL_I2C_IsActiveFlag_TXIS(pI2C->I2Cx) == 0);

	LL_I2C_TransmitData8(pI2C->I2Cx, data);
	////////////////////////////////////////////////////////////////////////////
	while(LL_I2C_IsActiveFlag_STOP(pI2C->I2Cx) == 0);	//	Oczekiwanie na ustawienie flagi STOP,
	LL_I2C_ClearFlag_STOP(pI2C->I2Cx);
}

void I2C_write_bytes_no_reg(I2C_t *pI2C, const uint8_t dev_address, const uint8_t data[], const uint8_t data_size)
{

	uint8_t read_data_count = data_size;

	LL_I2C_ClearFlag_STOP(pI2C->I2Cx);
	LL_I2C_ClearFlag_NACK(pI2C->I2Cx);


	//	Wysłanie pod adres 'address' jednego bajtu danych z opcją automatycznego generowania STOP, zapis do SLAVE,
	LL_I2C_HandleTransfer(pI2C->I2Cx, dev_address<<1, LL_I2C_ADDRSLAVE_7BIT, (uint32_t)(data_size), LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);

	//	Oczekiwanie na zdjęcie flagi TXIS,
	//	Flaga jest zdejmowana przy zapisie rejestru TXDR kolejnymi danymi do wysyłu,

	while(read_data_count > 0)
	{
		VT100_number(read_data_count);
		VT100_EOL();
		while(LL_I2C_IsActiveFlag_TXIS(pI2C->I2Cx) == 0);

		LL_I2C_TransmitData8(pI2C->I2Cx, *data);
		data++;
		read_data_count--;
	}

	////////////////////////////////////////////////////////////////////////////
	while(LL_I2C_IsActiveFlag_STOP(pI2C->I2Cx) == 0);	//	Oczekiwanie na ustawienie flagi STOP,
	LL_I2C_ClearFlag_STOP(pI2C->I2Cx);
}


//	////////////////////////////////////////////////////////////////////////////
void I2C_write_byte(I2C_t *pI2C, uint8_t dev_address, const uint8_t data)
{
	//	Wysłanie pod adres 'address' jednego bajtu danych z opcją automatycznego generowania STOP, zapis do SLAVE,
	LL_I2C_HandleTransfer(pI2C->I2Cx, dev_address<<1, LL_I2C_ADDRSLAVE_7BIT, (uint32_t)1, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);

	//	Oczekiwanie na zdjęcie flagi TXIS,
	//	Flaga jest zdejmowana przy zapisie rejestru TXDR kolejnymi danymi do wysyłu,
	while(LL_I2C_IsActiveFlag_TXIS(pI2C->I2Cx) == 0);
	LL_I2C_TransmitData8(pI2C->I2Cx, data);

	while(LL_I2C_IsActiveFlag_STOP(pI2C->I2Cx) == 0);	//	Oczekiwanie na ustawienie flagi STOP,
	LL_I2C_ClearFlag_STOP(pI2C->I2Cx);
}

uint8_t I2C_write_bytes(I2C_t *pI2C, uint8_t dev_address, const uint8_t mem_address, const uint8_t data[], const uint8_t number_of_data)
{
	uint8_t read_data_count = number_of_data;

	//	Wysłanie pod adres 'address' jednego bajtu danych z opcją automatycznego generowania STOP, zapis do SLAVE,
	LL_I2C_HandleTransfer(pI2C->I2Cx, dev_address<<1, LL_I2C_ADDRSLAVE_7BIT, (uint32_t)(1 + number_of_data), LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);

	//	Oczekiwanie na zdjęcie flagi TXIS,
	//	Flaga jest zdejmowana przy zapisie rejestru TXDR kolejnymi danymi do wysyłu,
	while(LL_I2C_IsActiveFlag_TXIS(pI2C->I2Cx) == 0);

	LL_I2C_TransmitData8(pI2C->I2Cx, mem_address);

	while(read_data_count > 0)
	{
		//VT100_number(read_data_count);
		//VT100_EOL();
		while(LL_I2C_IsActiveFlag_TXIS(pI2C->I2Cx) == 0);

		LL_I2C_TransmitData8(pI2C->I2Cx, *data);
		data++;
		read_data_count--;
	}
	/////////
	while(LL_I2C_IsActiveFlag_STOP(pI2C->I2Cx) == 0);	//	Oczekiwanie na ustawienie flagi STOP,
	LL_I2C_ClearFlag_STOP(pI2C->I2Cx);

	return 0;
}

uint8_t I2C_read_byte(I2C_t *pI2C, uint8_t dev_address, const uint8_t start_register)
{
	uint8_t data = 0;

	//	************************************************************************
	//	Wysłanie pod adres 'address' jednego bajtu danych z opcją automatycznego generowania STOP, zapis do SLAVE,
	LL_I2C_HandleTransfer(pI2C->I2Cx, dev_address << 1, LL_I2C_ADDRSLAVE_7BIT, (uint32_t)(1), LL_I2C_MODE_SOFTEND, LL_I2C_GENERATE_START_WRITE);

	//	Oczekiwanie na zdjęcie flagi TXIS,
	//	Flaga jest zdejmowana przy zapisie rejestru TXDR kolejnymi danymi do wysyłu,
	while(LL_I2C_IsActiveFlag_TXIS(pI2C->I2Cx) == 0);

	LL_I2C_TransmitData8(pI2C->I2Cx, start_register);

	while(LL_I2C_IsActiveFlag_TC(pI2C->I2Cx) == 0);

	//	************************************************************************
	LL_I2C_HandleTransfer(pI2C->I2Cx, dev_address<<1, LL_I2C_ADDRSLAVE_7BIT, (uint32_t)(1), LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_READ);

	while(LL_I2C_IsActiveFlag_RXNE(pI2C->I2Cx) == 0);

	data = LL_I2C_ReceiveData8(pI2C->I2Cx);

	while(LL_I2C_IsActiveFlag_STOP(pI2C->I2Cx) == 0);	//	Oczekiwanie na ustawienie flagi STOP,
	LL_I2C_ClearFlag_STOP(pI2C->I2Cx);

	return data;
}

uint8_t I2C_read_bytes(I2C_t *pI2C, uint8_t dev_address, const uint8_t mem_address, uint8_t data[], const uint8_t data_size)
{
	uint8_t read_data_count = data_size;

	//	************************************************************************
	//	Wysłanie pod adres 'address' jednego bajtu danych z opcją automatycznego generowania STOP, zapis do SLAVE,
	LL_I2C_HandleTransfer(pI2C->I2Cx, dev_address << 1, LL_I2C_ADDRSLAVE_7BIT, (uint32_t)(1), LL_I2C_MODE_SOFTEND, LL_I2C_GENERATE_START_WRITE);

	//	Oczekiwanie na zdjęcie flagi TXIS,
	//	Flaga jest zdejmowana przy zapisie rejestru TXDR kolejnymi danymi do wysyłu,
	while(LL_I2C_IsActiveFlag_TXIS(pI2C->I2Cx) == 0);

	LL_I2C_TransmitData8(pI2C->I2Cx, mem_address);

	while(LL_I2C_IsActiveFlag_TC(pI2C->I2Cx) == 0);

	//	************************************************************************
	LL_I2C_HandleTransfer(pI2C->I2Cx, dev_address<<1, LL_I2C_ADDRSLAVE_7BIT, (uint32_t)(data_size), LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_READ);

	while(read_data_count > 0)
	{
		while(LL_I2C_IsActiveFlag_RXNE(pI2C->I2Cx) == 0);

		*data = LL_I2C_ReceiveData8(pI2C->I2Cx);
		data++;
		read_data_count--;
	}

	while(LL_I2C_IsActiveFlag_STOP(pI2C->I2Cx) == 0);	//	Oczekiwanie na ustawienie flagi STOP,
	LL_I2C_ClearFlag_STOP(pI2C->I2Cx);

	return 0;
}


//	TODO: wykonać odpalanie poniższych funkcji w menu_LCD
//	wykonać pomiar czasu jaki jest potrzebny do odczytu jednej strony danych czyli 32 bajtów

//	////////////////////////////////////////////////////////////////////////////
void I2C_write_byte_EE(I2C_t *pI2C, uint8_t dev_address, const uint16_t mem_address, const uint8_t data)
{
	//	Wysłanie pod adres 'address' jednego bajtu danych z opcją automatycznego generowania STOP, zapis do SLAVE,
	LL_I2C_HandleTransfer(pI2C->I2Cx, dev_address<<1, LL_I2C_ADDRSLAVE_7BIT, (uint32_t)(2 + 1), LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);

	//	ADDRESS:
	while(LL_I2C_IsActiveFlag_TXIS(pI2C->I2Cx) == 0);
	LL_I2C_TransmitData8(pI2C->I2Cx, (uint8_t)(mem_address>>8));	//	Wysłanie MSB adresu w pamięci EEPROM,

	while(LL_I2C_IsActiveFlag_TXIS(pI2C->I2Cx) == 0);
	LL_I2C_TransmitData8(pI2C->I2Cx, (uint8_t)mem_address);			//	Wysłanie LSB adresu w pamięci EEPROM,

	//	DATA:
	while(LL_I2C_IsActiveFlag_TXIS(pI2C->I2Cx) == 0);		//	Wysłanie danej do zapisu,
	LL_I2C_TransmitData8(pI2C->I2Cx, data);

	//	STOP:
	while(LL_I2C_IsActiveFlag_STOP(pI2C->I2Cx) == 0);		//	Oczekiwanie na ustawienie flagi STOP,
	LL_I2C_ClearFlag_STOP(pI2C->I2Cx);
	delay_ms(5);
}

void I2C_write_bytes_EE(I2C_t *pI2C, uint8_t dev_address, const uint16_t mem_address, const uint8_t data[], const uint8_t data_size)
{
	uint8_t read_data_count = data_size;

	//	Wysłanie pod adres 'address' jednego bajtu danych z opcją automatycznego generowania STOP, zapis do SLAVE,
	LL_I2C_HandleTransfer(pI2C->I2Cx, dev_address<<1, LL_I2C_ADDRSLAVE_7BIT, (uint32_t)(2+data_size), LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);

	//	ADDRESS:
	while(LL_I2C_IsActiveFlag_TXIS(pI2C->I2Cx) == 0);
	LL_I2C_TransmitData8(pI2C->I2Cx, (uint8_t)(mem_address>>8));	//	Wysłanie MSB adresu w pamięci EEPROM,

	while(LL_I2C_IsActiveFlag_TXIS(pI2C->I2Cx) == 0);
	LL_I2C_TransmitData8(pI2C->I2Cx, (uint8_t)mem_address);		//	Wysłanie LSB adresu w pamięci EEPROM,

	//	DATA:
	while(read_data_count > 0)
	{
		while(LL_I2C_IsActiveFlag_TXIS(pI2C->I2Cx) == 0);
		LL_I2C_TransmitData8(pI2C->I2Cx, *data);
		data++;
		read_data_count--;
	}

	//	STOP:
	while(LL_I2C_IsActiveFlag_STOP(pI2C->I2Cx) == 0);	//	Oczekiwanie na ustawienie flagi STOP,
	LL_I2C_ClearFlag_STOP(pI2C->I2Cx);
	delay_ms(5);
}

uint8_t I2C_read_byte_EE(I2C_t *pI2C, uint8_t dev_address, const uint16_t mem_address)
{

	uint8_t data = 0;

	//	************************************************************************
	//	Wysłanie pod adres 'address' jednego bajtu danych z opcją automatycznego generowania STOP, zapis do SLAVE,
	LL_I2C_HandleTransfer(pI2C->I2Cx, (dev_address<<1), LL_I2C_ADDRSLAVE_7BIT, (uint32_t)(2), LL_I2C_MODE_SOFTEND, LL_I2C_GENERATE_START_WRITE);

	//	Oczekiwanie na zdjęcie flagi TXIS,
	//	Flaga jest zdejmowana przy zapisie rejestru TXDR kolejnymi danymi do wysyłu,
	//	ADDRESS:
	while(LL_I2C_IsActiveFlag_TXIS(pI2C->I2Cx) == 0);
	LL_I2C_TransmitData8(pI2C->I2Cx, (mem_address>>8));	//	Wysłanie MSB adresu w pamięci EEPROM,

	while(LL_I2C_IsActiveFlag_TXIS(pI2C->I2Cx) == 0);
	LL_I2C_TransmitData8(pI2C->I2Cx, mem_address);		//	Wysłanie LSB adresu w pamięci EEPROM,

	//	DATA READ:
	while(LL_I2C_IsActiveFlag_TC(pI2C->I2Cx) == 0);

	//	************************************************************************
	LL_I2C_HandleTransfer(pI2C->I2Cx, dev_address<<1, LL_I2C_ADDRSLAVE_7BIT, (uint32_t)(1), LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_READ);

	while(LL_I2C_IsActiveFlag_RXNE(pI2C->I2Cx) == 0);	//	Oczekiwanie na pojawienie się nowych danych,
	data = LL_I2C_ReceiveData8(pI2C->I2Cx);

	//	STOP:
	while(LL_I2C_IsActiveFlag_STOP(pI2C->I2Cx) == 0);	//	Oczekiwanie na ustawienie flagi STOP,
	LL_I2C_ClearFlag_STOP(pI2C->I2Cx);

	return data;
}

uint8_t I2C_read_bytes_EE(I2C_t *pI2C, uint8_t dev_address, const uint16_t mem_address, uint8_t data[], const uint8_t data_size)
{
	//	SEQUENTIAL READ - ODCZYT SEKWENCYJNY:
	//	-rodzaj odczytu danych wzięty z datasheeta,
	//	-jest inicjowany tak jak 'current address read' lub 'random adress read',
	//	-kiedy uC otrzyma słowo danych wysyła potwierdzenie ACK,
	//	-tak długo jak EEPROM otrzymuje potwierdzenie, tak długo zwiększa adres słowa danych
	//	oraz szeregowo tyka??? sekwencyjne słowo danych - wysyła dane z pod zwiększonego adresu,
	//	-kiedy granica pamięci zostanie osiągnięta, to następuje 'roll-over' na pierwszy adres
	//	oraz odczyt sekwencyjny jest kontynuowany,
	//	-operacje sekwencyjne są wyłączane gdy: uC nie odpowie stanem niskim (nie poda ACK) ale
	//	wyśle komendę STOP,
	//	-po prostu jest to odczyt dowolnej ilości danych, które znajdują się pod adresami
	//	w szeregu pamięci,
	//	-taki wielokrotny 'random read' bez ciągłego wysyłania adresu z pod którego mamy wykonać
	//	odczyt,

	uint8_t read_data_count = data_size;

	//	************************************************************************
	//	Wysłanie pod adres 'address' jednego bajtu danych z opcją automatycznego generowania STOP, zapis do SLAVE,
	LL_I2C_HandleTransfer(pI2C->I2Cx, (dev_address<<1), LL_I2C_ADDRSLAVE_7BIT, (uint32_t)(2), LL_I2C_MODE_SOFTEND, LL_I2C_GENERATE_START_WRITE);

	//	Oczekiwanie na zdjęcie flagi TXIS,
	//	Flaga jest zdejmowana przy zapisie rejestru TXDR kolejnymi danymi do wysyłu,
	//	ADDRESS:
	while(LL_I2C_IsActiveFlag_TXIS(pI2C->I2Cx) == 0);
	LL_I2C_TransmitData8(pI2C->I2Cx, (mem_address>>8));	//	Wysłanie MSB adresu w pamięci EEPROM,

	while(LL_I2C_IsActiveFlag_TXIS(pI2C->I2Cx) == 0);
	LL_I2C_TransmitData8(pI2C->I2Cx, mem_address);		//	Wysłanie LSB adresu w pamięci EEPROM,

	while(LL_I2C_IsActiveFlag_TC(pI2C->I2Cx) == 0);

	//	************************************************************************
	LL_I2C_HandleTransfer(pI2C->I2Cx, dev_address<<1, LL_I2C_ADDRSLAVE_7BIT, (uint32_t)(data_size), LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_READ);

	while(read_data_count > 0)
	{
		while(LL_I2C_IsActiveFlag_RXNE(pI2C->I2Cx) == 0);

		*data = LL_I2C_ReceiveData8(pI2C->I2Cx);
		data++;
		read_data_count--;
	}

	//	STOP:
	while(LL_I2C_IsActiveFlag_STOP(pI2C->I2Cx) == 0);	//	Oczekiwanie na ustawienie flagi STOP,
	LL_I2C_ClearFlag_STOP(pI2C->I2Cx);

	return 0;
}


//	############################################################################
uint8_t I2C_read_current_address(I2C_t *pI2C, uint8_t dev_address)
{
	//	CURRENT ADDRESS READ - odczyt z obecnego adresu:
	//	-wewnętrzny licznik adresów słów danych zarządza ostatnim adresem, do kórego był wykonywany
	//	dostęp podczas ostatniego zapisu lub odczytu, zwiększony o jeden,
	//	-ten adres pozostaje aktywny w operacjach tak długo jak zasilanie układu jest zapewnione,
	//	-roll over podczas odczytu zachodzi po przejściu z ostatniej strony, ostatniego bitu na
	//	pierwszą stronę i pierwszy bit,
	//	-roll over podczas zapisu następuje jako przejście z ostatniego bitu na pierwszy bit tej
	//	samej strony,

	//	UWAGI:
	//	-co jeśli zamiast jednego odczytu z pamięci wykonamy odczyt sekwencyjny???
	//	-tak samo jak odczyt sekwencyjny jest rozszerzeniem odczytu losowego,

	uint8_t data = 0;

	LL_I2C_ClearFlag_STOP(pI2C->I2Cx);
	LL_I2C_ClearFlag_NACK(pI2C->I2Cx);

	//	Wysłanie pod adres 'address' jednego bajtu danych z opcją automatycznego generowania STOP, zapis do SLAVE,
	LL_I2C_HandleTransfer(pI2C->I2Cx, dev_address<<1, LL_I2C_ADDRSLAVE_7BIT, (uint32_t)1, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_READ);

	while(LL_I2C_IsActiveFlag_RXNE(pI2C->I2Cx) == 0);
	data = LL_I2C_ReceiveData8(pI2C->I2Cx);

	while(LL_I2C_IsActiveFlag_STOP(pI2C->I2Cx) == 0);	//	Oczekiwanie na ustawienie flagi STOP,
	LL_I2C_ClearFlag_STOP(pI2C->I2Cx);

	return data;
}

uint8_t I2C_read_random(I2C_t *pI2C, uint8_t dev_address, const uint16_t mem_address)
{
	//	RANDOM READ:
	//	-odczyt jednego bajtu danych z podanego adresu,

	return I2C_read_byte_EE(pI2C, dev_address, mem_address);
}

uint8_t I2C_read_sequential(I2C_t *pI2C, uint8_t dev_address, const uint16_t start_register, const uint8_t transfer_size, uint8_t data[])
{
	//	SEQUENTIAL READ - ODCZYT SEKWENCYJNY:
	//	-rodzaj odczytu danych wzięty z datasheeta,
	//	-jest inicjowany tak jak 'current address read' lub 'random adress read',
	//	-kiedy uC otrzyma słowo danych wysyła potwierdzenie ACK,
	//	-tak długo jak EEPROM otrzymuje potwierdzenie, tak długo zwiększa adres słowa danych
	//	oraz szeregowo tyka??? sekwencyjne słowo danych - wysyła dane z pod zwiększonego adresu,
	//	-kiedy granica pamięci zostanie osiągnięta, to następuje 'roll-over' na pierwszy adres
	//	oraz odczyt sekwencyjny jest kontynuowany,
	//	-operacje sekwencyjne są wyłączane gdy: uC nie odpowie stanem niskim (nie poda ACK) ale
	//	wyśle komendę STOP,
	//	-po prostu jest to odczyt dowolnej ilości danych, które znajdują się pod adresami
	//	w szeregu pamięci,
	//	-taki wielokrotny 'random read' bez ciągłego wysyłania adresu z pod którego mamy wykonać
	//	odczyt,

	uint8_t read_data_count = transfer_size;

	//	************************************************************************
	//	Wysłanie pod adres 'address' jednego bajtu danych z opcją automatycznego generowania STOP, zapis do SLAVE,
	LL_I2C_HandleTransfer(pI2C->I2Cx, (dev_address<<1), LL_I2C_ADDRSLAVE_7BIT, (uint32_t)(2), LL_I2C_MODE_SOFTEND, LL_I2C_GENERATE_START_WRITE);

	//	Oczekiwanie na zdjęcie flagi TXIS,
	//	Flaga jest zdejmowana przy zapisie rejestru TXDR kolejnymi danymi do wysyłu,
	while(LL_I2C_IsActiveFlag_TXIS(pI2C->I2Cx) == 0);
	LL_I2C_TransmitData8(pI2C->I2Cx, (start_register>>8));	//	Wysłanie MSB adresu w pamięci EEPROM,

	while(LL_I2C_IsActiveFlag_TXIS(pI2C->I2Cx) == 0);
	LL_I2C_TransmitData8(pI2C->I2Cx, start_register);		//	Wysłanie LSB adresu w pamięci EEPROM,

	while(LL_I2C_IsActiveFlag_TC(pI2C->I2Cx) == 0);

	//	************************************************************************
	LL_I2C_HandleTransfer(pI2C->I2Cx, dev_address<<1, LL_I2C_ADDRSLAVE_7BIT, (uint32_t)(transfer_size), LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_READ);

	while(read_data_count > 0)
	{
		while(LL_I2C_IsActiveFlag_RXNE(pI2C->I2Cx) == 0);

		*data = LL_I2C_ReceiveData8(pI2C->I2Cx);
		data++;
		read_data_count--;
	}

	while(LL_I2C_IsActiveFlag_STOP(pI2C->I2Cx) == 0);	//	Oczekiwanie na ustawienie flagi STOP,
	LL_I2C_ClearFlag_STOP(pI2C->I2Cx);

	return 0;
}


//	////////////////////////////////////////////////////////////////////////////
void I2C_write(I2C_t *pI2C, uint8_t dev_address, const uint16_t mem_address, const uint8_t mem_address_size, uint8_t data[], const uint8_t size)
{
	uint8_t read_data_count = size;

	//	Wysłanie pod adres 'address' jednego bajtu danych z opcją automatycznego generowania STOP, zapis do SLAVE,
	LL_I2C_HandleTransfer(pI2C->I2Cx, dev_address<<1, LL_I2C_ADDRSLAVE_7BIT, (uint32_t)mem_address_size+size, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);

	//	Oczekiwanie na zdjęcie flagi TXIS,
	//	Flaga jest zdejmowana przy zapisie rejestru TXDR kolejnymi danymi do wysyłu,

	if(mem_address_size == 2)
	{
		while(LL_I2C_IsActiveFlag_TXIS(pI2C->I2Cx) == 0);
		LL_I2C_TransmitData8(pI2C->I2Cx, (mem_address>>8));	//	Wysłanie MSB adresu w pamięci EEPROM,
	}

	while(LL_I2C_IsActiveFlag_TXIS(pI2C->I2Cx) == 0);
	LL_I2C_TransmitData8(pI2C->I2Cx, mem_address);			//	Wysłanie LSB adresu w pamięci EEPROM,

	while(read_data_count > 0)
	{
		while(LL_I2C_IsActiveFlag_TXIS(pI2C->I2Cx) == 0);
		LL_I2C_TransmitData8(pI2C->I2Cx, *data);
		data++;
		read_data_count--;
	}

	while(LL_I2C_IsActiveFlag_STOP(pI2C->I2Cx) == 0);	//	Oczekiwanie na ustawienie flagi STOP,
	LL_I2C_ClearFlag_STOP(pI2C->I2Cx);
}

uint8_t I2C_read(I2C_t *pI2C, uint8_t dev_address, const uint16_t mem_address, const uint8_t mem_address_size, uint8_t data[], const uint8_t data_size)
{
	uint8_t read_data_count = data_size;

	//	************************************************************************
	//	Wysłanie pod adres 'address' jednego bajtu danych z opcją automatycznego generowania STOP, zapis do SLAVE,
	LL_I2C_HandleTransfer(pI2C->I2Cx, (dev_address<<1), LL_I2C_ADDRSLAVE_7BIT, (uint32_t)(mem_address_size), LL_I2C_MODE_SOFTEND, LL_I2C_GENERATE_START_WRITE);

	//	Oczekiwanie na zdjęcie flagi TXIS,
	//	Flaga jest zdejmowana przy zapisie rejestru TXDR kolejnymi danymi do wysyłu,
	if(mem_address_size == 2)
	{
		while(LL_I2C_IsActiveFlag_TXIS(pI2C->I2Cx) == 0);
		LL_I2C_TransmitData8(pI2C->I2Cx, (mem_address>>8));	//	Wysłanie MSB adresu w pamięci EEPROM,
	}
	while(LL_I2C_IsActiveFlag_TXIS(pI2C->I2Cx) == 0);
	LL_I2C_TransmitData8(pI2C->I2Cx, mem_address);		//	Wysłanie LSB adresu w pamięci EEPROM,

	while(LL_I2C_IsActiveFlag_TC(pI2C->I2Cx) == 0);

	//	************************************************************************
	LL_I2C_HandleTransfer(pI2C->I2Cx, dev_address<<1, LL_I2C_ADDRSLAVE_7BIT, (uint32_t)(data_size), LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_READ);

	while(read_data_count > 0)
	{
		while(LL_I2C_IsActiveFlag_RXNE(pI2C->I2Cx) == 0);

		*data = LL_I2C_ReceiveData8(pI2C->I2Cx);
		data++;
		read_data_count--;
	}

	while(LL_I2C_IsActiveFlag_STOP(pI2C->I2Cx) == 0);	//	Oczekiwanie na ustawienie flagi STOP,
	LL_I2C_ClearFlag_STOP(pI2C->I2Cx);

	return *(data-1);
}




//	OLD

/*
 *
 *
 *
 *


void I2C_init(I2C_TypeDef *pI2Cx)
{
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C2);

	//	Dla pewności wyłączamy interfejs I2C:
	LL_I2C_Disable(pI2Cx);

	//	Konfiguracja filtrów:
	LL_I2C_ConfigFilters(pI2Cx, LL_I2C_ANALOGFILTER_ENABLE, 0x00);

	//	Wartość timingów:
	LL_I2C_SetTiming(pI2Cx, 0x00202538);	//	Dla 10 [MHz],

	//	Dla trybu master wymagane jest włączenie rozszerzania zegara:
	LL_I2C_EnableClockStretching(pI2Cx);

	//	Wybór komunikacji - I2C:
	LL_I2C_SetMode(pI2Cx, LL_I2C_MODE_I2C);

	//	Włączenie interfejsu po zakończonej konfiguracji:
	LL_I2C_Enable(pI2Cx);
}





void I2C_write_(I2C_TypeDef *I2Cx)
{
	uint8_t dataToSend[5] = {0, 0, 0, 0, 0};
	uint8_t reg_addr = name_arr[4];
	uint8_t *pData = &dataToSend[0];
	uint8_t data_count = 0;

	LL_I2C_HandleTransfer(I2Cx, reg_addr, LL_I2C_ADDRSLAVE_7BIT, 1, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);



	while(LL_I2C_IsActiveFlag_TXIS(I2Cx) == 0);
	LL_I2C_TransmitData8(I2Cx, reg_addr);

	while(data_count > 0)
	{
		while(LL_I2C_IsActiveFlag_TXIS(I2Cx) == 0);
		LL_I2C_TransmitData8(I2Cx, *pData);

		pData++;
		data_count--;
	}

	while(LL_I2C_IsActiveFlag_STOP(I2Cx) == 0);
	LL_I2C_ClearFlag_STOP(I2Cx);
} */




