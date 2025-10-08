/*
 * sd.h
 *
 *  Created on: 25 maj 2025
 *      Author: asmnop
 */

#ifndef SD_H_
#define SD_H_


#include "universal_lib.h"
#include "../../LIB/Inc/gpio.h"
#include "../../LIB/Inc/spi.h"
#include "../../LIB/Inc/terminal.h"

//#include "fatfs.h"

#include "ff.h"
#include "ff_gen_drv.h"

#include "user_diskio.h" /* defines USER_Driver as external */
#include "user_diskio_spi.h"

#define SANDISK_128	128
#define SANDISK_512	512

#define SANDISK_128_SECTORS	246016	//	Number of sectors in card,
#define SANDISK_512_SECTORS	990627	//	Number of sectors in card,


#define CMD0			0
#define CMD0_ARG		0x00000000
#define CMD0_CRC		0x94
#define GO_ILDE_STATE	0

#define CMD1			1
#define CMD1_ARG		0x00000000
#define CMD1_CRC		0xF9
#define SEND_OP_COND	0

#define CMD8			8
#define CMD8_ARG		0x000001AA
#define CMD8_CRC		0x86

#define CMD17                   17
#define CMD17_CRC               0x00
#define SD_MAX_READ_ATTEMPTS    40000

#define CMD24					24
#define CMD24_ARG				0x00
#define CMD24_CRC				0x00
#define SD_MAX_WRITE_ATTEMPTS	40000

#define ACMD41		41
#define ACMD41_ARG	0x00000000
#define ACMD41_CRC	0x00

#define CMD55		55
#define CMD55_ARG	0x00000000
#define CMD55_CRC	0x00

#define CMD58		58
#define CMD58_ARG	0x00000000
#define CMD58_CRC	0x00


#define SD_START_TOKEN	0xFE

#define SD_BLOCK_LENGHT	512


typedef struct
{
	LINE_t *pLINE;
	uint8_t token;
	uint8_t buf[SD_BLOCK_LENGHT];
	uint16_t sector_to_read;
	uint8_t R1;
	uint8_t R7[4];
}SD_t;

extern SD_t SD;

void SD_init(SD_t *pSD);
void SD_power_up(SD_t *pSD);

void SD_send_CMD0(SD_t *pSD);
void SD_send_CMD1(SD_t *pSD);
void SD_send_CMD8(SD_t *pSD);
void SD_send_CMD24(SD_t *pSD, uint32_t addr, uint8_t *buf);
void SD_send_ACMD41(SD_t *pSD);
void SD_send_CMD55(SD_t *pSD);
void SD_send_CMD58(SD_t *pSD);


void SD_show_R1(SD_t *pSD, TERMINAL_t *pTERMINAL, const uint8_t line, const uint8_t row_1, const uint8_t row_2);
void SD_show_OCR(SD_t *pSD, TERMINAL_t *pTERMINAL, const uint8_t line, const uint8_t row_1, const uint8_t row_2);

uint8_t SD_send_CMD17(SD_t *pSD, const uint32_t sector, uint8_t *buf);

void SD_inc_sector(SD_t *pSD, const uint32_t value);
void SD_dec_sector(SD_t *pSD, const uint32_t value);

uint8_t SD_discio_read(SD_t *pSD, uint8_t buf[], uint32_t sector, uint32_t number_of_sectors);
uint8_t SD_discio_initialize(SD_t *pSD);


void USER_TEST(void);



#endif /* SD_H_ */








