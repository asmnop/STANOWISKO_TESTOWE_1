/*
 * SM_driver.h
 *
 *  Created on: 18 gru 2024
 *      Author: asmnop
 */


#ifndef SM_DRIVER_H_
#define SM_DRIVER_H_

#include "universal_lib.h"
#include "../../LIB/Inc/gpio.h"
#include "../../LIB/Inc/pwm.h"
#include "../../LIB/Inc/songs.h"
//#include "main_SM.h"

enum MICROSTEPPING_e
{
	FULL_STEP,
	HALF_STEP,
	QUARTER_STEP,
	EIGHT_STEP,
	SIXTEENTH_STEP,
};

enum SM_WORK_MODE_e
{
	SM_CCW,
	SM_CW,
	SM_STANDBY,
	SM_STOP,
	SM_ACCEL,
	SM_DECCEL,
	SM_STEP,
	SM_CONTINUOUS,
	SM_INC_DEC,
};

enum SM_WAY_OF_ROTATION
{
	SM_NONE,
	SM_STEP_,
	SM_CONTINUOUS_,
	SM_INC_,
	SM_DEC,
	SM_CONST,
};

enum LINEAR_MODULE_WORK_MODE_e
{
	LM_MANUAL,
	LM_HOMING,
	LM_AUTO,
	LM_SONG,
};

enum LIMIT_SWITCH_e
{
	LIMIT_X1,
	LIMIT_X2,
};




typedef struct
{
	char description[80];		/*Informacje o silniku, numer, piny, ilość zębów na wale, rodzaj silnika, oporność,*/
	CHANNEL_t *step_line;		//	Linia do zadawania kroków,
	LINE_t *dir_line;			//	Linia do obsługi kierunku,
	LINE_t *enable_line;		//	Linia włączająca zasilanie do silnika,
	LINE_t *microstepping_1;	//	Linia 1 do obsługi mikrokroków,
	LINE_t *microstepping_2;	//	Linia 2 do obsługi mikrokroków,
	LINE_t *microstepping_3;	//	Linia 3 do obsługi mikrokroków,

	uint8_t work_mode;					//	Tryb pracy sterownika: STOP, ANGLE, CONTINUOUS,
	uint8_t microstep_mode;				//

	const uint16_t steps_per_revolution;//	Ilość kroków na obrót wału silnika,
	const uint16_t max_freq;			//	Maksymalna częstotliwość sygnału PWM,

	uint16_t step_value;				//	Wartość kroku do zwiększania/zmniejszania,

	volatile uint16_t global_step_counter;	//	Licznik wejść do przerwania,
	volatile int16_t local_position;		//	Położenie lokalne wózka względem pozycji po włączeniu zasilania,
	volatile uint16_t global_position;		//	Położenie globalne wózka względem pozycji krańcówki X1,
	volatile uint16_t CW_step_counter;		//	Licznik wejść do przerwania kiedy silnik miał wykonać krok w kierunku CW,
	volatile uint16_t CCW_step_counter;		//	Licznik wejść do przerwania kiedy silnik miał wykonać krok w kierunku CCW,
	volatile uint8_t flag_limit_switch_1;	//	Flaga mówiąca o osiągnięciu położenia krańcówki 1,
	volatile uint8_t flag_limit_switch_2;	//	Flaga mówiąca o osiągnięciu położenia krańcówki 2,
	volatile int16_t limit_switch_1;		//	Położenie lokalne krańcówki numer 1 na danej osi,
	volatile int16_t limit_switch_2;		//	Położenie lokalne krańcówki numer 2 na danej osi,
	volatile uint8_t homing_value;			//	Ilość kroków od krańcówki do pozycji bazowej,

	volatile uint16_t steps_counter;		//	Licznik kroków zadanych do wykonania z ustaloną częstotliwością,
	volatile uint16_t steps_to_make;		//	Liczba kroków zadana do wykonania z ustaloną częstotliwością,
	volatile uint16_t interval;				//	Ilość interwałów czyli na ile sekcji następuje podział czasu,
	volatile uint16_t interval_counter;		//	Licznik interwałów,
	volatile uint16_t freq_base;			//	Bazowa wartość częstotliwości przy sterowaniu rampą,
	volatile uint16_t time_base;			//	Bazowa wartość czasu przy sterowaniu rampą,

	volatile uint16_t param_table[100][3];	//	Array for ...

}SM_t;

extern LINE_t LIMIT_SWITCH_X1_LINE;
extern LINE_t LIMIT_SWITCH_X2_LINE;


void SM_init(SM_t* ppSM);

void SM_reset_counters(SM_t *pSM);

/**
 * @brief Stepper motor is stopped and the are no power added to motor
 * @note  1
 * @param pSM Pointer to struct
 */
void SM_set_standby(SM_t *pSM);

/**
 * @brief Stop stepper motor. Stop means that there is no new steps are made
 * @param pSM Pointer to struct
 */
void SM_stop(SM_t *pSM);

/**
 * @brief Make a motor movement
 * @param pSM   Pointer to struct
 * @param dir   Rotation directory of motor shaft
 * @param freq  Frequency of generating steps
 * @param steps Number of steps to make
 */
void SM_set_steps(SM_t *pSM, const uint8_t dir, const uint16_t freq, const uint16_t steps);

/**
 * @brief Make a number of rotations in specified time
 * @note  This function based on 'SM_set_steps()'
 * @param pSM
 * @param direction
 * @param time_s
 * @param rotations
 */
void SM_set_rotations(SM_t *pSM, const uint8_t direction, const uint16_t time_s, const uint16_t rotations);



void SM_set_continuous(SM_t *pSM, const uint8_t direction, const uint16_t freq);
void SM_set_continuous_accel(SM_t *pSM, const uint8_t direction, const uint16_t time_s, const uint16_t freq, const uint8_t interval);

uint8_t SM_get_direction(SM_t *pSM);
uint8_t SM_set_direction(SM_t *pSM, const uint8_t direction);
void SM_freq_correction(SM_t *pSM, uint16_t freq);
void SM_set_freq(SM_t *pSM, uint16_t freq);

void SM_homing(SM_t *pSM, const uint8_t Xx);
void SM_set_position(SM_t *pSM, const uint16_t position, const uint16_t freq);


void SM_set_microstepping(SM_t *pSM, uint8_t microstepping);
void SM_inc_dec_freq(SM_t *pSM, const int16_t value);
void SM_change_step_value(SM_t *pSM);
uint8_t SM_get_step_value(SM_t *pSM);

uint16_t SM_get_step_range(SM_t *pSM);

void SM_song(SM_t *pSM, SONG_t *pSONG);
void SM_demo(SM_t *pSM);


#endif /* SM_DRIVER_H_ */









