/*
 * ASCII_table.h
 *
 *  Created on: 7 maj 2024
 *      Author: asmnop
 */

#ifndef INC_ASCII_TABLE_H_
#define INC_ASCII_TABLE_H_


#define LED_A_LINE		0
#define LED_B_LINE		1
#define LED_C_LINE		2
#define LED_D_LINE		3
#define LED_E_LINE		4
#define LED_F_LINE		5
#define LED_G_LINE		6
#define LED_DP_LINE		7

#define SEG_A		(1<<LED_A_LINE)
#define	SEG_B		(1<<LED_B_LINE)
#define	SEG_C		(1<<LED_C_LINE)
#define	SEG_D		(1<<LED_D_LINE)
#define	SEG_E		(1<<LED_E_LINE)
#define	SEG_F		(1<<LED_F_LINE)
#define	SEG_G		(1<<LED_G_LINE)
#define	SEG_DP		(1<<LED_DP_LINE)

#define ASCII_LENGTH	128



//	Definition of ASCII table, which contain 128 signs of const char,
//	Table write in FLASH memory,
const char ASCII_table[ASCII_LENGTH] =
{
	(SEG_G),	//	0,	0x00,	NUL,	Null,
	(SEG_G),	//	1,	0x01,	SOH,	Start of heading,
	(SEG_G),	//	2,	0x02,	STX,	Start of text,
	(SEG_G),	//	3,	0x03,	ETX,	End of text,
	(SEG_G),	//	4,	0x04,	EOT,	End of transmission,
	(SEG_G),	//	5,	0x05,	ENQ,	Enquiry,
	(SEG_G),	//	6,	0x06,	ACK,	Acknowledge,
	(SEG_G),	//	7,	0x07,	BEL,	Bell,

	(SEG_G),	//	8,	0x08,	BS,		Backspace,
	(SEG_G),	//	9,	0x09,	TAB,	Horizontal tab,
	(SEG_G),	//	10,	0x0A,	LF,		NL line feed, new line,
	(SEG_G),	//	11,
	(SEG_G),	//	12,
	(SEG_G),	//	13,
	(SEG_G),	//	14,
	(SEG_G),	//	15,

	(SEG_G),	//	16,
	(SEG_G),	//	17,
	(SEG_G),	//	18,
	(SEG_G),	//	19,
	(SEG_G),	//	20,
	(SEG_G),	//	21,
	(SEG_G),	//	22,
	(SEG_G),	//	23,

	(SEG_G),	//	24,
	(SEG_G),	//	25,
	(SEG_G),	//	26,
	(SEG_G),	//	27,
	(SEG_G),	//	28,
	(SEG_G),	//	29,
	(SEG_G),	//	30,
	(SEG_G),	//	31,

	0,									//	32,	0x20,	Space
	(SEG_B | SEG_C ),					//	33,	0x21,	!
	(SEG_B | SEG_F),					//	34,	0x22,	"
	(SEG_C | SEG_D | SEG_E | SEG_G),	//	35,	0x23,	#
	(SEG_G),							//	36,	0x24,	$	-->	NO SIGN,
	(SEG_G),							//	37,	0x25,	%	-->	NO SIGN,
	(SEG_G),							//	38,	0x26,	&	-->	NO SIGN,
	(SEG_B),							//	38,	0x27,	'

	(SEG_A | SEG_D | SEG_E | SEG_F),	//	40,	0x28,	(
	(SEG_A | SEG_B | SEG_C | SEG_D),	//	41,	0x29,	)
	(SEG_G),							//	42,	0x2A,	*	-->	NO SIGN,
	(SEG_G),							//	43,	0x2B,	+	-->	NO SIGN,
	(SEG_C),							//	44,	0x2C,	,
	(SEG_G),							//	45,	0x2D,	-
	(SEG_G),							//	46,	0x2E,	.	-->	NO SIGN,
	(SEG_B | SEG_E | SEG_G),			//	47,	0x2F,	/

	(SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F),	//	48,	0x30,	0
	(SEG_B | SEG_C),									//	49,	0x31,	1
	(SEG_A | SEG_B | SEG_D | SEG_E | SEG_G),			//	50,	0x32,	2
	(SEG_A | SEG_B | SEG_C | SEG_D | SEG_G),			//	51,	0x33,	3
	(SEG_B | SEG_C | SEG_F | SEG_G),					//	52,	0x34,	4
	(SEG_A | SEG_C | SEG_D | SEG_F | SEG_G),			//	53,	0x35,	5
	(SEG_A | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G),	//	54,	0x36,	6
	(SEG_A | SEG_B | SEG_C),							//	55,	0x37,	7

	(SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G),	//	56,	0x38,	8
	(SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G),			//	57,	0x39,	9
	(SEG_G),													//	58,	0x3A,	:	-->	NO SIGN,
	(SEG_G),													//	59,	0x3B,	;	-->	NO SIGN,
	(SEG_D | SEG_E | SEG_G),									//	60,	0x3C,	<
	(SEG_D | SEG_G),											//	61,	0x3D,	=
	(SEG_C | SEG_D | SEG_G),									//	62,	0x3E,	>
	(SEG_A | SEG_B | SEG_E | SEG_G | SEG_DP),					//	63,	0x3F,	?


	(SEG_A | SEG_B | SEG_D | SEG_E | SEG_F | SEG_G),			//	64,	0x40,	@
	(SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G),			//	65,	0x41,	A
	(SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G),	//	66,	0x42,	B
	(SEG_A | SEG_D | SEG_E | SEG_F),							//	67,	0x43,	C
	(SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F),			//	68,	0x44,	D
	(SEG_A | SEG_D | SEG_E | SEG_F | SEG_G),					//	69,	0x45,	E
	(SEG_A | SEG_E | SEG_F | SEG_G),							//	70,	0x46,	F
	(SEG_A | SEG_C | SEG_D | SEG_E | SEG_F),					//	71,	0x47,	G

	(SEG_B | SEG_C | SEG_E | SEG_F | SEG_G),			//	72,	0x48,	H
	(SEG_B | SEG_C),									//	73,	0x49,	I
	(SEG_B | SEG_C | SEG_D),							//	74,	0x4A,	J
	(SEG_D | SEG_E | SEG_F | SEG_G),					//	75,	0x4B,	K
	(SEG_D | SEG_E | SEG_F),							//	76,	0x4C,	L
	(SEG_A | SEG_C | SEG_E),							//	77,	0x4D,	M
	(SEG_G),											//	78,	0x4E,	N	-->	NO SIGN,
	(SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F),	//	79,	0x4F,	O

	(SEG_A | SEG_B | SEG_E | SEG_F | SEG_G),			//	80,	0x50,	P
	(SEG_G),											//	81,	0x51,	Q	-->	NO SIGN,
	(SEG_G),											//	82,	0x52,	R	-->	NO SIGN,
	(SEG_A | SEG_C | SEG_D | SEG_F | SEG_G),			//	83,	0x53,	S
	(SEG_A | SEG_B | SEG_C),							//	84,	0x54,	T
	(SEG_B | SEG_C | SEG_D | SEG_E | SEG_F),			//	85,	0x55,	U
	(SEG_B | SEG_C | SEG_D | SEG_E | SEG_F),			//	86,	0x56,	V
	(SEG_B | SEG_D | SEG_F),							//	87,	0x57,	W

	(SEG_B | SEG_C | SEG_E | SEG_F | SEG_G),			//	88,	0x58,	X
	(SEG_B | SEG_C | SEG_D | SEG_F | SEG_G),			//	89,	0x59,	Y
	(SEG_A | SEG_B | SEG_D | SEG_E | SEG_G),			//	90,	0x5A,	Z
	(SEG_A | SEG_D | SEG_E | SEG_F),					//	91,	0x5B,	[
	(SEG_C | SEG_F | SEG_G),							//	92,	0x5C,	\'
	(SEG_A | SEG_B | SEG_C | SEG_D),					//	93,	0x5D,	]
	(SEG_A | SEG_B | SEG_F),							//	94,	0x5E,	^
	(SEG_D),											//	95,	0x5F,	_

	(SEG_G),											//	96,		0x60,	`	-->	NO SIGN,
	(SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_G),	//	97,		0x61,	a
	(SEG_C | SEG_D | SEG_E | SEG_F | SEG_G),			//	98,		0x62,	b
	(SEG_D | SEG_E | SEG_G),							//	99,		0x63,	c
	(SEG_B | SEG_C | SEG_D | SEG_E | SEG_G),			//	100,	0x64,	d
	(SEG_A | SEG_B | SEG_D | SEG_E | SEG_F | SEG_G),	//	101,	0x65,	e
	(SEG_A | SEG_E | SEG_F | SEG_G),					//	102,	0x66,	f	-->	LARGE SIGN SYMBOL,
	(SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G),	//	103,	0x67,	g

	(SEG_C | SEG_E | SEG_F | SEG_G),					//	104,	0x68,	h
	(SEG_C),											//	105,	0x69,	i
	(SEG_C | SEG_D),									//	106,	0x6A,	j
	(SEG_D | SEG_E | SEG_F | SEG_G),					//	107,	0x6B,	k	-->	LARGE SIGN SYMBOL,
	(SEG_D | SEG_E),									//	108,	0x6C,	l
	(SEG_G),											//	109,	0x6D,	m	-->	NO SIGN,
	(SEG_C | SEG_E | SEG_G),							//	110,	0x6E,	n
	(SEG_C | SEG_D | SEG_E | SEG_G),					//	111,	0x6F,	o

	(SEG_A | SEG_B | SEG_E | SEG_F | SEG_G),			//	112,	0x70,	p	-->	LARGE SIGN SYMBOL,
	(SEG_A | SEG_B | SEG_C | SEG_F | SEG_G),			//	113,	0x71,	q
	(SEG_E | SEG_G),									//	114,	0x72,	r
	(SEG_A | SEG_C | SEG_D | SEG_F | SEG_G),			//	115,	0x73,	s	-->	LARGE SIGN SYMBOL,
	(SEG_D | SEG_E | SEG_F | SEG_G),					//	116,	0x74,	t
	(SEG_C | SEG_D | SEG_E),							//	117,	0x75,	u
	(SEG_C | SEG_D | SEG_E),							//	118,	0x76,	v
	(SEG_C | SEG_D | SEG_E),							//	119,	0x77,	w

	(SEG_G),											//	120,	0x78,	x	-->	NO SIGN,
	(SEG_B | SEG_C | SEG_D | SEG_F | SEG_G),			//	121,	0x79,	y	-->	LARGE SIGN SYMBOL,
	(SEG_D | SEG_G),									//	122,	0x7A,	z
	(SEG_B | SEG_C | SEG_G),							//	123,	0x7B,	{
	(SEG_E | SEG_F),									//	124,	0x7C,	|
	(SEG_E | SEG_F | SEG_G),							//	125,	0x7D,	}
	(SEG_G),											//	126,	0x7E,	~
	(SEG_G)												//	127,	0x7F,	DEL	-->	NO SIGN,
};


//	########################################################## SYSTEM HEX ########################################################
//	Znaki określające liczby w systemie szesnastkowym,
const char HEX_table[16] =
{
	(SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F),			//	0,	0x00,	0
	(SEG_B | SEG_C),											//	1,	0x01,	1
	(SEG_A | SEG_B | SEG_D | SEG_E | SEG_G),					//	2,	0x02,	2
	(SEG_A | SEG_B | SEG_C | SEG_D | SEG_G),					//	3,	0x03,	3
	(SEG_B | SEG_C | SEG_F | SEG_G),							//	4,	0x04,	4
	(SEG_A | SEG_C | SEG_D | SEG_F | SEG_G),					//	5,	0x05,	5
	(SEG_A | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G),			//	6,	0x06,	6
	(SEG_A | SEG_B | SEG_C),									//	7,	0x07,	7
	(SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G),	//	8,	0x08,	8
	(SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G),			//	9,	0x09,	9
	(SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G),			//	10,	0x0A,	A
	(SEG_C | SEG_D | SEG_E | SEG_F | SEG_G),					//	11,	0x0B,	b
	(SEG_A | SEG_D | SEG_E | SEG_F),							//	12,	0x0C,	C
	(SEG_B | SEG_C | SEG_D | SEG_E | SEG_G),					//	13,	0x0D,	d
	(SEG_A | SEG_D | SEG_E | SEG_F | SEG_G),					//	14,	0x0E,	E
	(SEG_A | SEG_E | SEG_F | SEG_G)								//	15,	0x0F,	F
};


//	################################################################################################
const char BIN_table[4] =
{
	0,
	SEG_B | SEG_C,
	SEG_E | SEG_F,
	SEG_B | SEG_C | SEG_E | SEG_F
};


#endif /* INC_ASCII_TABLE_H_ */






