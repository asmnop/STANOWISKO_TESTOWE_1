/*
 * songs.c
 *
 *  Created on: 23 mar 2023
 *      Author: asmnop
 */


#include "..\..\LIB\Inc\songs.h"
#include "..\..\LIB\Inc\notes.h"


//	##############
//	UTWÓR NUMER 1:
SONG_t TEST_song =
{
	.name = "tESt",
	.name_extended = "TEST",
	.tempo = 200,
	.notes =
	{
		H4, 32,
		A5, 1, A4, DOT_4, A4, 1,
		C4, 1, D4, 1, E4, 1, F4, 1,
		A4, 1, H4, 1, A4, 1, H4, 1,
		A4, 1, H4, 1, A4, 1, H4, 1,
		END_SONG
	}
};


//	##############
//	UTWÓR NUMER 2:
SONG_t BEEP_song =
{
	.name = "bEEP",
	.name_extended = "BEEP",
	.tempo = 60,
	.notes =
	{
		G7, 64, G7, 64,
		END_SONG
	}
};


//	##############
//	UTWÓR NUMER 3:
SONG_t PANIE_JANIE_song =
{
	.name = "PAnE",
	.name_extended = "PANIE JANIE",
	.tempo = 120,
	.notes =
	{
		G4, 4, A4, 4, H4, 4, G4, 4,
		G4, 4, A4, 4, H4, 4, G4, 4,
		H4, 4, C5, 4, D5, 2,
		H4, 4, C5, 4, D5, 2,
		D5, 8, E5, 8, D5, 8, C5, 8, H4, 4, G4, 4,
		D5, 8, E5, 8, D5, 8, C5, 8, H4, 4, G4, 4,
		A4, 4, A4, 4, G4, 2,
		A4, 4, A4, 4, G4, 2,
		END_SONG
	}
};


//	##############
//	UTWÓR NUMER 4:
SONG_t TANIEC_song =
{
	.name = "tAnC",
	.name_extended = "TANIEC",
	.tempo = 120,
	.notes =
	{
		H4, 8, C5, 8, D5, 4, D5, 4, C5, 4,
		H4, 4, A4, 4, G4, 2,
		A4, 8, G4, 8, A4, 8, H4, 8, A4, 8, H4, 8, C5, 8, A4, 8,
		H4, 2, G4, 2,
		END_SONG
	}
};


//	##############
//	UTWÓR NUMER 5:
SONG_t PRZYBIEZELI_song =
{
	.name = "Przy",
	.name_extended = "PRZYBIEZELI DO BET",
	.tempo = 120,
	.notes =
	{
		F4, 8, E4, 8, F4, 8, G4, 8,		//	Przy - bie - że - li
		A4, 8, G4, 8, A4, 8, H4b, 8,	//	do Be - tle - jem
		C5, 4, D5, 4,					//	pa - ste -
		C5, 2,							//	- rze

		F4, 8, E4, 8, F4, 8, G4, 8,		//	gra - ją sko - cznie
		A4, 8, G4, 8, A4, 8, H4b, 8,	//	Dzie - cią - te - czku
		C5, 4, D5, 4,					//	na li -
		C5, 2,							//	- rze

		F5, 4, C5, 8, C5, 8,			//	Chwa - ła na
		D5, 8, C5, 8, H4b,8, A4, 8,		//	wy - so - ko - ści,
		H4b,4, H4b,8, H4b,8,			//	Chwa - ła na
		C5, 8, H4b,8, A4, 8, G4, 8,		//	wy - so - ko - ści,
		A4, 4, H4b,4,					//	a po -
		C5, 2,							//	- kój
		A4, 4, G4, 4,					//	na zie -
		F4, 2,							//	- mi

		F5, 4, C5, 8, C5, 8,			//	Chwa - ła na
		D5, 8, C5, 8, H4b,8, A4, 8,		//	wy - so - ko - ści,
		H4b,4, H4b,8, H4b,8,			//	Chwa - ła na
		C5, 8, H4b,8, A4, 8, G4, 8,		//	wy - so - ko - ści,
		A4, 4, H4b,4,					//	a po -
		C5, 2,							//	- kój
		A4, 4, G4, 4,					//	na zie -
		F4, 2,							//	- mi
		END_SONG
	}
};


//	##############
//	UTWÓR NUMER 6:
SONG_t MARIO_song =
{
	.name = "Ario",
	.name_extended = "MARIO",
	.tempo = 100,
	.notes =
	{
		E5, 16, E5, 8, E5, 16, PAUSE_SONG, 16, C5, 16, E5, 8, G5, 4, G4, 4,
		C5, 12, G4, 16, PAUSE_SONG, 8, E4, 8, PAUSE_SONG, 16, A4, 8, H4, 16, PAUSE_SONG, 16, A4x, 16, A4, 8,
		G4, 24, E5, 24, G5, 16, A5, 8, F5, 16, G5, 16, PAUSE_SONG, 16, E5, 8, C5, 16, D5, 16, H4, 12,
		C5, 12, G4, 16, PAUSE_SONG, 8, E4, 8, PAUSE_SONG, 16, A4, 8, H4, 16, PAUSE_SONG, 16, A4x, 16, A4, 8,
		G4, 24, E5, 24, G5, 16, A5, 8, F5, 16, G5, 16, PAUSE_SONG, 16, E5, 8, C5, 16, D5, 16, H4, 12,
		END_SONG
	}
};


//	################
//	UTWÓR NUMER 6.1:
SONG_t MARIO_2_song =
{
	.name = "mAro",
	.name_extended = "MARIO 2",
	.tempo = 100,
	.notes =
	{
		E5, 16, E5, 8, E5, 16, PAUSE_SONG, 16, C5, 16, E5, 8, G5, 4, G4, 8,
		END_SONG,



		E5, 16, E5, 8, E5, 16, PAUSE_SONG, 8, E5, 8, C5, 8, E5, 4,
		G5, 4, PAUSE_SONG, 4, G4, 4, PAUSE_SONG, 4,
		C5, 4, PAUSE_SONG, 8, G4, 4, PAUSE_SONG, 8, E4, 4,
		END_SONG
	}
};


//	UTWÓR NUMER 7:

//	https://newsroom.intel.com/editorials/intel-bong-chime-jingle-sound-mark-history/

//	D flat, D flat, G flat, D flat, A flat
SONG_t INTEL_song =
{
	.name = " int",
	.name_extended = "INTEL",
	.tempo = 240,
	.notes =
	{
		D4b, 4, PAUSE_SONG, 2, D4b, 4, G4b, 4, D4b, 4, A4b, 4,
		END_SONG
	}
};


//	##############

//	UTWÓR NUMER 8:
SONG_t CONTRA_song =
{
	.name = "cont",
	.name_extended = "CONTRA",
	.tempo = 60,
	.notes =
	{
		C5, 16, H4b, 16, G4b, 16, F4, 16, G4, 16, F4, 16, E4b, 16, D4, 16,
		E4b, 16, D4b, 16, C4, 16, G5, 16, G5, 16, E5b, 16, G5, 16, G5x, 16,
		END_SONG
	}
};


//	##############
//	UTWÓR NUMER 9:
SONG_t BLUE_EYED_STRANGER_song =
{
	.name = "bLUE",
	.name_extended = "BLUE EYED STRANGER",
	.tempo = 160,
	.notes =
	{
		G4, 4, D4, 4, D4, 4, G4, 8, F4x, 8,
		E4, 8, D4, 8, E4, 8, F4x, 8, G4, 4, A4, 4,
		H4, 4, D5, 4, C5, 8, H4, 8, A4, 8, G4, 8,
		E4, 4, F4x, 4, G4, 4, D4, 4,
		END_SONG
	}
};


//	###############
//	UTWÓR NUMER 10:
SONG_t BOONDOCK_SAINTS_song =
{
	.name = "bOSt",
	.name_extended = "BOONDOCK SAINTS",
	.tempo = 150,
	.notes =
	{
		D5, 4, E5, 8, F5x, 4, D5, 8, H4, 4, D5, 8, H4, 8, A4, 8, G4, 8,
		A4, 4, D5, 8, A4, 8, G4, 8, F4x, 8,	A4, 4, D5, 8, A4, 3,
		D5, 4, E5, 8, F5x, 4, D5, 8, H4, 4, D5, 8, H4, 8, A4, 8, G4, 8,
		D5, 4, E5, 8, F5x, 8, D5, 8, G5, 8, F5x, 4, E5, 8, F5x, 28, E5, 28, D5, 5,
		END_SONG
	}
};


//	###############
//	UTWÓR NUMER 11:
SONG_t ODA_DO_RADOSCI_song =
{
	.name = " OdA",
	.name_extended = "ODA DO RADOSCI",
	.tempo = 115,
	.notes =
	{
		H4, 4, H4, 4, C5, 4, D5, 4,
		D5, 4, C5, 4, H4, 4, A4, 4,
		G4, 4, G4, 4, A4, 4, H4, 4,
		H4, DOT_4, A4, 8, A4, 2,

		H4, 4, H4, 4, C5, 4, D5, 4,
		D5, 4, C5, 4, H4, 4, A4, 4,
		G4, 4, G4, 4, A4, 4, H4, 4,
		A4, DOT_4, G4, 8, G4, 2,

		A4, 4, A4, 4, H4, 4, G4, 4,
		A4, 4, H4, 8, C5, 8, H4, 4, G4, 4,
		A4, 4, H4, 8, C5, 8, H4, 4, A4, 4,
		G4, 4, A4, 4, D4, 2,

		H4, 4, H4, 4, C5, 4, D5, 4,
		D5, 4, C5, 4, H4, 4, A4, 4,
		G4, 4, G4, 4, A4, 4, H4, 4,
		A4, DOT_4, G4, 8, G4, 2,
		END_SONG
	}
};


//	###############
//	UTWÓR NUMER 12:
SONG_t STARY_NIEDZWIEDZ_song =
{
	.name = "niEd",
	.name_extended = "STARY NIEDZWIEDZ",
	.tempo = 120,
	.notes =
	{
		C4, 8, E4, 8, E4, 8, E4, 8,
		F4, 4, D4, 4,
		C4, 2,
		E4, 8, G4, 8, G4, 8, G4, 8,
		C5, 4, A4, 4,
		G4, 2,
		C5, 8, H4, 8, C5, 8, D5, 8,
		C5, DOT_4, H4, 8,

		D5, 8, C5, 8, H4, 8, A4, 8,
		G4, DOT_4, E4, 8,
		C4, 8, E4, 8, E4, 8, E4, 8,
		F4, 4, D4, 4,
		G4, 2,
		C4, 8, E4, 8, E4, 8, E4, 8,
		F4, 4, D4, 4,
		C4, 2,
		END_SONG
	}
};


//	###############
//	UTWÓR NUMER 13:
SONG_t THE_LICK_song =
{
	.name = "LICk",
	.name_extended = "THE LICK",
	.tempo = 240,
	.notes =
	{
		D4, 8, E4, 8, F4, 8, G4, 8,
		E4, 4, C4, 8, D4, 1,
		END_SONG
	}
};


//	###############
//	UTWÓR NUMER 14:
SONG_t ROZMARYN_song =
{
	.name = "roZn",
	.name_extended = "O MOJ ROZMARYNIE",
	.tempo = 120,
	.notes =
	{
		E4, 8, E4, 8, E4, 8, E4, 8,
		A4, 4, G4, 4,
		F4x, 4, G4, 8, F4x, 8,
		E4, 2,

		H4, 8, H4, 8, H4, 8, H4, 8,
		E5, 4, D5, 4,
		C5, 4, D5, 8, C5, 8,
		H4, 2,

		H4, 8, H4, 8, H4, 8, H4, 8,
		C5, 4, H4, 4,
		A4, 8, G4, 8, F4x, 8, G4, 8,
		A4, 4, G4, 4,

		F4x, 4, G4, 8, A4, 8,
		H4, 2,
		H4, 8, H4, 8, H4, 8, H4, 8,
		C5, 4, H4, 4,

		A4, 8, G4, 8, F4x, 8, G4, 8,
		A4, 4, G4, 4,
		F4x, 4, G4, 8, F4x, 8,
		E4, 2,
		END_SONG
	}
};


//	###############
//	UTWÓR NUMER 15:
//	https://www.youtube.com/watch?v=wsvcR3Y19xQ
SONG_t NYAN_CAT_song =
{
	.name = " CAt",
	.name_extended = "NYAN CAT",
	.tempo = 120,
	.notes =
	{
		F5x, 8, G5x, 8, D5x, 16, D5x, 16, PAUSE_SONG, 16, H4, 16, D5, 16, C5x, 16, H4, 16, PAUSE_SONG, 16, H4, 8, C5x, 8,
		D5, 8, D5, 16, C5x, 16, H4, 16, C5x, 16, D5x, 16, F5x, 16, G5x, 16, D5x, 16, F5x, 16, C5x, 16, D5x, 16, H4, 16, C5x, 16, H4, 16,
		D5x, 8, F5x, 8, G5x, 16, D5x, 16, F5x, 16, C5x, 16, D5x, 16, H4, 16, D5, 16, D5x, 16, D5, 16, C5x, 16, H4, 16, C5x, 16,
		D5, 8, H4, 16, C5x, 16, D5x, 16, F5x, 16, C5x, 16, D5x, 16, C5x, 16, H4, 16, C5x, 8, H4, 8, C5x, 8,
		H4, 8, F4x, 16, G4x, 16, H4, 8, F4x, 16, G4x, 16, H4, 16, C5x, 16, D5x, 16, H4, 16, E5, 16, D5x, 16, E5, 16, F5x, 16,
		H4, 8, H4, 8, F4x, 16, G4x, 16, H4, 16, F4x, 16, E5, 16, D5x, 16, C5x, 16, H4, 16, F4x, 16, D4x, 16, E4, 16, G4x, 16,
		H4, 8, F4x, 16, G4x, 16, H4, 8, F4x, 16, G4x, 16, H4, 16, H4, 16, C5x, 16, D5x, 16, H4, 16, F4x, 16, G4x, 16, F4x, 16,
		H4, 8, H4, 16, A4x, 16, H4, 16, F4x, 16, G4x, 16, H4, 16, E5, 16, D5x, 16, E5, 16, F5x, 16, H4, 8, A4x, 8,
		H4, 8, H4, 16, A4x, 16, H4, 16, F4x, 16, G4x, 16, H4, 16, E5, 16, D5x, 16, E5, 16, F5x, 16, H4, 8, C5x, 8,
		//F5x, 8, G5x, 8, D5x, 16, D5x, 16,
		END_SONG
	}
};


//	###############
//	UTWÓR NUMER 16:
SONG_t HEJNAL_song =
{
	.name = " HEJ",
	.name_extended = "HEJNAL MARIACKI",
	.tempo = 80,
	.notes =
	{
		C4, 2, E4, 16, G4, 16, C5, 2,
		G4, 8, E4, 8, C4, 8, G4, 12, G4, 16, G4, 2,
		E4, 4, G4, TRI_16, E4, TRI_16, C4, TRI_16, G3, 2,
		C4, 16, E4, 16, G4, 4, E4, 16, C4, 16, G3, 2,
		C4, 4, E4, TRI_8, C4, TRI_8, E4, TRI_8, G4, 4, E4, 8, C4, 8,
		G4, 12, G4, 16, G4, 2, E4, 2, G4, 8,
		END_SONG
	}
};

//	###############
//	UTWÓR NUMER 17:
SONG_t IN_THE_MOOD_song =
{
	.name = "mood",
	.name_extended = "IN THE MOOD",
	.tempo = 160,
	.notes =
	{
		F4, 8, A4, 8, C5, 8, F5, 8, PAUSE_SONG, 8, F5, 8, PAUSE_SONG, 8, F5, 8,
		PAUSE_SONG, 8, F5, 8, PAUSE_SONG, 8, F5, 8, E5, 8, F5, 8, C5, 8, A4, 8,
		C5, 8, H4, 8, H4b, 8, A4, 8, A4b, 8, G4, 8, PAUSE_SONG, 8, F4, 8,
		//F4, 1,
		PAUSE_SONG, 8, D5, 8, F5, 8, H5, 8, PAUSE_SONG, 8, F5, 8, D5, 8, H5b, 8,
		C5, 8, D5, 8, C5, 8, H5b, 8, G4, 4, D5, 8, E5, 8,
		C5, 8, H5b, 8, G4, 8, G4, 8, PAUSE_SONG, 8,

		END_SONG
	}
};

//	###############
//	UTWÓR NUMER 18:
SONG_t KOTEK_NA_PLOTEK_song =
{
	.name = " kot",
	.name_extended = "KOTEK NA PLOTEK",
	.tempo = 90,
	.notes =
	{
		D4, 4, H3, 4, H3, 4,
		C4, 4, A3, 4, A3, 4,
		G3, 8, H3, 8, D4, 2,
		D4, 4, H3, 4, H3, 4,
		C4, 4, A3, 4, A3, 4,
		G3, 8, H3, 8, G3, 2,
		END_SONG,

		D5, 4, H4, 4, H4, 4,
		C5, 4, A4, 4, A4, 4,
		G4, 8, H4, 8, D5, 2,
		D5, 4, H4, 4, H4, 4,
		C5, 4, A4, 4, A4, 4,
		G4, 8, H4, 8, G4, 2,
		END_SONG,
	}
};





SONG_t piosnka[6] =
{
	[0].name = "dsss",
};


SONG_t *METAL_disc[5] =
{
	&CONTRA_song,		//	0
	&HEJNAL_song,		//	1
	&MARIO_song,		//	2
	&INTEL_song,		//	3
	&IN_THE_MOOD_song,	//	4
};

SONG_t *TECHNO_disc[5] =
{
	&ODA_DO_RADOSCI_song,	//	0
	&STARY_NIEDZWIEDZ_song,	//	1
	&THE_LICK_song,			//	2
	&ROZMARYN_song,			//	3
	&NYAN_CAT_song			//	4
};




SONG_t *ALARM_disc[5] =
{
	&NYAN_CAT_song,		//	0
	&MARIO_song,		//	1
	&NYAN_CAT_song,		//	2
	&NYAN_CAT_song,		//	3
	&HEJNAL_song,		//	4
};


SONG_t *PLAYLIST_ALL[18] =
{
	&TEST_song,					//	0
	&BEEP_song,					//	1
	&PANIE_JANIE_song,			//	2
	&TANIEC_song,				//	3
	&PRZYBIEZELI_song,			//	4
	&MARIO_song,				//	5
	&INTEL_song,				//	6
	&CONTRA_song,				//	7
	&BLUE_EYED_STRANGER_song,	//	8
	&BOONDOCK_SAINTS_song,		//	9
	&ODA_DO_RADOSCI_song,		//	10
	&STARY_NIEDZWIEDZ_song,		//	11
	&THE_LICK_song,				//	12
	&ROZMARYN_song,				//	13
	&NYAN_CAT_song,				//	14
	&HEJNAL_song,				//	15
	&IN_THE_MOOD_song,			//	16
	&KOTEK_NA_PLOTEK_song,		//	17
};


SONG_t **PLAYLIST_ptr = &PLAYLIST_ALL[0];


const char *song_names[] =
{
	&TEST_song.name_extended[0],				//	0
	&BEEP_song.name_extended[0],				//	1
	&PANIE_JANIE_song.name_extended[0],			//	2
	&TANIEC_song.name_extended[0],				//	3
	&PRZYBIEZELI_song.name_extended[0],			//	4
	&MARIO_song.name_extended[0],				//	5
	&INTEL_song.name_extended[0],				//	6
	&CONTRA_song.name_extended[0],				//	7
	&BLUE_EYED_STRANGER_song.name_extended[0],	//	8
	&BOONDOCK_SAINTS_song.name_extended[0],		//	9
	&ODA_DO_RADOSCI_song.name_extended[0],		//	10
	&STARY_NIEDZWIEDZ_song.name_extended[0],	//	11
	&THE_LICK_song.name_extended[0],			//	12
	&ROZMARYN_song.name_extended[0],			//	13
	&NYAN_CAT_song.name_extended[0],			//	14
	&HEJNAL_song.name_extended[0],				//	15
	&IN_THE_MOOD_song.name_extended[0],			//	16
	&KOTEK_NA_PLOTEK_song.name_extended[0]		//	17
};



void address_song(void)
{
	printf("Adres struktury BEEP_song: %p\n", &BEEP_song);
	printf("Adres name[0]: %p\n", &BEEP_song.name[0]);
	printf("Adres name[1]: %p\n", &BEEP_song.name[1]);
	printf("Adres name[2]: %p\n", &BEEP_song.name[2]);
	printf("Adres name[3]: %p\n", &BEEP_song.name[3]);
	printf("Adres tempo: %p\n", &BEEP_song.tempo);
	printf("Adres notes[0]: %p\n", &BEEP_song.notes[0]);
	printf("Adres notes[1]: %p\n", &BEEP_song.notes[1]);
	//printf("Adres notes: %p\n", &BEEP_song.notes);

	printf("Rozmiar struktury BEEP_song: %d\n", sizeof(BEEP_song));
	printf("Rozmiar name[0]: %d\n", sizeof(BEEP_song.name[0]));
	printf("Rozmiar name[1]: %d\n", sizeof(BEEP_song.name[1]));
	printf("Rozmiar name[2]: %d\n", sizeof(BEEP_song.name[2]));
	printf("Rozmiar name[3]: %d\n", sizeof(BEEP_song.name[3]));
	printf("Rozmiar tempo: %d\n", sizeof(BEEP_song.tempo));
	printf("Adres notes[0]: %d\n", BEEP_song.notes[0]);
	printf("Adres notes[1]: %d\n", BEEP_song.notes[1]);
	printf("Adres notes[2]: %d\n", BEEP_song.notes[2]);
	printf("Adres notes[3]: %d\n", BEEP_song.notes[3]);
	printf("Wartosc TEST_song 000: %d\n", TEST_song.name[0]);
	printf("Wartosc TEST_song 111: %d\n", TEST_song.name[1]);
	//printf("Rozmiar notes: %d\n", sizeof(BEEP_song.notes));

	printf("Rozmiar struktury TEST_song: %d\n", sizeof(TEST_song));
	printf("Adres struktury TEST_song: %p\n", &TEST_song);
	printf("Adres name[0]: %p\n", &TEST_song.name[0]);
	printf("Adres name[1]: %p\n", &TEST_song.name[1]);
	printf("Adres name[2]: %p\n", &TEST_song.name[2]);
	printf("Adres name[3]: %p\n", &TEST_song.name[3]);
	printf("Adres tempo: %p\n", &TEST_song.tempo);
	printf("Adres notes: %p\n", &TEST_song.notes);



	printf("Adres struktury PANIE_JANIE_song: %p\n", &PANIE_JANIE_song);
	printf("Roznica adresow: %i\n", &BEEP_song - &PANIE_JANIE_song);

	printf("\n");
	printf("\a");
	for(uint8_t i=0; i<15; i++)
	{
		printf("Nazwa struktury: %25s ==> Adres w pamieci: %p\n", *(song_names+i), PLAYLIST_ALL[i]);
	}


}





