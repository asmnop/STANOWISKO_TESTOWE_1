/*
 * buzzer.h
 *
 *  Created on: 12 mar 2023
 *      Author: asmnop
 */

#ifndef MAIN_BUZZER_H_
#define MAIN_BUZZER_H_

#include "universal_lib.h"

#include "..\..\LIB\Inc\songs.h"

typedef enum
{
	SONG_OFF,
	SONG_PLAY,
	SONG_PAUSE,
}BOOMBOX_WORK_MODE_e;

typedef struct
{
	uint8_t work_mode;		//	Tryb pracy odtwarzacza utworów: OFF, PLAY, PAUSE,
	uint8_t song_number;	//	Numer utworu, który odtwarzamy lub, który jest zapauzowany,
	uint8_t disc_capacity;	//	Ilość utworów na płycie,
	uint16_t chronon;		//	Dźwięk elementarny, który jest odgrywany lub zapauzowany
	SONG_t **disc;
}BOOMBOX_t;


#define PLAY_MODE	0
#define STOP_MODE	1
#define PAUSE_MODE	8
#define NOTE_MIN	128		//	Najkrótszy dźwięk jaki można wydać z buzzera,

void BUZZER_init(void);
void play_note(const uint16_t freq, const uint32_t duration);
void play_song(SONG_t *song);
void play_BEEP(const uint16_t freq);
void playlist(uint8_t mode, const uint16_t song);
uint8_t play_song_pooling(uint8_t mode, uint8_t song);
void play_song_(void);
void TIM_song_init(void);

void TIM_synchro_init(void);
void TIM_song_play(SONG_t *song);

uint8_t compute_note_duration(uint8_t note_duration);
uint16_t number_of_chronon(SONG_t *song);
uint16_t number_of_notes(SONG_t *pSONG);

#endif /* MAIN_BUZZER_H_ */










