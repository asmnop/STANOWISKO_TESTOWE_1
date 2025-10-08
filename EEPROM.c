/*
 * EEPROM.c
 *
 *  Created on: 14 maj 2024
 *      Author: asmnop
 */


#include "..\..\LIB\Inc\EEPROM.h"

#include "..\..\LIB\Inc\i2c.h"
#include "..\..\LIB\Inc\VT100.h"


//	INFORMACJE:
//	-adres układu: 1010_A2_A1_A0_R/W,
//	-wartości bitów A2, A1 i A0 są ustawiane poprzez zwarcie pinów do Vcc (jedynka) lub połączenie z masą (zero),
//	-jeśli piny nie zostaną podłączone jest to przyjmowane jako stan niski, zero,
//	-można za pomocą pinów A2, A1 i A0 podłączyć do ośmiu układów pamięci,
//	-pamięć układu AT24C32 to 32K - 32 kilo bity = 32 768 bitów = 4 096 bajtów, (4096 x 8),
//	-pamięć jest zorganizowana w 128 stron po 32 bajty każda,
//	-dane są wysyłane od MSB do LSB,

//	WNIOSKI Z OBSŁUGI PROGRAMU:
//	-przeprogramowywanie uC lub reset uC nie powodują zmiany wewnętrznego licznika adresu słowa danych,
//	-powyższe nie zachodzi ponieważ zasilanie nie jest wtedy odłączane,
//	-numery stron zawierają się w zakresie od 0 do 127,

//	ORGANIZACJA PAMIĘCI:
//	-adres w pamięci układu składa się z dwóch części, numeru strony oraz numeru bajtu,
//	-numer bajtu - B - zajmuje 5 najmniej znaczących bitów,
//	-numer strony  - P - zajmuje 7 kolejnych bitów,
//	0bxxxx_PPPP 0bPPP_BBBBB,
//	-bit 0bxxxx_PPPP jest przesyłany jako pierwszy,
//	-bit 0bPPP_BBBBB jest przesyłany jako drugi,
//	-powyższe informacje trzeba mieć na uwadzę używając funckji:
//	LL_I2C_HandleTransfer(I2C_TypeDef *I2Cx, uint32_t SlaveAddr, uint32_t SlaveAddrSize, uint32_t TransferSize, uint32_t EndMode, uint32_t Request)
//	gdzie pod argumentem 'TransferSize' musimy wpisać wartość '2' i następnie przesłać MSB adresu w EEPROM, a
//	następnie przesłać LSB adresu w EEPROM,
//	-'TransferSize' określa całkowitą ilość bajtów do wysłania z parametrami zdefiniowanymi w funkcji czyli np: adres slave, MSB adresu,
//	LSB adresu oraz ilość danych,



//	ZAPIS DANYCH:
//	-należy pamiętać, że autoinkrementacja ma miejsce tylko dla 5 dolnych bitów adresu,
//	-z tego powodu po osiągnięciu granicy strony nie nastąpi przejście na kolejną tylko
//	zostanie nadpisana strona obecna,


//	ODCZYT DANYCH:
//	-podczas odczytu danych występuje tzw. przepełnienie czyli jeśli zaczniemy np. odczytywać 128
//	stron dla przykładu od strony 67, to po osiągnięciu odczytu ostatniego bajtu na stronie 127,
//	nastąpi przejście do odczytu bajtu zerowego na stronie zerowej,


//	KASOWANIE DANYCH:
//	-puste pole po wykonaniu kasowania strony zawiera wartości 0xFF, 255,


//	ANALOGIA DO DZIALANIA:
//	-pamięć EEPROM to książka, zeszyt, która ma określoną ilość stron, a na każdej stronie znajduje się
//	określona ilość liter, które mogą się tam zmieścić,
//	-w naszym przypadku zeszyt ma 128 stron, a na każdej stronie mieszczą się 32 znaki, każdy znak
//	może być zapisany na 256 różnych sposobów,


//	############################################################################
uint8_t EEPROM_write_byte(EEPROM_t_new *pEEPROM, const uint16_t mem_address, uint8_t data)
{
	//	-zapis bajtu danych pod wskazany adres o postaci: numer strony, numer bajtu,
	//	-mamy do dyspozycji: strony - 128, bajty - 32,

	if(mem_address >= pEEPROM->address_register)
	{
		return 1;
	}

	//	Sprawdzenie jakie dane znajdują się już pod wskazanym adresem:

	if(I2C_read_byte_EE(pEEPROM->I2Cx, pEEPROM->dev_address, mem_address) != data)
	{
		I2C_write_byte_EE(pEEPROM->I2Cx, pEEPROM->dev_address, mem_address, data);
		VT100_text("DOKONANO ZAPISU");
		VT100_EOL();

		return 0;
	}
	else
	{
		VT100_text("ZMIANA NIE JEST KONIECZNA");
		VT100_EOL();

		return 3;
	}
}

uint8_t EEPROM_read_byte(EEPROM_t_new *pEEPROM, const uint16_t mem_address)
{
	//	-odczyt bajtu danych z pod wskazanego adresu: numer strony, numer bajtu,
	//	-mamy do dyspozycji: strony - 128, bajty - 32,

	return I2C_read_byte_EE(pEEPROM->I2Cx, pEEPROM->dev_address, mem_address);
}

void EEPROM_write_page(EEPROM_t_new *pEEPROM, const uint8_t page, uint8_t data[])
{
	//	-zapis całej strony danych,
	//	-zapis następuje bez względu na zawartość zapisywanej strony,

	uint16_t mem_address = (page << 5);

	I2C_write_bytes_EE(pEEPROM->I2Cx, pEEPROM->dev_address, mem_address, data, pEEPROM->bytes_on_page);
}

void EEPROM_read_page(EEPROM_t_new *pEEPROM, const uint8_t page)
{
	//	-odczyt zawartości całej strony czyli 32 bajty,

	uint8_t data[AT24C32_BYTES_ON_PAGE];

	uint16_t mem_address = (page << 5);

	I2C_read_bytes_EE(pEEPROM->I2Cx, pEEPROM->dev_address, mem_address, data, pEEPROM->bytes_on_page);
}

void EEPROM_fill_page(EEPROM_t_new *pEEPROM, const uint8_t page, const char sign)
{
	//	-wypełnienie jednej strony wybranymi znakami,

	uint16_t mem_address = (page << 5);
	uint8_t data[AT24C32_BYTES_ON_PAGE];

	for(uint8_t i=0; i<pEEPROM->bytes_on_page; i++)
	{
		data[i] = sign;
	}

	I2C_write_bytes_EE(pEEPROM->I2Cx, pEEPROM->dev_address, mem_address, data, pEEPROM->bytes_on_page);
}

void EEPROM_erase_page(EEPROM_t_new *pEEPROM, const uint8_t page)
{
	//	-zapis całej strony pamięci znakami symbolizującymi jej wyczyszczenie,

	EEPROM_fill_page(pEEPROM, page, 255);
}

void EEPROM_erase_memory(EEPROM_t_new *pEEPROM)
{
	uint8_t data[AT24C32_BYTES_ON_PAGE];

	for(uint8_t i=0; i<pEEPROM->bytes_on_page; i++)
	{
		data[i] = 255;
	}

	for(uint8_t i=0; i<pEEPROM->bytes_on_page; i++)
	{
		EEPROM_write_page(pEEPROM, i, data);
	}
}


//	############################################################################
uint8_t EEPROM_write(EEPROM_t_new *pEEPROM, const uint16_t mem_address, uint8_t data[], const uint8_t data_size)
{
	//	-zapis bajtu danych pod wskazany adres o postaci: numer strony, numer bajtu,
	//	-mamy do dyspozycji: strony - 128, bajty - 32,

	if(mem_address >= pEEPROM->address_register)
	{
		return 1;
	}

	//	Sprawdzenie jakie dane znajdują się już pod wskazanym adresem:

	for(uint8_t i=0; i<data_size; i++)
	{
		if(I2C_read(pEEPROM->I2Cx, pEEPROM->dev_address, mem_address, 2, data+i, 1) != *(data+i))
		{
			I2C_write(pEEPROM->I2Cx, pEEPROM->dev_address, mem_address, 2, data+i, 1);
			VT100_text("DOKONANO ZAPISU");
			VT100_EOL();
		}
		else
		{
			VT100_text("ZMIANA NIE JEST KONIECZNA");
			VT100_EOL();
		}
	}

	return 0;
}

uint8_t EEPROM_read(EEPROM_t_new *pEEPROM, const uint16_t mem_address, uint8_t data[], const uint8_t data_size)
{
	//	-odczyt zawartości całej strony czyli 32 bajty,

	I2C_read(pEEPROM->I2Cx, pEEPROM->dev_address, mem_address, 2, data, data_size);

	return *data;

	/*
	if(status == HAL_ERROR)
	{
		printf("Blad w funkcji 'AT24C32_read_page'\n");
	}

	printf("Zawartosc strony nr: %d\n", page);
	for(uint8_t i=0; i<4; i++)
	{
		printf("%4d %4d %4d %4d %4d %4d %4d %4d\n", data[i*8], data[1+i*8], data[2+i*8], data[3+i*8], data[4+i*8], data[5+i*8], data[6+i*8], data[7+i*8]);
	}


	printf("Zawartosc strony nr: %d\n", page);
	for(uint8_t i=0; i<32; i++)
	{
		printf("Bit nr %d: %d\n", i, data[i]);
	}
*/
}








