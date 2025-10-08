/*
 * fonts.h
 *
 *  Created on: May 19, 2025
 *      Author: asmnop
 */

#ifndef FONTS_H_
#define FONTS_H_


#include "universal_lib.h"


typedef struct
{
	const uint8_t width;	//	Font width in pixels,
	const uint8_t height;	//	Font height in pixels,
	const uint16_t *data;	//	Pointer to data font array,
}FONT_t;


extern FONT_t FONT_7x10;
extern FONT_t FONT_11x18;
extern FONT_t FONT_16x26;

#endif /* FONTS_H_ */






