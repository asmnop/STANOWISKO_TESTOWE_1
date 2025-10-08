/*
 * sd.c
 *
 *  Created on: 25 maj 2025
 *      Author: asmnop
 */


#include "../../LIB/Inc/sd.h"
#include "../../LIB/Inc/universal_functions.h"
#include "../../LIB/Inc/VT100.h"

//#include "../FatFs/ff.h"
//#include "ff.h"

#define DUMMY_BYTE	0xFF


//	https://elm-chan.org/docs/mmc/mmc_e.html
//	https://electronics.stackexchange.com/questions/602105/how-can-i-initialize-use-sd-cards-with-spi
//	https://www.sdcard.org/downloads/pls/
//	http://www.rjhcoding.com/avrc-sd-interface-1.php

static void SD_CS_active(const SD_t *pSD)
{
	SPI_send(DUMMY_BYTE);
	GPIO_reset_pin(pSD->pLINE);
	SPI_send(DUMMY_BYTE);
}

static void SD_CS_inactive(const SD_t *pSD)
{
	SPI_send(DUMMY_BYTE);
	GPIO_set_pin(pSD->pLINE);
	SPI_send(DUMMY_BYTE);
}


void SD_power_up(SD_t *pSD)
{
	GPIO_set_pin(pSD->pLINE);

	delay_ms(1);

	//LL_SPI_Enable(SPI1);

	for(uint8_t i=0; i<10; i++)
	{
		SPI_send(0xFF);
	}

	GPIO_set_pin(pSD->pLINE);
	SPI_send(0xFF);

	//LL_SPI_Disable(SPI1);

}


void SD_command(uint8_t cmd, uint32_t arg, uint8_t crc)
{
	SPI_send(cmd | 0x40);
	SPI_send((uint8_t)(arg >> 24));
	SPI_send((uint8_t)(arg >> 16));
	SPI_send((uint8_t)(arg >> 8));
	SPI_send((uint8_t)(arg));
	SPI_send(crc | 0x10);
}


uint8_t SD_R1(SD_t *pSD)
{
	//	-R1 response,
	//	-if R1 == 0xFF it means that no card has been detected,

	uint8_t data_received = 0xFF;

	for(uint8_t i=0; i<8; i++)
	{
		if((data_received = SPI_receive()) != 0xFF)
		{
			pSD->R1 = data_received;
			return data_received;
		}
	}

	pSD->R1 = data_received;
	return data_received;
}

void SD_R7(SD_t *pSD)
{
	//	-R7 response,
	//	-5 byte response,

	pSD->R1 = SD_R1(pSD);

	if(pSD->R1 > 1)
	{
		//return;
	}

	pSD->R7[0] = SPI_receive();
	pSD->R7[1] = SPI_receive();
	pSD->R7[2] = SPI_receive();
	pSD->R7[3] = SPI_receive();
}


void SD_send_CMD0(SD_t *pSD)
{
	SD_CS_active(pSD);

	SD_command(CMD0, CMD0_ARG, CMD0_CRC);
	SD_R1(pSD);

	SD_CS_inactive(pSD);
}

void SD_send_CMD1(SD_t *pSD)
{
	SD_CS_active(pSD);

	SD_command(CMD1, CMD1_ARG, CMD1_CRC);
	SD_R1(pSD);

	SD_CS_inactive(pSD);
}

void SD_send_CMD8(SD_t *pSD)
{
	SD_CS_active(pSD);

	SD_command(CMD8, CMD8_ARG, CMD8_CRC);
	SD_R7(pSD);

	SD_CS_inactive(pSD);
}

void SD_send_ACMD41(SD_t *pSD)
{
	SD_CS_active(pSD);

	SD_command(ACMD41, ACMD41_ARG, ACMD41_CRC);
	SD_R1(pSD);

	SD_CS_inactive(pSD);
}

void SD_send_CMD55(SD_t *pSD)
{
	SD_CS_active(pSD);

	SD_command(CMD55, CMD55_ARG, CMD55_CRC);
	SD_R1(pSD);

	SD_CS_inactive(pSD);
}

void SD_send_CMD58(SD_t *pSD)
{
	SD_CS_active(pSD);

	SD_command(CMD58, CMD58_ARG, CMD58_CRC);
	SD_R7(pSD);

	SD_CS_inactive(pSD);
}


void SD_init(SD_t *pSD)
{
	//	-SD card initialisation to work in SPI mode,

	SPI_init();
	SD_power_up(pSD);

	do
	{
		SD_send_CMD0(pSD);
		VT100_text("SD - CMD0:    ");
		VT100_number(pSD->R1);
		VT100_text("\n\r");
	}while(pSD->R1 != 0x01);

	SPI_set_speed(LL_SPI_BAUDRATEPRESCALER_DIV256);

	SD_send_CMD8(pSD);
	VT100_text("SD - CMD8:    ");
	VT100_number(pSD->R1);
	VT100_text("\n\r");

	uint8_t counter = 0;

	do
	{
		SD_send_CMD55(pSD);
		VT100_text("SD - CMD55:   ");
		VT100_number(pSD->R1);
		VT100_text("\n\r");

		SD_send_ACMD41(pSD);
		VT100_text("SD - ACMD41:  ");
		VT100_number(pSD->R1);
		VT100_text("\n\r");
		counter++;

	}while(pSD->R1 != 0 && counter != 100);



	do
	{
		SD_send_CMD58(pSD);
		VT100_text("SD - CMD58:   ");
		VT100_number(pSD->R1);

		for(uint8_t i=0; i<4; i++)
		{
			VT100_text("     ");
			VT100_number(pSD->R7[i]);
		}


		VT100_text("\n\r");
	}while(((pSD->R7[0]>>7) != 1));


	//while(1);


	//	WRITE DATA TO SD CARD:
	uint16_t sector_number = 524;

	for(uint16_t i=0; i<SD_BLOCK_LENGHT; i++)
	{
		pSD->buf[i] = '1';
	}

	SD_send_CMD24(pSD, sector_number, pSD->buf);

	//	READ DATA FROM CARD:
	for(uint16_t i=0; i<SD_BLOCK_LENGHT; i++)
	{
		pSD->buf[i] = '0';
	}

	SD_send_CMD17(pSD, sector_number, pSD->buf);

	if(pSD->token == SD_START_TOKEN)
	{
		for(uint16_t i=0; i<SD_BLOCK_LENGHT + 2; i++)
		{
			VT100_text("  ");
			VT100_number(pSD->buf[i]);

			if((i + 1)%16 == 0)
			{
				VT100_text("\n\r");
			}
		}
	}




}





void SD_show_R1(SD_t *pSD, TERMINAL_t *pTERMINAL, const uint8_t line, const uint8_t row_1, const uint8_t row_2)
{
	TERMINAL_line(pTERMINAL, line, row_1, "SD - R1");

	TERMINAL_number(pTERMINAL, line, row_2, pSD->R1, 4);
}


void SD_show_OCR(SD_t *pSD, TERMINAL_t *pTERMINAL, const uint8_t line, const uint8_t row_1, const uint8_t row_2)
{

	TERMINAL_line(pTERMINAL, line, row_1, "SD - CMD58 - OCR");

	for(uint8_t i=0; i<4; i++)
	{
		TERMINAL_number(pTERMINAL, line, row_2 + i * 4, pSD->R7[i], 4);
	}

	if((pSD->R7[0]>>7) == 1)
	{
		TERMINAL_line(pTERMINAL, line + 1, row_1, "CARD READY");

		if(((pSD->R7[0] & 0b01000000)>>6) == 1)
		{
			TERMINAL_line(pTERMINAL, line + 1, row_1 + 20, "HC or EC");
		}
		else
		{
			TERMINAL_line(pTERMINAL, line + 1, row_1 + 20, "STANDARD");
		}
	}
	else
	{
		TERMINAL_line(pTERMINAL, line + 1, row_1, " NOT READY                     ");
	}



}

uint8_t SD_send_CMD17(SD_t *pSD, const uint32_t sector, uint8_t buf[])
{
	//	-odczyt jednego bloku danych, wielkość bloku to domyślnie 512 bajtów,
	//	-funkcja zwraca wartości: 0xFE --> odczyt prawidłowy,
	//	-0x0X --> błąd danych,
	//	-0xFF --> przekroczono czas,

	uint8_t read;

	pSD->token = 0xFF;

	SD_CS_active(pSD);

	SD_command(CMD17, sector<<9, CMD17_CRC);
	SD_R1(pSD);

	if(pSD->R1 != 0xFF)
	{
		// wait for a response token (timeout = 100ms)
		/*
		readAttempts = 0;
		while(++readAttempts != 1953)
			if((read = SPI_send(0xFF)) != 0xFF)
			{
				break;
			}
		*/

		while((read = SPI_send(0xFF)) == 0xFF);

		if(read == SD_START_TOKEN)
		{
			//VT100_text("TOKEN OK: ");
			//VT100_number(read);
			//VT100_text("\n\r");

			for(uint16_t i=0; i<SD_BLOCK_LENGHT; i++)
			{
				buf[i] = SPI_send(0xFF);
			}

			// read 16-bit CRC
			SPI_send(0xFF);
			SPI_send(0xFF);
		}
		else
		{
			//VT100_text("BLEDNY TOKEN: ");
			//VT100_number(read);
			//VT100_text("\n\r");
		}

		pSD->token = read;
	}

	SD_CS_inactive(pSD);

	return pSD->R1;
}


void SD_send_CMD24(SD_t *pSD, uint32_t addr, uint8_t *buf)
{
	//	-funkcja do zapisu danych do pamięci karty SD,
	//	-zapis odbywa się za pomocą 512-bajtowych bloków,
	//	-kolejność działań:
	//	-wysył komendy CMD24
	//	-odczekanie na odbiór bajtu R1 w postaci 0x00,
	//	-wysył tokena startu wysyłu bloków - 0xFE,
	//	-wysył bloku danych - 512 bajtów,
	//	-oczekiwanie na token odbioru w postaci 0bxxx0___1,
	//	-gdzie ___
	//	-jeśli karta zaakceptuje dane to otrzymamy w odpowiedzi 0bxxx00101,
	//	-po tym karta wchodzi w stan zajętości wysyłając token 0x00 aż do momentu zakończenia operacji,

	addr = addr<<9;

	SD_CS_active(pSD);

	SD_command(CMD24, addr, CMD24_CRC);
	SD_R1(pSD);

	if(pSD->R1 == 0)
	{
		SPI_send(SD_START_TOKEN);

		for(uint16_t i=0; i<SD_BLOCK_LENGHT; i++)
		{
			SPI_send(pSD->buf[i]);
		}

		// wait for a response token (timeout = 100ms)
		uint32_t readAttempts = 0;
		uint32_t read = 0;

		while(++readAttempts != 98000)
			if((read = SPI_send(0xFF)) != 0xFF) break;

		if((read & 0x1F) == 0x05)
		{
			pSD->token = 0x05;

			// wait for write to finish (timeout = 250ms)
			readAttempts = 0;

			while(SPI_send(0xFF) == 0x00)
			{
				if(++readAttempts == SD_MAX_WRITE_ATTEMPTS)
				{
					pSD->token = 0x00;
					break;
				}
			};
		}
	}

	SD_CS_inactive(pSD);
}


void SD_inc_sector(SD_t *pSD, const uint32_t value)
{
	pSD->sector_to_read = pSD->sector_to_read + value;
}

void SD_dec_sector(SD_t *pSD, const uint32_t value)
{
	pSD->sector_to_read = pSD->sector_to_read - value;
}



//	////////////////////////////////////////////////////////////////////////////
uint8_t SD_discio_read(SD_t *pSD, uint8_t buf[], uint32_t sector, uint32_t number_of_sectors)
{
	//	-odczyt jednego bloku danych, wielkość bloku to domyślnie 512 bajtów,
	//	-funkcja zwraca wartości: 0xFE --> odczyt prawidłowy,
	//	-0x0X --> błąd danych,
	//	-0xFF --> przekroczono czas,

	uint8_t read;

	pSD->token = 0xFF;

	SD_CS_active(pSD);

	SD_command(CMD17, sector<<9, CMD17_CRC);
	SD_R1(pSD);

	if(pSD->R1 != 0xFF)
	{
		// wait for a response token (timeout = 100ms)
		/*
		readAttempts = 0;
		while(++readAttempts != 1953)
			if((read = SPI_send(0xFF)) != 0xFF)
			{
				break;
			}
		*/

		while((read = SPI_send(0xFF)) == 0xFF);

		if(read == SD_START_TOKEN)
		{
			//VT100_text("TOKEN OK: ");
			//VT100_number(read);
			//VT100_text("\n\r");

			for(uint16_t i=0; i<SD_BLOCK_LENGHT; i++)
			{
				buf[i] = SPI_send(0xFF);
			}

			// read 16-bit CRC
			SPI_send(0xFF);
			SPI_send(0xFF);
		}
		else
		{
			//VT100_text("BLEDNY TOKEN: ");
			//VT100_number(read);
			//VT100_text("\n\r");
		}

		pSD->token = read;
	}

	SD_CS_inactive(pSD);

	return pSD->R1;
}

void SD_discio_send(SD_t *pSD, uint32_t addr, uint8_t *buf)
{
	//	-funkcja do zapisu danych do pamięci karty SD,
	//	-zapis odbywa się za pomocą 512-bajtowych bloków,
	//	-kolejność działań:
	//	-wysył komendy CMD24
	//	-odczekanie na odbiór bajtu R1 w postaci 0x00,
	//	-wysył tokena startu wysyłu bloków - 0xFE,
	//	-wysył bloku danych - 512 bajtów,
	//	-oczekiwanie na token odbioru w postaci 0bxxx0___1,
	//	-gdzie ___
	//	-jeśli karta zaakceptuje dane to otrzymamy w odpowiedzi 0bxxx00101,
	//	-po tym karta wchodzi w stan zajętości wysyłając token 0x00 aż do momentu zakończenia operacji,

	addr = addr<<9;

	SD_CS_active(pSD);

	SD_command(CMD24, addr, CMD24_CRC);
	SD_R1(pSD);

	if(pSD->R1 == 0)
	{
		SPI_send(SD_START_TOKEN);

		for(uint16_t i=0; i<SD_BLOCK_LENGHT; i++)
		{
			SPI_send(pSD->buf[i]);
		}

		// wait for a response token (timeout = 100ms)
		uint32_t readAttempts = 0;
		uint32_t read = 0;

		while(++readAttempts != 98000)
			if((read = SPI_send(0xFF)) != 0xFF) break;

		if((read & 0x1F) == 0x05)
		{
			pSD->token = 0x05;

			// wait for write to finish (timeout = 250ms)
			readAttempts = 0;

			while(SPI_send(0xFF) == 0x00)
			{
				if(++readAttempts == SD_MAX_WRITE_ATTEMPTS)
				{
					pSD->token = 0x00;
					break;
				}
			};
		}
	}

	SD_CS_inactive(pSD);
}


uint8_t SD_discio_initialize(SD_t *pSD)
{
	//	-SD card initialisation to work in SPI mode,

	static volatile uint8_t Stat = 0x01;

	SPI_init();
	SD_power_up(pSD);

	do
	{
		SD_send_CMD0(pSD);
		VT100_text("SD - CMD0:    ");
		VT100_number(pSD->R1);
		VT100_text("\n\r");
	}while(pSD->R1 != 0x01);

	SPI_set_speed(LL_SPI_BAUDRATEPRESCALER_DIV256);

	SD_send_CMD8(pSD);
	VT100_text("SD - CMD8:    ");
	VT100_number(pSD->R1);
	VT100_text("\n\r");

	uint8_t counter = 0;

	do
	{
		SD_send_CMD55(pSD);
		VT100_text("SD - CMD55:   ");
		VT100_number(pSD->R1);
		VT100_text("\n\r");

		SD_send_ACMD41(pSD);
		VT100_text("SD - ACMD41:  ");
		VT100_number(pSD->R1);
		VT100_text("\n\r");
		counter++;

	}while(pSD->R1 != 0 && counter != 100);



	do
	{
		SD_send_CMD58(pSD);
		VT100_text("SD - CMD58:   ");
		VT100_number(pSD->R1);

		for(uint8_t i=0; i<4; i++)
		{
			VT100_text("     ");
			VT100_number(pSD->R7[i]);
		}


		VT100_text("\n\r");
	}while(((pSD->R7[0]>>7) != 1));


	//while(1);

	Stat &= ~0x01;

	return Stat;



}



///*
void USER_TEST(void)
{
	//myprintf("\r\n~ SD card demo by kiwih ~\r\n\r\n");

	  //HAL_Delay(200); //a short delay is important to let the SD card settle


	VT100_text("OK TEST 222: ");
	//VT100_number(fres);
	VT100_text("\n\r");


	  //some variables for FatFs
	  FATFS FatFs; 	//Fatfs handle
	  FIL fil; 		//File handle
	  FRESULT fres; //Result after operations

	  //Open the file system
	  fres = f_mount(&FatFs, "", 1); //1=mount now


		VT100_text("OK TEST: ");
		//VT100_number(fres);
		VT100_text("\n\r");

	  if (fres != FR_OK) {
		VT100_text("f_mount error: ");
		VT100_number(fres);
		VT100_text("\n\r");
		//////////myprintf("f_mount error (%i)\r\n", fres);
		while(1);
	  }

		VT100_text("OK 11: ");
		//VT100_number(fres);
		VT100_text("\n\r");

	  //Let's get some statistics from the SD card
	  DWORD free_clusters, free_sectors, total_sectors;

	  FATFS* getFreeFs;

	  fres = f_getfree("", &free_clusters, &getFreeFs);
	  if (fres != FR_OK) {
		/////////myprintf("f_getfree error (%i)\r\n", fres);
		while(1);
	  }

	  //Formula comes from ChaN's documentation
	  total_sectors = (getFreeFs->n_fatent - 2) * getFreeFs->csize;
	  free_sectors = free_clusters * getFreeFs->csize;

	  //////////myprintf("SD card stats:\r\n%10lu KiB total drive space.\r\n%10lu KiB available.\r\n", total_sectors / 2, free_sectors / 2);



	  //Now let's try to open file "test.txt"
	  fres = f_open(&fil, "test.txt", FA_READ);
	  if (fres != FR_OK) {
		/////////myprintf("f_open error (%i)\r\n");
		while(1);
	  }
	  /////////myprintf("I was able to open 'test.txt' for reading!\r\n");

	  //Read 30 bytes from "test.txt" on the SD card
	  BYTE readBuf[30];

	  //We can either use f_read OR f_gets to get data out of files
	  //f_gets is a wrapper on f_read that does some string formatting for us
	  TCHAR* rres = f_gets((TCHAR*)readBuf, 30, &fil);
	  if(rres != 0) {
		/////////myprintf("Read string from 'test.txt' contents: %s\r\n", readBuf);
	  } else {
		//////////myprintf("f_gets error (%i)\r\n", fres);
	  }

	  //Be a tidy kiwi - don't forget to close your file!
	  f_close(&fil);

	  //Now let's try and write a file "write.txt"
	  fres = f_open(&fil, "write.txt", FA_WRITE | FA_OPEN_ALWAYS | FA_CREATE_ALWAYS);
	  if(fres == FR_OK) {
		/////////myprintf("I was able to open 'write.txt' for writing\r\n");
	  } else {
		/////////myprintf("f_open error (%i)\r\n", fres);
	  }

	  //Copy in a string
	  strncpy((char*)readBuf, "a new file is made!", 19);
	  UINT bytesWrote;
	  fres = f_write(&fil, readBuf, 19, &bytesWrote);
	  if(fres == FR_OK) {
		//////////myprintf("Wrote %i bytes to 'write.txt'!\r\n", bytesWrote);
	  } else {
		/////////myprintf("f_write error (%i)\r\n");
	  }

	  //Be a tidy kiwi - don't forget to close your file!
	  f_close(&fil);

	  //We're done, so de-mount the drive
	  f_mount(NULL, "", 0);


		VT100_text("OK: ");
		//VT100_number(fres);
		VT100_text("\n\r");

		//	$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
		//	$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
		//	$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

}

//*/





/*
uint8_t SD_read_(void)
{
    uint8_t i = 0, res1 = 5;

    // keep polling until actual data received
    while((res1 = SPI_send(0xFF)) == 0xFF)
    {
        i++;

        // if no data received for 8 bytes, break
        if(i > 8) break;
    };

    return res1;
}


void SD_R7_old(uint8_t data[])
{
	//	-R7 response,
	//	-5 byte response,

	*data = SD_R1();

	if(*data > 1)
	{
		return;
	}

	*(data + 1) = SPI_receive();
	*(data + 2) = SPI_receive();
	*(data + 3) = SPI_receive();
	*(data + 4) = SPI_receive();
}




*/









