/*
 * notes.h
 *
 *  Created on: 12 mar 2023
 *      Author: asmnop
 */

#ifndef NOTES_H_
#define NOTES_H_

//	Dla NOTE_MIN = 64:
#define END_SONG		0	//	Liczba oznaczająca koniec utworu,
#define PAUSE_SONG		1	//	Liczba oznaczająca pauzę w utworze,
#define DOT_4			6	//	Długość dźwięku dla połączonej ćwierćnuty (4) oraz ósemki (8),
#define COUPLE			72
#define TRI_8			200
#define TRI_16			201


//	Octave

#define	A2	110
#define	a2	110
#define	A2b	104
#define	a2b	104
#define	A2x	117
#define	a2x	117
#define	Ais2	117
#define	ais2	117
#define	B2	123
#define	b2	123
#define	B2b	117
#define	b2b	117
#define	C2	65
#define	c2	65
#define	C2x	69
#define	c2x	69
#define	Cis2	69
#define	cis2	69
#define	D2	73
#define	d2	73
#define	D2b	69
#define	d2b	69
#define	D2x	78
#define	d2x	78
#define	Dis2	78
#define	dis2	78
#define	E2	82
#define	e2	82
#define	E2b	78
#define	e2b	78
#define	F2	87
#define	f2	87
#define	F2x	93
#define	f2x	93
#define	Fis2	93
#define	fis2	93
#define	G2	98
#define	g2	98
#define	G2b	93
#define	g2b	93
#define	G2x	104
#define	g2x	104
#define	Gis2	104
#define	gis2	104
#define	H2	123
#define	h2	123
#define	H2b	117
#define	h2b	117
#define	bH2	117
#define	bh2	117
#define	xA2	117
#define	xa2	117
#define	xC2	69
#define	xc2	69
#define	xF2	93
#define	xf2	93
#define	xG2	98
#define	xg2	98

//	Octave

#define	A3		220
#define	a3		220
#define	A3b		208
#define	a3b		208
#define	A3x		233
#define	a3x		233
#define	Ais3	233
#define	ais3	233
#define	B3	247
#define	b3	247
#define	B3b	233
#define	b3b	233
#define	C3	131
#define	c3	131
#define	C3x	139
#define	c3x	139
#define	Cis3	139
#define	cis3	139
#define	D3		147
#define	d3		147
#define	D3b		139
#define	d3b		139
#define	D3x		156
#define	d3x		156
#define	Dis3	156
#define	dis3	156
#define	E3	165
#define	e3	165
#define	E3b	156
#define	e3b	156
#define	F3	175
#define	f3	175
#define	F3x	185
#define	f3x	185
#define	Fis3	185
#define	fis3	185
#define	G3	196
#define	g3	196
#define	G3b	185
#define	g3b	185
#define	G3x	208
#define	g3x	208
#define	Gis3	208
#define	gis3	208
#define	H3	247
#define	h3	247
#define	H3b	233
#define	h3b	233
#define	bH3	233
#define	bh3	233
#define	xA3	233
#define	xa3	233
#define	xC3	139
#define	xc3	139
#define	xF3	185
#define	xf3	185
#define	xG3	196
#define	xg3	196

//	Octave

#define	A4	440
#define	a4	440
#define	A4b	415
#define	a4b	415
#define	A4x	466
#define	a4x	466
#define	Ais4	466
#define	ais4	466
#define	B4	494
#define	b4	494
#define	B4b	466
#define	b4b	466
#define	C4	262
#define	c4	262
#define	C4x	277
#define	c4x	277
#define	Cis4	277
#define	cis4	277
#define	D4	294
#define	d4	294
#define	D4b	277
#define	d4b	277
#define	D4x	311
#define	d4x	311
#define	Dis4	311
#define	dis4	311
#define	E4	330
#define	e4	330
#define	E4b	311
#define	e4b	311
#define	F4	349
#define	f4	349
#define	F4x	370
#define	f4x	370
#define	Fis4	370
#define	fis4	370
#define	G4	392
#define	g4	392
#define	G4b	370
#define	g4b	370
#define	G4x	415
#define	g4x	415
#define	Gis4	415
#define	gis4	415
#define	H4	494
#define	h4	494
#define	H4b	466
#define	h4b	466
#define	bH4	466
#define	bh4	466
#define	xA4	466
#define	xa4	466
#define	xC4	277
#define	xc4	277
#define	xF4	370
#define	xf4	370
#define	xG4	392
#define	xg4	392

//	Octave

#define	A5	880
#define	a5	880
#define	A5b	831
#define	a5b	831
#define	A5x	932
#define	a5x	932
#define	Ais5	932
#define	ais5	932
#define	B5	988
#define	b5	988
#define	B5b	932
#define	b5b	932
#define	C5	523
#define	c5	523
#define	C5x	554
#define	c5x	554
#define	Cis5	554
#define	cis5	554
#define	D5	587
#define	d5	587
#define	D5b	554
#define	d5b	554
#define	D5x	622
#define	d5x	622
#define	Dis5	622
#define	dis5	622
#define	E5	659
#define	e5	659
#define	E5b	622
#define	e5b	622
#define	F5	698
#define	f5	698
#define	F5x	740
#define	f5x	740
#define	Fis5	740
#define	fis5	740
#define	G5	784
#define	g5	784
#define	G5b	740
#define	g5b	740
#define	G5x	831
#define	g5x	831
#define	Gis5	831
#define	gis5	831
#define	H5	988
#define	h5	988
#define	H5b	932
#define	h5b	932
#define	bH5	932
#define	bh5	932
#define	xA5	932
#define	xa5	932
#define	xC5	554
#define	xc5	554
#define	xF5	740
#define	xf5	740
#define	xG5	784
#define	xg5	784

//	Octave

#define	A6	1760
#define	a6	1760
#define	A6b	1661
#define	a6b	1661
#define	A6x	1865
#define	a6x	1865
#define	Ais6	1865
#define	ais6	1865
#define	B6	1976
#define	b6	1976
#define	B6b	1865
#define	b6b	1865
#define	C6	1047
#define	c6	1047
#define	C6x	1109
#define	c6x	1109
#define	Cis6	1109
#define	cis6	1109
#define	D6	1175
#define	d6	1175
#define	D6b	1109
#define	d6b	1109
#define	D6x	1245
#define	d6x	1245
#define	Dis6	1245
#define	dis6	1245
#define	E6	1319
#define	e6	1319
#define	E6b	1245
#define	e6b	1245
#define	F6	1397
#define	f6	1397
#define	F6x	1480
#define	f6x	1480
#define	Fis6	1480
#define	fis6	1480
#define	G6	1568
#define	g6	1568
#define	G6b	1480
#define	g6b	1480
#define	G6x	1661
#define	g6x	1661
#define	Gis6	1661
#define	gis6	1661
#define	H6	1976
#define	h6	1976
#define	H6b	1865
#define	h6b	1865
#define	bH6	1865
#define	bh6	1865
#define	xA6	1865
#define	xa6	1865
#define	xC6	1109
#define	xc6	1109
#define	xF6	1480
#define	xf6	1480
#define	xG6	1568
#define	xg6	1568

//	Octave

#define	A7	3520
#define	a7	3520
#define	A7b	3322
#define	a7b	3322
#define	A7x	3729
#define	a7x	3729
#define	Ais7	3729
#define	ais7	3729
#define	B7	3951
#define	b7	3951
#define	B7b	3729
#define	b7b	3729
#define	C7	2093
#define	c7	2093
#define	C7x	2217
#define	c7x	2217
#define	Cis7	2217
#define	cis7	2217
#define	D7	2349
#define	d7	2349
#define	D7b	2217
#define	d7b	2217
#define	D7x	2489
#define	d7x	2489
#define	Dis7	2489
#define	dis7	2489
#define	E7	2637
#define	e7	2637
#define	E7b	2489
#define	e7b	2489
#define	F7	2794
#define	f7	2794
#define	F7x	2960
#define	f7x	2960
#define	Fis7	2960
#define	fis7	2960
#define	G7	3136
#define	g7	3136
#define	G7b	2960
#define	g7b	2960
#define	G7x	3322
#define	g7x	3322
#define	Gis7	3322
#define	gis7	3322
#define	H7	3951
#define	h7	3951
#define	H7b	3729
#define	h7b	3729
#define	bH7	3729
#define	bh7	3729
#define	xA7	3729
#define	xa7	3729
#define	xC7	2217
#define	xc7	2217
#define	xF7	2960
#define	xf7	2960
#define	xG7	3136
#define	xg7	3136


#endif /* NOTES_H_ */





