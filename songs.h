/*
 * songs.h
 *
 *  Created on: 12 mar 2023
 *      Author: asmnop
 */

#ifndef MAIN_SONGS_H_
#define MAIN_SONGS_H_


#include "universal_lib.h"


typedef struct
{
	const char name[4];				//	Nazwa utworu do wyświetlenia na wyświetlaczu LED czterosegmentowym,
	const char name_extended[20];	//	Nazwa utworu do wyświetlenia na LCD,
	const uint8_t tempo;			//	Tempo utworu,
	const uint16_t notes[];			//	Nuty wraz z czasem trwania,
}SONG_t;


void address_song(void);


extern SONG_t TEST_song;				//	0
extern SONG_t BEEP_song;				//	1
extern SONG_t PANIE_JANIE_song;			//	2
extern SONG_t TANIEC_song;				//	3
extern SONG_t PRZYBIEZELI_song;			//	4
extern SONG_t MARIO_song;				//	5
extern SONG_t INTEL_song;				//	6
extern SONG_t CONTRA_song;				//	7
extern SONG_t BLUE_EYED_STRANGER_song;	//	8
extern SONG_t BOONDOCK_SAINTS_song;		//	9
extern SONG_t ODA_DO_RADOSCI_song;		//	10
extern SONG_t STARY_NIEDZWIEDZ_song;	//	11
extern SONG_t THE_LICK_song;			//	12
extern SONG_t ROZMARYN_song;			//	13
extern SONG_t NYAN_CAT_song;			//	14
extern SONG_t HEJNAL_song;
extern SONG_t IN_THE_MOOD_song;			//	17
extern SONG_t KOTEK_NA_PLOTEK_song;		//	18

extern SONG_t MARIO_2_song;


extern SONG_t *METAL_disc[];
extern SONG_t *TECHNO_disc[];
extern SONG_t *ALARM_disc[];
extern SONG_t *PLAYLIST_ALL[];

extern SONG_t **PLAYLIST_ptr;






#endif /* MAIN_SONGS_H_ */





