/*
 * ssd1306.c
 *
 *  Created on: 13 maj 2025
 *      Author: asmnop
 */


#include "../../LIB/Inc/ssd1306.h"
#include "../../LIB/Inc/universal_functions.h"


//	ZAŁOŻENIA:
//	-istnieją dwie osie: x oraz y,
//	-oś x ma kierunek poziomy, przyjmuje wartości od 0 do 127,
//	-oś y ma kierunek pionowy, przyjmuje wartości od 0 do 63,

//	ADRESACJA:
//	-podział pamięcia RAM:
//	-podział na 128 kolumn oraz na 8 stron,
//	-wymiary jednej strony to 128 kolumn bajtów,
//	-wpisując wartość do wyświetlenia, podajemy numer kolejnej kolumny,

//	INFORMACJE:
//	-wykonując zaświecenie dowolnego wybranego piksela musimy albo znać dotychczasową zawartość pola 8-bitowego
//	albo "zapalić" wybrany piksel w tablicy tymczasowej i nastepnie przesłać do pamięcia xxxxRAM zawartość
//	tego pola 8-bitowego,

//	ZAWARTOŚĆ TABLICY:
//	...#...   0x1000 --> 0001000_
//	...#...
//	...#...
//	...#...
//	...#...
//	...#...
//	.......
//	...#...
//	.......
//	.......


//	..#.#..   0x2800 --> 0010100_
//	..#.#..
//	..#.#..
//	.......
//	.......
//	.......
//	.......
//	.......
//	.......
//	.......




static void SSD1306_command(OLED_t *pOLED, const uint8_t command)
{
	I2C_write_bytes(pOLED->I2Cx, pOLED->address, 0x00, &command, 1);
}

static void SSD1306_data(OLED_t *pOLED, const uint8_t data)
{
	I2C_write_bytes(pOLED->I2Cx, pOLED->address, 0x40, &data, 1);
}


void SSD1306_draw_point(OLED_t *pOLED, const uint8_t x, const uint8_t y, const uint8_t on_off)
{
	/*
	if(on_off == 1)
	{
		pOLED->memory_buffer[x][y/8] = pOLED->memory_buffer[x][y/8] | (1 << y%8);
	}
	else
	{
		pOLED->memory_buffer[x][y/8] = pOLED->memory_buffer[x][y/8] & (~(1 << y%8));
	}
*/
	if(on_off == 1)
	{
		pOLED->memory_buffer[y/8][x] = pOLED->memory_buffer[y/8][x] | (1 << y%8);
	}
	else
	{
		pOLED->memory_buffer[y/8][x] = pOLED->memory_buffer[y/8][x] & (~(1 << y%8));
	}
}

void SSD1306_draw_sign(OLED_t *pOLED, const uint8_t x, const uint8_t y, const FONT_t *pFONT, const uint8_t sign, const uint8_t invert)
{
	//const uint8_t LINES_ON_SIGN = 10;	//	Number of lines in sign,
	//const uint8_t COLUMNS_ON_SIGN = 7;	//	Number of columns in sign,
	uint16_t line_from_sign = 0;
	uint16_t bit_from_line = 0;

	for(uint8_t i=0; i<pFONT->height; i++)	//	Read line,
	{
		line_from_sign = pFONT->data[i + pFONT->height * (sign - 32)];

		for(uint8_t j=0; j<pFONT->width; j++)	//	Read bit in line,
		{
			bit_from_line = (line_from_sign << j) & 0x8000;
			if(invert == 1)
			{
				bit_from_line = ~bit_from_line;
			}
			bit_from_line = bit_from_line >> 15;	//	'1' - pixel on, '0' - pixel off,

			SSD1306_draw_point(pOLED, x + j, y + i, bit_from_line);
		}
	}
}

void SSD1306_draw_string(OLED_t *pOLED, const uint8_t x, const uint8_t y, const FONT_t *pFONT, const char string[], const uint8_t invert)
{
	//	-display string,

	uint8_t counter = 0;

	while(*string)
	{
		SSD1306_draw_sign(pOLED, x + (counter*pFONT->width), y, pFONT, *string++, invert);
		counter++;
	}
}

void SSD1306_refresh(OLED_t *pOLED)
{
	SSD1306_command(pOLED, OLED_MEMORY_ADDRESSING_MODE);
	SSD1306_command(pOLED, pOLED->memory_addressing_mode);	//	0x00 - Horizontal, 0x01 - Vertical, 0x02 - Page

	SSD1306_command(pOLED, OLED_SET_COLUMN_ADDR);
	SSD1306_command(pOLED, 0);
	SSD1306_command(pOLED, pOLED->column - 1);

	SSD1306_command(pOLED, OLED_SET_PAGE_ADDR);
	SSD1306_command(pOLED, 0);
	SSD1306_command(pOLED, (pOLED->line / 8) - 1);


	if(pOLED->memory_addressing_mode == OLED_HORIZONTAL_ADDRESSING_MODE)
	{
		for(uint8_t i=0; i<8; i++)
		{
			for(uint8_t j=0; j<128; j++)
			{
				SSD1306_data(pOLED, pOLED->memory_buffer[i][j]);
			}
		}
	}
	else if(pOLED->memory_addressing_mode == OLED_VERTICAL_ADDRESSING_MODE)
	{
		for(uint8_t i=0; i<128; i++)
		{
			for(uint8_t j=0; j<8; j++)
			{
				SSD1306_data(pOLED, pOLED->memory_buffer[j][i]);
			}
		}
	}
}

void SSD1306_clear_display(OLED_t *pOLED)
{
	for(uint8_t i=0; i<pOLED->line/8; i++)
	{
		for(uint8_t j=0; j<pOLED->column; j++)
		{
			pOLED->memory_buffer[i][j] = 0;
		}
	}

	SSD1306_refresh(pOLED);
}


void SSD1306_show_fonts(OLED_t *pOLED, const FONT_t *pFONT)
{
	//	-show on display all signs from font,
	//	-diplay 96 signs,

	#define FIRST_LINE	0

	//	Clear begining lines:
	for(uint8_t i=0; i<FIRST_LINE; i++)
	{
		for(uint8_t j=0; j<pOLED->column; j++)
		{
			SSD1306_draw_point(pOLED, j, i, 0);
		}
	}

	//	Show font:
	for(uint8_t j=0; j<6; j++)
	{
		for(uint8_t i=0; i<16; i++)
		{
			SSD1306_draw_sign(pOLED, i*pFONT->width, FIRST_LINE + j*pFONT->height, pFONT, i + j*(1<<4) + 32, 0);
		}
	}

	#undef FIRST_LINE
}

void SSD1306_toggle(OLED_t *pOLED)
{
	//	-toggle all pixels to display,

	for(uint8_t i=0; i<(pOLED->line / 8); i++)
	{
		for(uint8_t j=0; j<pOLED->column; j++)
		{
			pOLED->memory_buffer[i][j] = ~pOLED->memory_buffer[i][j];
		}
	}
}

void SSD1306_fill_bitmap(OLED_t *pOLED, const uint8_t bitmap[])
{

	if(pOLED->memory_addressing_mode == OLED_HORIZONTAL_ADDRESSING_MODE)
	{
		for(uint8_t i=0; i<8; i++)
		{
			for(uint8_t j=0; j<128; j++)
			{
				pOLED->memory_buffer[i][j] = bitmap[j + i * 128];
			}
		}
	}
	else if(pOLED->memory_addressing_mode == OLED_VERTICAL_ADDRESSING_MODE)
	{
		for(uint8_t i=0; i<128; i++)
		{
			for(uint8_t j=7; j!=255; j--)
			{
				pOLED->memory_buffer[j][i] = bitmap[j + i * 8];
			}
		}
	}
}


void SSD1306_init(OLED_t *pOLED)
{
	SSD1306_command(pOLED, OLED_DISPLAY_OFF);
	SSD1306_command(pOLED, OLED_SET_DISPLAY_CLOCK_DIV_RATIO);
	SSD1306_command(pOLED, 0x80);
	SSD1306_command(pOLED, OLED_SET_MULTIPLEX_RATIO);
	SSD1306_command(pOLED, 0x3F);
	SSD1306_command(pOLED, OLED_SET_DISPLAY_OFFSET);
	SSD1306_command(pOLED, 0x0);
	SSD1306_command(pOLED, OLED_SET_START_LINE | 0x0);
	SSD1306_command(pOLED, OLED_CHARGE_PUMP);
	SSD1306_command(pOLED, 0x14);

	//	Chose memory addressing mode, how data are written to memory,
	SSD1306_command(pOLED, OLED_MEMORY_ADDRESSING_MODE);
	SSD1306_command(pOLED, pOLED->memory_addressing_mode);	//	0x00 - Horizontal, 0x01 - Vertical, 0x02 - Page

	if(pOLED->memory_addressing_mode == OLED_PAGE_ADDRESSING_MODE)
	{
		SSD1306_command(pOLED, 0xB1);
		SSD1306_command(pOLED, 0x00);
		SSD1306_command(pOLED, 0x17);
	}
	else if((pOLED->memory_addressing_mode == OLED_HORIZONTAL_ADDRESSING_MODE) || (pOLED->memory_addressing_mode == OLED_VERTICAL_ADDRESSING_MODE))
	{
		SSD1306_command(pOLED, OLED_SET_COLUMN_ADDR);
		SSD1306_command(pOLED, 0);
		SSD1306_command(pOLED, 127);

		SSD1306_command(pOLED, OLED_SET_PAGE_ADDR);
		SSD1306_command(pOLED, 0);
		SSD1306_command(pOLED, 7);
	}

	SSD1306_command(pOLED, OLED_SET_SEGMENT_REMAP | 0x01);
	SSD1306_command(pOLED, OLED_COM_SCAN_DIR_DEC);
	SSD1306_command(pOLED, OLED_SET_COM_PINS);
	SSD1306_command(pOLED, 0x12); // Jeżeli 128x32 - 0x02

	//	Set contrast, 0 means that nothing is showed,
	SSD1306_command(pOLED, OLED_SET_CONTRAST_CONTROL);	//	OK
	SSD1306_command(pOLED, 200);

	SSD1306_command(pOLED, OLED_SET_PRECHARGE_PERIOD);
	SSD1306_command(pOLED, 0xF1);
	SSD1306_command(pOLED, OLED_SET_VCOM_DESELECT);
	SSD1306_command(pOLED, 0x40);
	SSD1306_command(pOLED, OLED_DISPLAY_ALL_ON_RESUME);
	SSD1306_command(pOLED, OLED_NORMAL_DISPLAY);
	SSD1306_command(pOLED, OLED_DISPLAY_ON);

}

void SSD1306_show_tests(OLED_t *pOLED)
{
	/*
	while(1)
	{
		for(uint8_t i=1; ; i++)
		{
			SSD1306_command(pOLED, OLED_SET_CONTRAST_CONTROL);
			SSD1306_command(pOLED, i);
			delay_ms(10);
		}
	}
*/


	while(1)
	{


		//for(uint8_t j=0; j<4; j++)
		{
			for(uint16_t i=0; i<1024; i++)
			{
				SSD1306_data(pOLED, 0);
				//delay_ms(1);
			}
		}

		SSD1306_draw_point(pOLED, 0, 0, 0);
		SSD1306_draw_point(pOLED, 1, 8, 0);
		SSD1306_draw_point(pOLED, 2, 16, 0);
		SSD1306_draw_point(pOLED, 3, 24, 0);
		SSD1306_draw_point(pOLED, 0, 1, 0);

		for(uint8_t i = 0; i < 64; i = i + 1)
		{
			//SSD1306_draw_point(pOLED, i, i, 1);
		}

		//SSD1306_toggle(pOLED);

		//SSD1306_fill_bitmap(pOLED, github_logo_64x64);

		/*
		for(uint8_t i = 'A'; i < 'A' + 16; i++)
		{
			SSD1306_draw_sign(pOLED, ((i - 'A') << 3), 20, font_7x10, i);
		}


		SSD1306_draw_sign(pOLED, 0, 0, font_7x10, 'A');
		SSD1306_draw_sign(pOLED, 8, 0, font_7x10, 'B');
		SSD1306_draw_sign(pOLED, 16, 0, font_7x10, 'C');
		SSD1306_draw_sign(pOLED, 24, 8, font_7x10, 'M');
		SSD1306_draw_sign(pOLED, 11, 0, font_7x10, 'A');
		SSD1306_draw_sign(pOLED, 21, 0, font_7x10, 'A');
		SSD1306_draw_sign(pOLED, 25, 0, font_7x10, 'A');
		*/
		//SSD1306_draw_sign(pOLED, 20, 20, font_7x10, 2);

		SSD1306_show_fonts(pOLED, &FONT_7x10);
		SSD1306_draw_string(pOLED, 0, 10, &FONT_7x10, "POZDRO 600 ", 1);

		//SSD1306_toggle(pOLED);



		SSD1306_refresh(pOLED);

		while(1);

	}
}

void SSD1306_show_info(OLED_t *pOLED)
{
	SSD1306_draw_string(pOLED, 0, 0, &FONT_16x26, "GPS", 0);
	SSD1306_draw_string(pOLED, 0, 26, &FONT_11x18, "Markowski", 0);
	SSD1306_draw_string(pOLED, 0, 44, &FONT_11x18, "Marek", 0);
	SSD1306_draw_string(pOLED, 60, 44, &FONT_11x18, "asmnop", 0);
	SSD1306_refresh(pOLED);
}

void SSD1306_show_GPS(OLED_t *pOLED)
{
	//	PERMAMENT MENU:
	SSD1306_draw_string(pOLED, FONT_7x10.width * 8, 0, &FONT_7x10, "GPS", 0);
	SSD1306_draw_string(pOLED, 0, 10, &FONT_7x10, "WSPOLRZEDNE GEO:", 0);
	SSD1306_draw_string(pOLED, 0, 20, &FONT_7x10, "DL:", 0);
	SSD1306_draw_string(pOLED, 0, 30, &FONT_7x10, "SZ:", 0);
	SSD1306_draw_string(pOLED, 0, 40, &FONT_7x10, "CZAS UTC :", 0);
	SSD1306_draw_string(pOLED, 0, 50, &FONT_7x10, "STAN LED :", 0);
	SSD1306_refresh(pOLED);
}



/*
void SSD1306_point(OLED_t *pOLED, uint8_t x, uint8_t y, uint8_t p)
{
	SSD1306_command(pOLED, OLED_MEMORY_ADDRESSING_MODE);
	SSD1306_command(pOLED, OLED_HORIZONTAL_ADDRESSING_MODE);	//	0x00 - Horizontal, 0x01 - Vertical, 0x02 - Page

	SSD1306_command(pOLED, OLED_SET_COLUMN_ADDR);
	SSD1306_command(pOLED, x);
	SSD1306_command(pOLED, x);

	SSD1306_command(pOLED, OLED_SET_PAGE_ADDR);
	SSD1306_command(pOLED, y/8);
	SSD1306_command(pOLED, y/8);

	SSD1306_data(pOLED, 1 << y%8);
}

*/













