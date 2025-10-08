/*
 * AT24C32.c
 *
 *  Created on: 14 maj 2024
 *      Author: asmnop
 */


#include "..\..\LIB\Inc\AT24C32.h"

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
uint8_t AT24C32_write_byte(EEPROM_t *pEEPROM, const uint8_t page, const uint8_t byte, uint8_t data)
{
	//	-zapis bajtu danych pod wskazany adres o postaci: numer strony, numer bajtu,
	//	-mamy do dyspozycji: strony - 128, bajty - 32,

	if(page >= pEEPROM->pages)
	{
		return 1;
	}

	if(byte >= pEEPROM->bytes_on_page)
	{
		return 2;
	}

	uint16_t start_register = (page<<5) | byte;

	//	Sprawdzenie jakie dane znajdują się już pod wskazanym adresem:

	if(AT24C32_read_byte(pEEPROM, page, byte) != data)
	{
		I2C_write_byte_EE(pEEPROM->I2Cx, pEEPROM->address, start_register, data);
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

uint8_t AT24C32_read_byte(EEPROM_t *pEEPROM, const uint8_t page, const uint8_t byte)
{
	//	-odczyt bajtu danych z pod wskazanego adresu: numer strony, numer bajtu,
	//	-mamy do dyspozycji: strony - 128, bajty - 32,

	uint16_t address = (page<<5) | byte;

	return I2C_read_random(pEEPROM->I2Cx, pEEPROM->address, address);
}

void AT24C32_write_page(EEPROM_t *pEEPROM, const uint8_t page, uint8_t data[])
{
	//	-zapis całej strony danych,
	//	-zapis następuje bez względu na zawartość zapisywanej strony,

	uint16_t start_register = (page << 5);

	uint8_t data_arr[AT24C32_BYTES_ON_PAGE] = {0};

	for(uint8_t i=0; i<pEEPROM->bytes_on_page; ++i)
	{
		data_arr[i] = i;
	}

	//I2C_write_page(I2C_t *pI2C, uint8_t address, const uint16_t start_register, const uint8_t numberOfData, const uint8_t data[]);
	I2C_write_page(pEEPROM->I2Cx, pEEPROM->address, start_register, pEEPROM->bytes_on_page, data_arr);

	//HAL_StatusTypeDef status = HAL_I2C_Mem_Write(&hi2c1, (AT24C32_ADDR<<1), address, 2, data, AT24C32_BYTES_ON_PAGE, HAL_MAX_DELAY);
	//HAL_Delay(5);


}

void AT24C32_read_page(EEPROM_t *pEEPROM, const uint8_t page)
{
	//	-odczyt zawartości całej strony czyli 32 bajty,

	uint8_t data[AT24C32_BYTES_ON_PAGE];

	uint16_t address = (page << 5);

	I2C_read_sequential(pEEPROM->I2Cx, pEEPROM->address, address, AT24C32_BYTES_ON_PAGE, data);


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

void AT24C32_fill_page(EEPROM_t *pEEPROM, const uint8_t page, const char sign)
{
	//	-wypełnienie jednej strony wybranymi znakami,

	uint8_t data[AT24C32_BYTES_ON_PAGE];

	for(uint8_t i=0; i<AT24C32_BYTES_ON_PAGE; i++)
	{
		data[i] = sign;
	}

	AT24C32_write_page(pEEPROM, page, data);
}

void AT24C32_erase_page(EEPROM_t *pEEPROM, const uint8_t page)
{
	//	-zapis całej strony pamięci znakami symbolizującymi jej wyczyszczenie,

	AT24C32_fill_page(pEEPROM, page, 255);
}

void AT24C32_erase_memory(EEPROM_t *pEEPROM)
{
	uint8_t data[AT24C32_BYTES_ON_PAGE];

	for(uint8_t i=0; i<AT24C32_BYTES_ON_PAGE; i++)
	{
		data[i] = 255;
	}

	for(uint8_t i=0; i<AT24C32_BYTES_ON_PAGE; i++)
	{
		AT24C32_write_page(pEEPROM, i, data);
	}
}


/*
void AT24C32_read_page(const uint8_t page)
{
	//	-odczyt zawartości całej strony czyli 32 bajty,

	uint8_t data[AT24C32_BYTES_ON_PAGE];
	uint16_t address = (page << 5);

	HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c1, (AT24C32_ADDR<<1), address, 2, data, AT24C32_BYTES_ON_PAGE, HAL_MAX_DELAY);
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

}

*/

/*
//	############################################################################
void foo_EEPROM_read_all_pages(void)
{
	uint8_t data;

	for(uint8_t j=0; j<EEPROM_1.pages; ++j)
	{
		VT100_text("ZAWARTOSC STRONY NUMER ");
		VT100_number(j);
		VT100_text(":\n\r");

		for(uint8_t i=0; i<EEPROM_1.bytes_on_page; ++i)
		{
			VT100_number(i+1);
			VT100_text(" --> ");
			data = AT24C32_read_byte(&EEPROM_1, j, i);
			VT100_number(data);
			VT100_text("\n\r");
		}

		VT100_text("\n\r");
	}
}

void foo_EEPROM_read_page_terminal(void)
{
	uint8_t data = 0;
	uint8_t page_number = 0;

	menu_entry(&LCD_1, "EEPROM ODCZYT");

	VT100_EOL();
	VT100_EOL();
	VT100_text("PODAJ NUMER STRONY:");
	VT100_EOL();
	page_number = VT100_create_number();

	VT100_EOL();
	VT100_EOL();
	VT100_text("ZAWARTOSC STRONY NUMER: ");
	VT100_number(page_number);
	VT100_EOL();

	for(uint8_t i=0; i<EEPROM_1.bytes_on_page; ++i)
	{
		VT100_number(i+1);
		VT100_text(" --> ");
		data = AT24C32_read_byte(&EEPROM_1, page_number, i);
		VT100_number(data);
		VT100_EOL();
	}


	menu_update(&LCD_1);
}

void foo_EEPROM_read_page(void)
{
	//	-odczyt zawartości wybranej strony z pamięci układu EEPROM oraz możliwość jej przeglądania,

	uint8_t key_state = 0;				//	Zmienna pomocnicza do przechowywania odczytanego stanu na przyciskach,
	uint8_t byte_counter = 0;			//	Licznik bajtów,
	static uint8_t page_counter = 0;	//	Licznik stron,

	menu_entry(&LCD_1, "EEPROM");

	//	WYBÓR STRONY DO WYŚWIETLENIA:
	LCD_clear_display(&LCD_1);
	LCD_buf_locate(&LCD_1, 0, 0);
	LCD_text(&LCD_1, "NUMER STRONY:");

	LCD_buf_locate(&LCD_1, 1, 0);
	LCD_number(&LCD_1, page_counter, DEC);
	LCD_refresh(&LCD_1);

	do
	{
		//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//	ZWIĘKSZENIE NUMERU STRONY DO WYŚWIETLENIA:
		//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		key_state = key_pressed(&CHILD, 500, 500, NULL, NULL);
		if(key_state == 2)
		{
			if(page_counter != EEPROM_1.pages - 1)
			{
				page_counter++;
				LCD_buf_locate(&LCD_1, 1, 0);
				LCD_text(&LCD_1, "   ");
				LCD_buf_locate(&LCD_1, 1, 0);
				LCD_number(&LCD_1, page_counter, DEC);
				LCD_refresh(&LCD_1);
			}
		}

		//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//	ZMNIEJSZENIE NUMERU STRONY DO WYŚWIETLENIA:
		//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		key_state = key_pressed(&PARENT, 500, 500, NULL, NULL);
		if(key_state == 2)
		{
			if(page_counter != 0)
			{
				page_counter--;
				LCD_buf_locate(&LCD_1, 1, 0);
				LCD_text(&LCD_1, "   ");
				LCD_buf_locate(&LCD_1, 1, 0);
				LCD_number(&LCD_1, page_counter, DEC);
				LCD_refresh(&LCD_1);
			}
		}

		//	Warunki wyjścia z funkcji:
		key_pressed(&NEXT, 1000, 0, NULL, menu_exit);
	}while(flag_exit == 0);

	flag_exit = 0;


	//	################################################################################################################
	//	PIERWSZE WYŚWIETLENIE ZAWARTOŚCI STRONY:
	uint8_t data = 0;

	LCD_clear_display(&LCD_1);

	for(uint8_t i=0; i<4; i++)
	{
		LCD_buf_locate(&LCD_1, i, 0);
		LCD_number(&LCD_1, byte_counter+i, DEC);
		LCD_buf_locate(&LCD_1, i, 5);
		data = AT24C32_read_byte(&EEPROM_1, page_counter, byte_counter+i);
		LCD_number(&LCD_1, data, DEC);
	}

	LCD_buf_locate(&LCD_1, 0, 10);
	LCD_text(&LCD_1, "P");
	LCD_buf_locate(&LCD_1, 1, 10);
	LCD_text(&LCD_1, "A");
	LCD_buf_locate(&LCD_1, 2, 10);
	LCD_text(&LCD_1, "G");
	LCD_buf_locate(&LCD_1, 3, 10);
	LCD_text(&LCD_1, "E");

	LCD_buf_locate(&LCD_1, 1, 12);
	LCD_number(&LCD_1, page_counter, DEC);
	LCD_refresh(&LCD_1);

	while(key_pressed(&NEXT, 100, 100, NULL, NULL));


	do
	{
		//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//	ZWIĘKSZENIE NUMERÓW BAJTÓW DO WYŚWIETLENIA:
		//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		key_state = key_pressed(&CHILD, 500, 500, NULL, NULL);
		if(key_state == 2)
		{
			if(byte_counter != EEPROM_1.bytes_on_page-4)
			{
				byte_counter = byte_counter + 4;
				LCD_clear_display(&LCD_1);

				for(uint8_t k=0; k<4; k++)
				{
					uint8_t temp = byte_counter+k;
					LCD_buf_locate(&LCD_1, k, 0);
					LCD_number(&LCD_1, temp, DEC);
					LCD_buf_locate(&LCD_1, k, 5);
					data = AT24C32_read_byte(&EEPROM_1, page_counter, temp);
					LCD_number(&LCD_1, data, DEC);
				}

				LCD_buf_locate(&LCD_1, 0, 10);
				LCD_text(&LCD_1, "P");
				LCD_buf_locate(&LCD_1, 1, 10);
				LCD_text(&LCD_1, "A");
				LCD_buf_locate(&LCD_1, 2, 10);
				LCD_text(&LCD_1, "G");
				LCD_buf_locate(&LCD_1, 3, 10);
				LCD_text(&LCD_1, "E");

				LCD_buf_locate(&LCD_1, 1, 12);
				LCD_number(&LCD_1, page_counter, DEC);
				LCD_refresh(&LCD_1);
			}
		}

		//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//	ZMNIEJSZENIE NUMERÓW BAJTÓW DO WYŚWIETLENIA:
		//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		key_state = key_pressed(&PARENT, 500, 500, NULL, NULL);
		if(key_state == 2)
		{
			if(byte_counter != 0)
			{
				byte_counter = byte_counter - 4;
				LCD_clear_display(&LCD_1);

				for(uint8_t i=0; i<4; i++)
				{
					uint8_t temp = byte_counter+i;
					LCD_buf_locate(&LCD_1, i, 0);
					LCD_number(&LCD_1, temp, DEC);
					LCD_buf_locate(&LCD_1, i, 5);
					data = AT24C32_read_byte(&EEPROM_1, page_counter, temp);
					LCD_number(&LCD_1, data, DEC);
				}

				LCD_buf_locate(&LCD_1, 0, 10);
				LCD_text(&LCD_1, "P");
				LCD_buf_locate(&LCD_1, 1, 10);
				LCD_text(&LCD_1, "A");
				LCD_buf_locate(&LCD_1, 2, 10);
				LCD_text(&LCD_1, "G");
				LCD_buf_locate(&LCD_1, 3, 10);
				LCD_text(&LCD_1, "E");

				LCD_buf_locate(&LCD_1, 1, 12);
				LCD_number(&LCD_1, page_counter, DEC);
				LCD_refresh(&LCD_1);
			}
		}

		//	Warunki wyjścia z funkcji:
		key_pressed(&PREV, 1000, 0, NULL, menu_exit);
	}while(flag_exit == 0);

	menu_end(&LCD_1, "KONIEC EEPROM");
}

void foo_EEPROM_write_byte_terminal(void)
{
	//	-zapis danej na wybraną stronę i pozycję,

	uint8_t page_number = 0;
	uint8_t byte_number = 0;
	uint8_t data = 0;

	menu_entry(&LCD_1, "EEPROM ZAPIS");

	//	########################################################################
	VT100_EOL();
	VT100_text("PODAJ NUMER STRONY:");
	VT100_EOL();
	page_number = VT100_create_number();
	VT100_EOL();

	for(uint8_t i=0; i<EEPROM_1.bytes_on_page; ++i)
	{
		VT100_number(i+1);
		VT100_text(" --> ");
		data = AT24C32_read_byte(&EEPROM_1, page_number, i);
		VT100_number(data);
		VT100_EOL();
	}

	//	########################################################################
	VT100_EOL();
	VT100_text("PODAJ NUMER BAJTU:");
	VT100_EOL();
	byte_number = VT100_create_number();
	VT100_EOL();

	//	########################################################################
	VT100_EOL();
	VT100_text("PODAJ WARTOSC DO ZAPISU:");
	VT100_EOL();
	data = VT100_create_number();
	VT100_EOL();

	AT24C32_write_byte(&EEPROM_1, page_number, byte_number, data);

	menu_update(&LCD_1);
}

*/




