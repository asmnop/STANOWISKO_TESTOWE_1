/*
 * main_SM.c
 *
 *  Created on: 20 lut 2025
 *      Author: asmnop
 */


#include "main_1.h"
#include "../../LIB/Inc/usart.h"
#include "../../LIB/Inc/songs.h"
#include "software_timers.h"
#include "../../LIB/Inc/gpio.h"
#include "../../LIB/Inc/servo.h"
#include "../../LIB/Inc/adc.h"

extern USART_t USART_2;
extern SM_t SM[];
extern LINE_t LED_GREEN;
//extern TERMINAL_t TERMINAL_SM;
extern LINE_t SM_1_STEP_LINE;
extern SERVO_t SERVO;
extern LINE_t SERVO_LINE;
extern DMA_t DMA_TIM15_CH1;
extern ADC_t ADC_2;
extern TERMINAL_DISPLAY_t TERM_DISP;
extern LINE_t LIMIT_SWITCH_X1_LINE;
extern LINE_t LIMIT_SWITCH_X2_LINE;

const char* SM_WORK_MODE[] =
{
	"CCW",
	"CW",
	"STANDBY",
	"STOP",
};

const char* SM_WAY_OF_ROTATION[] =
{
	"NONE",
	"STEP",
	"CONTINUOUS",
};

const char* LINEAR_MODULE_MODE[] =
{
	"MANUAL",
	"HOMING",
	"AUTO",
	"SONG",
};

//	////////////////////////////////////////////////////////////////////////////
void main_SM(void)
{
	//	-obsługa nowo odebranej danej po USART z TERMINAL-a,

	SM_t *pSM = &SM[TERM_DISP.instance[TERM_DISP.terminal_ptr]];

	if((USART_get_flag_new_data(&USART_2) == 1))
	{
		USART_reset_flag_new_data(&USART_2);
		uint8_t data_read = USART_get_data(&USART_2);

		//	####################################################################
		if((data_read >= '1') && (data_read <= '9'))
		{
			USART_set_flag_new_data(&USART_2);
			return;
		}

		//	####################################################################
		if(data_read == 'q')
		{
			SM_set_rotations(pSM, SM_CW, 1, 4);
		}
		else if(data_read == 'w')
		{
			SM_set_rotations(pSM, SM_CW, 1, 3);
		}
		else if(data_read == 'e')
		{
			SM_set_rotations(pSM, SM_CW, 1, 2);
		}
		//	####################################################################
		else if(data_read == 'a')
		{
			SM_set_continuous(pSM, SM_CCW, 600);
		}
		else if(data_read == 's')
		{
			SM_set_continuous(pSM, SM_CCW, 400);
		}
		else if(data_read == 'd')
		{
			SM_set_continuous(pSM, SM_CCW, 300);
		}
		//	####################################################################
		else if(data_read == 'r')
		{
			SM_inc_dec_freq(pSM, SM_get_step_value(pSM));
		}
		else if(data_read == 'f')
		{
			SM_inc_dec_freq(pSM, -SM_get_step_value(pSM));
		}
		//	####################################################################
		else if(data_read == 'z')
		{
			SM_set_standby(pSM);
		}
		else if(data_read == 'x')
		{
			SM_stop(pSM);
		}
		else if(data_read == 'c')
		{
			SM_set_steps(pSM, SM_CW, 300, 200);
		}
		else if(data_read == 'v')
		{
			SM_set_steps(pSM, SM_CCW, 400, 600);
		}
		else if(data_read == 'b')
		{
			SM_set_continuous(pSM, SM_CW, 400);
		}
		else if(data_read == 'n')
		{
			SM_set_continuous(pSM, SM_CCW, 400);
		}
		//	################################################################
		else if(data_read == 'o')
		{
			//SM_set_continuous(pSM, SM_CCW_CONTINUOUS, 800);
			//SM_demo(pSM);
			SM_song(pSM, &NYAN_CAT_song);
		}
		else if(data_read == 'p')
		{
			SM_change_step_value(pSM);
		}
		else if(data_read == 'm')
		{
			SM_reset_counters(pSM);
		}
		else if(data_read == 't')
		{
			SM_set_steps(pSM, SM_CW, 300, pSM->steps_per_revolution);
		}
		else if(data_read == 'y')
		{
			SM_set_steps(pSM, SM_CCW, 300, (pSM->steps_per_revolution)>>2);
		}
		else if(data_read == 'l')
		{
			SM_set_rampe(pSM, SM_get_direction(pSM), 2400, 960, 24);
		}
		else if(data_read == 'k')
		{
			SM_set_steps(pSM, SM_get_direction(pSM), 200, 1);
		}
		else if(data_read == 13)
		{
			if(SM_get_direction(pSM) == SM_CW)
			{
				SM_set_direction(pSM, SM_CCW);
			}
			else if(SM_get_direction(pSM) == SM_CCW)
			{
				SM_set_direction(pSM, SM_CW);
			}
		}
	}
}

void terminal_SM(void)
{
	TERMINAL_t *pTERMINAL = TERM_DISP.terminal[TERM_DISP.terminal_ptr];
	SM_t *pSM = &SM[TERM_DISP.terminal_ptr];

	const uint8_t i = 4;
	const uint8_t j = 3;
	const uint8_t legend = 26;
	const uint8_t text_column = 1;
	const uint8_t start_column = 30;

	TERMINAL_line(pTERMINAL, j, text_column, "PARAMETRY SILNIKA:");
	TERMINAL_line(pTERMINAL, j + 1, text_column, pSM->description);

	TERMINAL_line(pTERMINAL, i + 1, text_column, "ILOSC KROKOW NA OBROT");
	TERMINAL_number(pTERMINAL, i + 1, start_column, pSM->steps_per_revolution, 4);

	TERMINAL_line(pTERMINAL, i + 2, text_column, "MAX FREQ:");
	TERMINAL_number(pTERMINAL, i + 2, start_column, (*pSM).max_freq, 4);

	TERMINAL_line(pTERMINAL, i + 3, text_column, "KROKI DO WYKONANIA:");
	TERMINAL_number(pTERMINAL, i + 3, start_column, (*pSM).steps_to_make, 5);

	TERMINAL_line(pTERMINAL, i + 4, text_column, "KROKI WYKONANE:");
	TERMINAL_number(pTERMINAL, i + 4, start_column, (*pSM).steps_counter, 5);

	TERMINAL_line(pTERMINAL, i + 5, text_column, "F KROKOW:");
	TERMINAL_number(pTERMINAL, i + 5, start_column, (*pSM).step_line->timer->frequency, 5);

	TERMINAL_line(pTERMINAL, i + 6, text_column, "TRYB PRACY:");
	TERMINAL_number(pTERMINAL, i + 6, start_column - 3, (*pSM).work_mode, 5);
	TERMINAL_line(pTERMINAL, i + 6, start_column, "                 ");
	TERMINAL_line(pTERMINAL, i + 6, start_column, SM_WORK_MODE[(*pSM).work_mode]);

	TERMINAL_line(pTERMINAL, i + 7, text_column, "SYS_CORE_CLK");
	TERMINAL_number(pTERMINAL, i + 7, start_column, SYS_CORE_CLOCK, 10);

	TERMINAL_line(pTERMINAL, i + 8, text_column, "PRESKALER:");
	TERMINAL_number(pTERMINAL, i + 8, start_column, LL_TIM_GetPrescaler((*pSM).step_line->timer->TIMx), 5);

	TERMINAL_line(pTERMINAL, i + 9, text_column, "F NA WYJSCIU:");
	TERMINAL_number(pTERMINAL, i + 9, start_column, SYS_CORE_CLOCK / (LL_TIM_GetPrescaler((*pSM).step_line->timer->TIMx) + 1), 10);

	TERMINAL_line(pTERMINAL, i + 10, text_column, "ARR");
	TERMINAL_number(pTERMINAL, i + 10, start_column, LL_TIM_GetAutoReload((*pSM).step_line->timer->TIMx), 5);

	TERMINAL_line(pTERMINAL, i + 11, text_column, "CCR");
	TERMINAL_number(pTERMINAL, i + 11, start_column, LL_TIM_OC_GetCompareCH1((*pSM).step_line->timer->TIMx), 5);

	TERMINAL_line(pTERMINAL, i + 12, text_column, "STEP VALUE:");
	TERMINAL_number(pTERMINAL, i + 12, start_column, (*pSM).step_value, 5);

	TERMINAL_line(pTERMINAL, i + 13, text_column, "LICZBA KROKOOW:");
	TERMINAL_number(pTERMINAL, i + 13, start_column, (*pSM).global_step_counter, 5);

	TERMINAL_line(pTERMINAL, i + 14, text_column, "KROKI CW:");
	TERMINAL_number(pTERMINAL, i + 14, start_column, (*pSM).CW_step_counter, 5);

	TERMINAL_line(pTERMINAL, i + 15, text_column, "KROKI CCW:");
	TERMINAL_number(pTERMINAL, i + 15, start_column, (*pSM).CCW_step_counter, 5);

	TERMINAL_line(pTERMINAL, i + 16, text_column, "LICZBA PELNYCH OBROTOW:");
	TERMINAL_number(pTERMINAL, i + 16, start_column, (*pSM).global_step_counter / (*pSM).steps_per_revolution, 5);

	TERMINAL_line(pTERMINAL, i + 17, text_column, "PWM PUL: ");
	TERMINAL_number(pTERMINAL, i + 17, start_column, GPIO_get_input_pin(pSM->step_line->line), 2);

	if(GPIO_get_input_pin(pSM->step_line->line) == 1)
	{
		TERMINAL_line(pTERMINAL, i + 17, start_column + 5, "PWM ON ");
	}
	else
	{
		TERMINAL_line(pTERMINAL, i + 17, start_column + 5, "PWM OFF");
	}

	TERMINAL_line(pTERMINAL, i + 18, text_column, "PWM DIR: ");
	TERMINAL_number(pTERMINAL, i + 18, start_column, GPIO_get_output_pin(pSM->dir_line), 2);

	if(GPIO_get_output_pin(pSM->dir_line) == SM_CW)
	{
		TERMINAL_line(pTERMINAL, i + 18, start_column + 5, "CW ");
	}
	else
	{
		TERMINAL_line(pTERMINAL, i + 18, start_column + 5, "CCW");
	}

	TERMINAL_line(pTERMINAL, i + 19, text_column, "ILE ITERACJI:");
	TERMINAL_number(pTERMINAL, i + 19, start_column, (*pSM).interval, 5);

	TERMINAL_line(pTERMINAL, i + 20, text_column, "ILE ZLICZONYCH ITERACJI:");
	TERMINAL_number(pTERMINAL, i + 20, start_column, (*pSM).interval_counter, 5);

	//	Legenda:
	TERMINAL_line(pTERMINAL, legend, text_column, "z - STANDBY, x - STOP");
	TERMINAL_line(pTERMINAL, legend + 1, text_column, "c - CW, STEP, v - CCW, STEP");
	TERMINAL_line(pTERMINAL, legend + 2, text_column, "b - CW, CONTI, n - CCW, CONTI");
	TERMINAL_line(pTERMINAL, legend + 3, text_column, "r - INC, f - DEC");
	TERMINAL_line(pTERMINAL, legend + 4, text_column, "q w e - STEP");
	TERMINAL_line(pTERMINAL, legend + 5, text_column, "a s d - CONTINUOUS");
	TERMINAL_line(pTERMINAL, legend + 6, text_column, "ENTER - CW <--> CCW");

	SYS_TICK_DMA_info_time(pTERMINAL, pTERMINAL->row - 2, text_column, start_column);	//	2 LINIE,
}


//	////////////////////////////////////////////////////////////////////////////
void main_songs(void)
{
	//	-obsługa nowo odebranej danej po USART z TERMINAL-a,

	//SM_t *pSM = &SM[pTERMINAL->instance];

	if((USART_get_flag_new_data(&USART_2) == 1))// && (pTERMINAL->menu != NULL))
	{
		USART_reset_flag_new_data(&USART_2);
		uint8_t data_read = USART_get_data(&USART_2);	//	Pobranie ostatnio odebranej danej,

		//	####################################################################
		//	####################################################################
		if(data_read == 'z')
		{
			SM_set_standby(&SM[0]);
		}
		else if(data_read == 'x')
		{
			SM_stop(&SM[0]);
		}
		else if(data_read == 'q')
		{
			SM_song(&SM[0], &NYAN_CAT_song);
			USART_reset_flag_new_data(&USART_2);
		}
		else if(data_read == 'w')
		{
			SM_song(&SM[0], &IN_THE_MOOD_song);
			USART_reset_flag_new_data(&USART_2);
		}
	}
}

void terminal_songs(TERMINAL_t *pTERMINAL)
{

	//latch_DMA = 1;

	SM_t *pSM = &SM[0];

	if(pTERMINAL->menu != NULL)
	{
		const uint8_t i = 3;
		const uint8_t legend = 26;
		const uint8_t text_column = 1;
		const uint8_t start_column = 30;

		TERMINAL_line(pTERMINAL, i, text_column, "PARAMETRY SILNIKA:");

		TERMINAL_line(pTERMINAL, i + 1, text_column, "ILOSC KROKOW NA OBROT");
		TERMINAL_number(pTERMINAL, i + 1, start_column, pSM->steps_per_revolution, 4);

		TERMINAL_line(pTERMINAL, i + 2, text_column, "MAX FREQ:");
		TERMINAL_number(pTERMINAL, i + 2, start_column, (*pSM).max_freq, 4);

		TERMINAL_line(pTERMINAL, i + 3, text_column, "KROKI DO WYKONANIA:");
		TERMINAL_number(pTERMINAL, i + 3, start_column, (*pSM).steps_to_make, 5);

		TERMINAL_line(pTERMINAL, i + 4, text_column, "KROKI WYKONANE:");
		TERMINAL_number(pTERMINAL, i + 4, start_column, (*pSM).steps_counter, 5);

		TERMINAL_line(pTERMINAL, i + 5, text_column, "F KROKOW:");
		TERMINAL_number(pTERMINAL, i + 5, start_column, (*pSM).step_line->timer->frequency, 5);

		TERMINAL_line(pTERMINAL, i + 6, text_column, "TRYB PRACY:");
		TERMINAL_number(pTERMINAL, i + 6, start_column - 3, (*pSM).work_mode, 5);
		TERMINAL_line(pTERMINAL, i + 6, start_column, "                 ");
		TERMINAL_line(pTERMINAL, i + 6, start_column, SM_WORK_MODE[(*pSM).work_mode]);

		TERMINAL_line(pTERMINAL, i + 7, text_column, "SYS_CORE_CLK");
		TERMINAL_number(pTERMINAL, i + 7, start_column, SYS_CORE_CLOCK, 10);

		TERMINAL_line(pTERMINAL, i + 8, text_column, "PRESKALER:");
		TERMINAL_number(pTERMINAL, i + 8, start_column, LL_TIM_GetPrescaler((*pSM).step_line->timer->TIMx), 5);

		TERMINAL_line(pTERMINAL, i + 9, text_column, "F NA WYJSCIU:");
		TERMINAL_number(pTERMINAL, i + 9, start_column, SYS_CORE_CLOCK / (LL_TIM_GetPrescaler((*pSM).step_line->timer->TIMx) + 1), 10);

		TERMINAL_line(pTERMINAL, i + 10, text_column, "ARR");
		TERMINAL_number(pTERMINAL, i + 10, start_column, LL_TIM_GetAutoReload((*pSM).step_line->timer->TIMx), 5);

		TERMINAL_line(pTERMINAL, i + 11, text_column, "CCR");
		TERMINAL_number(pTERMINAL, i + 11, start_column, LL_TIM_OC_GetCompareCH1((*pSM).step_line->timer->TIMx), 5);

		TERMINAL_line(pTERMINAL, i + 12, text_column, "STEP VALUE:");
		TERMINAL_number(pTERMINAL, i + 12, start_column, (*pSM).step_value, 5);

		TERMINAL_line(pTERMINAL, i + 13, text_column, "LICZBA KROKOOW:");
		TERMINAL_number(pTERMINAL, i + 13, start_column, (*pSM).global_step_counter, 5);

		TERMINAL_line(pTERMINAL, i + 14, text_column, "KROKI CW:");
		TERMINAL_number(pTERMINAL, i + 14, start_column, (*pSM).CW_step_counter, 5);

		TERMINAL_line(pTERMINAL, i + 15, text_column, "KROKI CCW:");
		TERMINAL_number(pTERMINAL, i + 15, start_column, (*pSM).CCW_step_counter, 5);

		TERMINAL_line(pTERMINAL, i + 16, text_column, "LICZBA PELNYCH OBROTOW:");
		TERMINAL_number(pTERMINAL, i + 16, start_column, (*pSM).global_step_counter / (*pSM).steps_per_revolution, 5);

		TERMINAL_line(pTERMINAL, i + 17, text_column, "PWM PUL: ");
		TERMINAL_number(pTERMINAL, i + 17, start_column, GPIO_get_input_pin(pSM->step_line->line), 2);

		if(GPIO_get_input_pin(pSM->step_line->line) == 1)
		{
			TERMINAL_line(pTERMINAL, i + 17, start_column + 5, "PWM ON ");
		}
		else
		{
			TERMINAL_line(pTERMINAL, i + 17, start_column + 5, "PWM OFF");
		}

		TERMINAL_line(pTERMINAL, i + 18, text_column, "PWM DIR: ");
		TERMINAL_number(pTERMINAL, i + 18, start_column, GPIO_get_output_pin(pSM->dir_line), 2);

		if(GPIO_get_output_pin(pSM->dir_line) == SM_CW)
		{
			TERMINAL_line(pTERMINAL, i + 18, start_column + 5, "CW ");
		}
		else
		{
			TERMINAL_line(pTERMINAL, i + 18, start_column + 5, "CCW");
		}

		TERMINAL_line(pTERMINAL, i + 19, text_column, "ILE ITERACJI:");
		TERMINAL_number(pTERMINAL, i + 19, start_column, (*pSM).interval, 5);

		TERMINAL_line(pTERMINAL, i + 20, text_column, "ILE ZLICZONYCH ITERACJI:");
		TERMINAL_number(pTERMINAL, i + 20, start_column, (*pSM).interval_counter, 5);

		//	Legenda:
		TERMINAL_line(pTERMINAL, legend, text_column, "z - STANDBY, x - STOP");
		TERMINAL_line(pTERMINAL, legend + 1, text_column, "c - CW, STEP, v - CCW, STEP");
		TERMINAL_line(pTERMINAL, legend + 2, text_column, "b - CW, CONTI, n - CCW, CONTI");
		TERMINAL_line(pTERMINAL, legend + 3, text_column, "r - INC, f - DEC");
		TERMINAL_line(pTERMINAL, legend + 4, text_column, "q w e - STEP");
		TERMINAL_line(pTERMINAL, legend + 5, text_column, "a s d - CONTINUOUS");
		TERMINAL_line(pTERMINAL, legend + 6, text_column, "ENTER - CW <--> CCW");

		SYS_TICK_DMA_info_time(pTERMINAL, pTERMINAL->row - 2, text_column, start_column);	//	2 LINIE,

		//latch_DMA_END = 1;
	}
}


//	////////////////////////////////////////////////////////////////////////////
void main_servo(void)
{
	//	-obsługa nowo odebranej danej po USART z TERMINAL-a,

	if(USART_get_flag_new_data(&USART_2) == 1)
	{
		USART_reset_flag_new_data(&USART_2);
		uint8_t data_read = USART_get_data(&USART_2);	//	Pobranie ostatnio odebranej danej,

		if((data_read >= '1') && (data_read <= '9'))
		{
			USART_set_flag_new_data(&USART_2);
			return;
		}

		//	####################################################################
		if(data_read == 'q')
		{
			SERVO_set_max(&SERVO);
		}
		else if(data_read == 'w')
		{
			SERVO_set_center(&SERVO);
		}
		else if(data_read == 'e')
		{
			SERVO_set_min(&SERVO);
		}
		else if(data_read == 'f')
		{
			SERVO_dec(&SERVO);
		}
		else if(data_read == 'r')
		{
			SERVO_inc(&SERVO);
		}
		else if(data_read == 'a')
		{
			SERVO_set_angle(&SERVO, 135);
		}
		else if(data_read == 's')
		{
			SERVO_set_angle(&SERVO, 45);
		}
		else if(data_read == 'm')
		{
			DMA_stop(&DMA_TIM15_CH1);
			DMA_TIM15_CH1.ptr_16 = DMA_TIM15_CH1.ptr_16 + 20;
			//DMA_TIM15_CH1.ptr_16 = &DMA_TIM15_CH1.ptr_16[20];
			LL_DMA_SetMode(DMA_TIM15_CH1.DMAx, DMA_TIM15_CH1.channel, LL_DMA_MODE_NORMAL);
			LL_DMA_ConfigAddresses(DMA_TIM15_CH1.DMAx, DMA_TIM15_CH1.channel,
			(uint32_t)(DMA_TIM15_CH1.ptr_16),												//	Źródło
			(uint32_t)(&TIM15->CCR1),														//	Cel,
			LL_DMA_GetDataTransferDirection(DMA_TIM15_CH1.DMAx, DMA_TIM15_CH1.channel));	//	Kierunek,
			LL_DMA_SetDataLength(DMA_TIM15_CH1.DMAx, DMA_TIM15_CH1.channel, 1);
			DMA_TIM15_CH1.ptr_16 = DMA_TIM15_CH1.ptr_16 - 20;
			DMA_start(&DMA_TIM15_CH1);
		}
		//	////////////////////////////////////////////////////////////////////
		else if(data_read == 'z')
		{
			DMA_stop(&DMA_TIM15_CH1);
		}
		else if(data_read == 'x')
		{
			DMA_start(&DMA_TIM15_CH1);
		}
		else if(data_read == 'p')
		{
			DMA_TIM15_CH1.ptr_16 = (uint16_t*)SERVO.data;
			DMA_TIM15_CH1.number_of_data = SERVO.total_size<<1;
			DMA_TIM15_CH1_init(&DMA_TIM15_CH1);
			DMA_start(&DMA_TIM15_CH1);
		}
	}
}

void terminal_servo(void)
{
	SERVO_t *pSERVO = &SERVO;
	TERMINAL_t *pTERMINAL = TERM_DISP.terminal[TERM_DISP.terminal_ptr];

	//if(pTERMINAL->menu != NULL)
	{
		const uint8_t i = 3;
		const uint8_t legend = 26;
		const uint8_t text_column = 1;
		const uint8_t start_column = 30;

		TERMINAL_line(pTERMINAL, i, text_column, "PARAMETRY SERWA:");

		TERMINAL_line(pTERMINAL, i + 1, text_column, "ILOSC MOZLIWYCH POZYCJI:");
		TERMINAL_number(pTERMINAL, i + 1, start_column, pSERVO->max_value - pSERVO->min_value, 4);

		TERMINAL_line(pTERMINAL, i + 2, text_column, "WARTOSC MINIMALNA POZYCJI:");
		TERMINAL_number(pTERMINAL, i + 2, start_column, pSERVO->min_value, 4);

		TERMINAL_line(pTERMINAL, i + 3, text_column, "WARTOSC MAKSYMALNA POZYCJI:");
		TERMINAL_number(pTERMINAL, i + 3, start_column, pSERVO->max_value, 5);

		TERMINAL_line(pTERMINAL, i + 4, text_column, "POLOZENIE KROKOWE GLOBALNE:");
		TERMINAL_number(pTERMINAL, i + 4, start_column, SERVO_get_global_position(pSERVO), 4);

		TERMINAL_line(pTERMINAL, i + 5, text_column, "POLOZENIE KATOWE GLOBALNE:");
		TERMINAL_number(pTERMINAL, i + 5, start_column, SERVO_get_angle_global_position(pSERVO), 3);

		//TERMINAL_line(pTERMINAL, i + 6, text_column, "TRYB PRACY:");
		//TERMINAL_number(pTERMINAL, i + 6, start_column - 3, (*pSM).work_mode, 5);
		//TERMINAL_line(pTERMINAL, i + 6, start_column, "                 ");
		//TERMINAL_line(pTERMINAL, i + 6, start_column, SM_WORK_MODE[(*pSM).work_mode]);

		TERMINAL_line(pTERMINAL, i + 7, text_column, "SYS_CORE_CLK");
		TERMINAL_number(pTERMINAL, i + 7, start_column, SYS_CORE_CLOCK, 10);

		TERMINAL_line(pTERMINAL, i + 8, text_column, "PRESKALER:");
		TERMINAL_number(pTERMINAL, i + 8, start_column, LL_TIM_GetPrescaler((*pSERVO).signal_line->timer->TIMx) + 1, 5);

		TERMINAL_line(pTERMINAL, i + 9, text_column, "F NA WYJSCIU:");
		TERMINAL_number(pTERMINAL, i + 9, start_column, SYS_CORE_CLOCK / (LL_TIM_GetPrescaler((*pSERVO).signal_line->timer->TIMx) + 1), 10);

		TERMINAL_line(pTERMINAL, i + 10, text_column, "ARR");
		TERMINAL_number(pTERMINAL, i + 10, start_column, LL_TIM_GetAutoReload((*pSERVO).signal_line->timer->TIMx) + 1, 5);

		TERMINAL_line(pTERMINAL, i + 11, text_column, "CCR");
		TERMINAL_number(pTERMINAL, i + 11, start_column, LL_TIM_OC_GetCompareCH1((*pSERVO).signal_line->timer->TIMx), 5);

		TERMINAL_line(pTERMINAL, i + 12, text_column, "ILE DANYCH DO POZOSTALO:");
		TERMINAL_number(pTERMINAL, i + 12, start_column, LL_DMA_GetDataLength(DMA_TIM15_CH1.DMAx, DMA_TIM15_CH1.channel), 5);

		//TERMINAL_line(pTERMINAL, i + 13, text_column, "LICZBA KROKOOW:");
		//TERMINAL_number(pTERMINAL, i + 13, start_column, (*pSM).global_step_counter, 5);

		//TERMINAL_line(pTERMINAL, i + 14, text_column, "KROKI CW:");
		//TERMINAL_number(pTERMINAL, i + 14, start_column, (*pSM).CW_step_counter, 5);

		//TERMINAL_line(pTERMINAL, i + 15, text_column, "KROKI CCW:");
		//TERMINAL_number(pTERMINAL, i + 15, start_column, (*pSM).CCW_step_counter, 5);

		//TERMINAL_line(pTERMINAL, i + 16, text_column, "LICZBA PELNYCH OBROTOW:");
	//	TERMINAL_number(pTERMINAL, i + 16, start_column, (*pSM).global_step_counter / (*pSM).steps_per_revolution, 5);

		TERMINAL_line(pTERMINAL, i + 17, text_column, "PWM PUL: ");
	//	TERMINAL_number(pTERMINAL, i + 17, start_column, GPIO_get_input_pin(pSM->step_line->line), 2);

		//if(GPIO_get_input_pin(pSM->step_line->line) == 1)
		{
			TERMINAL_line(pTERMINAL, i + 17, start_column + 5, "PWM ON ");
		}
		//else
		{
			TERMINAL_line(pTERMINAL, i + 17, start_column + 5, "PWM OFF");
		}

		TERMINAL_line(pTERMINAL, i + 18, text_column, "PWM DIR: ");
		//TERMINAL_number(pTERMINAL, i + 18, start_column, GPIO_get_output_pin(pSM->dir_line), 2);

		//if(GPIO_get_output_pin(pSM->dir_line) == SM_CW)
		{
			TERMINAL_line(pTERMINAL, i + 18, start_column + 5, "CW ");
		}
		//else
		{
			TERMINAL_line(pTERMINAL, i + 18, start_column + 5, "CCW");
		}

		TERMINAL_line(pTERMINAL, i + 19, text_column, "ILE ITERACJI:");
		//TERMINAL_number(pTERMINAL, i + 19, start_column, (*pSM).interval, 5);

		TERMINAL_line(pTERMINAL, i + 20, text_column, "ILE ZLICZONYCH ITERACJI:");
		//TERMINAL_number(pTERMINAL, i + 20, start_column, (*pSM).interval_counter, 5);

		//	Legenda:
		TERMINAL_line(pTERMINAL, legend, text_column, "z - STANDBY, x - STOP");
		TERMINAL_line(pTERMINAL, legend + 1, text_column, "c - CW, STEP, v - CCW, STEP");
		TERMINAL_line(pTERMINAL, legend + 2, text_column, "b - CW, CONTI, n - CCW, CONTI");
		TERMINAL_line(pTERMINAL, legend + 3, text_column, "r - INC, f - DEC");
		TERMINAL_line(pTERMINAL, legend + 4, text_column, "q w e - STEP");
		TERMINAL_line(pTERMINAL, legend + 5, text_column, "a s d - CONTINUOUS");
		TERMINAL_line(pTERMINAL, legend + 6, text_column, "ENTER - CW <--> CCW");



		SYS_TICK_DMA_info_time(pTERMINAL, pTERMINAL->row - 2, text_column, start_column);	//	2 LINIE,

	}
}


//	////////////////////////////////////////////////////////////////////////////
void main_adc(void)
{
	//	-obsługa nowo odebranej danej po USART z TERMINAL-a,

	if(USART_get_flag_new_data(&USART_2) == 1)
	{
		USART_reset_flag_new_data(&USART_2);
		uint8_t data_read = USART_get_data(&USART_2);	//	Pobranie ostatnio odebranej danej,

		if((data_read >= '1') && (data_read <= '9'))
		{
			USART_set_flag_new_data(&USART_2);
			return;
		}

		//	####################################################################
		if(data_read == 'q')
		{

		}
		else if(data_read == 'w')
		{

		}
	}
}

void terminal_adc(void)
{
	ADC_t *pADC = &ADC_2;
	TERMINAL_t *pTERMINAL = TERM_DISP.terminal[TERM_DISP.terminal_ptr];

	const uint8_t i = 3;
	const uint8_t text_column = 1;
	const uint8_t start_column = 30;

	TERMINAL_line(pTERMINAL, i, text_column, "KONWERSJA ADC:");

	TERMINAL_line(pTERMINAL, i + 1, text_column, "WYNIK KONWERSJI:");
	//TERMINAL_number(pTERMINAL, i + 1, start_column, pADC->data >> 16, 4);

	TERMINAL_line(pTERMINAL, i + 2, text_column, "NAPIECIE NA WYJSCIU:");
	TERMINAL_number(pTERMINAL, i + 2, start_column, ADC_compute_voltage(&ADC_2), 4);

	TERMINAL_line(pTERMINAL, i + 3, text_column, "WYNIK KONWERSJI:");
	//TERMINAL_number(pTERMINAL, i + 3, start_column, pADC->data & 0x0000FFFF, 4);

	TERMINAL_line(pTERMINAL, i + 4, text_column, "JOYSTICK - OS X: <-- -->");
	TERMINAL_number(pTERMINAL, i + 4, start_column, pADC->data_received[0], 4);

	TERMINAL_line(pTERMINAL, i + 5, text_column, "JOYSTICK - OS Y: /\\ \\/");
	TERMINAL_number(pTERMINAL, i + 5, start_column, pADC->data_received[1], 4);

	TERMINAL_line(pTERMINAL, i + 6, text_column, "POTENCJOMETR 1:");
	TERMINAL_number(pTERMINAL, i + 6, start_column, pADC->data_received[2], 4);

	TERMINAL_line(pTERMINAL, i + 7, text_column, "POTENCJOMETR 2:");
	TERMINAL_number(pTERMINAL, i + 7, start_column, pADC->data_received[3], 4);

	//	########################################################################
	SYS_TICK_DMA_info_time(pTERMINAL, pTERMINAL->row - 2, text_column, start_column);	//	2 LINIE,
}


//	////////////////////////////////////////////////////////////////////////////
void main_base(void)
{
	//	====================================================
	//	OBSŁUGA NOWO ODEBRANYCH DANYCH PO USART Z TERMINALA:
	//	====================================================
	if(USART_get_flag_new_data(&USART_2) == 1)
	{
		USART_reset_flag_new_data(&USART_2);
		uint8_t data_read = USART_get_data(&USART_2);	//	Pobranie ostatnio odebranej danej,

		if(data_read >= '1' && data_read <= '9')
		{
			USART_set_flag_new_data(&USART_2);
			return;
		}

		//	####################################################################
		if(data_read == 't')
		{
			GPIO_toggle_pin(&LED_GREEN);
			USART_reset_flag_new_data(&USART_2);
		}
		else if(data_read == 'y')
		{
			GPIO_set_pin(&LED_GREEN);
			USART_reset_flag_new_data(&USART_2);
		}
		else if(data_read == 'r')
		{
			GPIO_reset_pin(&LED_GREEN);
			USART_reset_flag_new_data(&USART_2);
		}
	}
}

void terminal_base(void)
{
	//	~~~~~~~~~~~~~~~~
	//	TERMINAL BAZOWY:
	//	~~~~~~~~~~~~~~~~


	TERMINAL_t *pTERMINAL = TERM_DISP.terminal[TERM_DISP.terminal_ptr];

	const uint8_t i = 5;
	const uint8_t text_column = 1;
	const uint8_t start_column = 30;

	SYS_TICK_terminal_info(pTERMINAL, i, text_column, start_column);			//	4 LINIE,

	TERMINAL_line(pTERMINAL, i + 4, text_column, "GREEN LED: ");
	TERMINAL_number(pTERMINAL, i + 4, start_column, GPIO_get_output_pin(&LED_GREEN), 2);
	if(GPIO_get_output_pin(&LED_GREEN) == 1)
	{
		TERMINAL_line(pTERMINAL, i + 4, start_column + 5, "LED ON ");
	}
	else
	{
		TERMINAL_line(pTERMINAL, i + 4, start_column + 5, "LED OFF");
	}

	USART_terminal_info(pTERMINAL, i + 5, text_column, start_column);			//	4 LINIE,
	SYS_TICK_terminal_info_time(pTERMINAL, i + 9, text_column, start_column);	//	1 LINIA,

	SYS_TICK_DMA_info_time(pTERMINAL, pTERMINAL->row - 2, text_column, start_column);	//	2 LINIE,

	//	~~~~~~~~~~~~~~~~
	//	TERMINAL BAZOWY:
	//	~~~~~~~~~~~~~~~~

	/*
	if(TERMINAL_BASE.menu != NULL)
	{

		const uint8_t i = 5;
		const uint8_t text_column = 1;
		const uint8_t start_column = 30;

		TERMINAL_line(&TERMINAL_BASE, i, text_column, "SYS_TICK:  ms - ");
		TERMINAL_number(&TERMINAL_BASE, i, start_column, (uint16_t)SYS_TICK.ms, 10);

		TERMINAL_line(&TERMINAL_BASE, i + 1, text_column, "SYS_TICK:   s - ");
		TERMINAL_number(&TERMINAL_BASE, i + 1, start_column, (uint16_t)SYS_TICK.s, 2);

		TERMINAL_line(&TERMINAL_BASE, i + 2, text_column, "SYS_TICK: min - ");
		TERMINAL_number(&TERMINAL_BASE, i + 2, start_column, (uint16_t)SYS_TICK.min, 2);

		TERMINAL_line(&TERMINAL_BASE, i + 3, text_column, "SYS_TICK:   h - ");
		TERMINAL_number(&TERMINAL_BASE, i + 3, start_column, (uint16_t)SYS_TICK.h, 2);

		TERMINAL_line(&TERMINAL_BASE, i + 4, text_column, "GREEN LED: ");
		TERMINAL_number(&TERMINAL_BASE, i + 4, start_column, GPIO_get_output_pin(&LED_GREEN), 2);

		if(GPIO_get_output_pin(&LED_GREEN) == 1)
		{
			TERMINAL_line(&TERMINAL_BASE, i + 4, start_column + 5, "LED ON ");
		}
		else
		{
			TERMINAL_line(&TERMINAL_BASE, i + 4, start_column + 5, "LED OFF");
		}

		//	Wpisanie ostatnio odebranego znaku z terminala:
		uint8_t tmp_last_key[10] = {USART_get_data(&USART_2), '\0'};
		if((tmp_last_key[0] == '\x0D') || (tmp_last_key[0] == 127) || (tmp_last_key[0] == 9))
		{
			tmp_last_key[0] = '!';
		}
		TERMINAL_line(&TERMINAL_BASE, i + 6, text_column, "Ostatni klawisz:");
		TERMINAL_line(&TERMINAL_BASE, i + 6, start_column, (char*)&tmp_last_key);

		//	Wpisanie kodu ASCII ostatnio odebranego znaku z terminala:
		TERMINAL_line(&TERMINAL_BASE, i + 5, text_column, "KOD KLAWISZA:");
		TERMINAL_number(&TERMINAL_BASE, i + 5, start_column, USART_get_data(&USART_2), 3);

		//	Wpisanie ostatniej sekwencji znaków z terminala:
		for(uint8_t i=0; i<USART_2.buf; i++)
		{
			if((USART_2.RX_data[i] == '\x0D') || (USART_2.RX_data[i] == 127) || (USART_2.RX_data[i] == 9))	//	Jeżeli odebrano ENTER,
			{
				tmp_last_key[i] = '!';
			}
			else
			{
				tmp_last_key[i] = USART_2.RX_data[i];
			}
		}
		TERMINAL_line(&TERMINAL_BASE, i + 7, text_column, "Sekwencja klawiszy:");
		TERMINAL_arr(&TERMINAL_BASE, i + 7, start_column, (char*)tmp_last_key, 10);

		TERMINAL_line(&TERMINAL_BASE, i + 8, text_column, "Odstep czasu [ms]:");
		TERMINAL_number(&TERMINAL_BASE, i + 8, start_column, latch_value_diff, 8);



		TERMINAL_line(&TERMINAL_BASE, i + 12, text_column, "Co ile zachodzi ISR [ms]:");
		TERMINAL_line(&TERMINAL_BASE, i + 12, start_column, "     ");
		TERMINAL_number(&TERMINAL_BASE, i + 12, start_column, latch_value_diff_DMA, 8);

		TERMINAL_line(&TERMINAL_BASE, i + 13, text_column, "Czas odswierzania [ms]");
		TERMINAL_line(&TERMINAL_BASE, i + 13, start_column, "     ");
		TERMINAL_number(&TERMINAL_BASE, i + 13, start_column, latch_value_diff_DMA_END, 8);


	}
	//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	*/
}


//	////////////////////////////////////////////////////////////////////////////
void main_axis(void)
{
	//	-obsługa nowo odebranej danej po USART z TERMINAL-a,

	//SM_t *pSM = &SM[TERM_DISP.instance[TERM_DISP.terminal_ptr]];
	SM_t *pSM = &SM[4];

	SM_joypad(pSM, ADC_2.data_received[0]);

	if((USART_get_flag_new_data(&USART_2) == 1))
	{
		USART_reset_flag_new_data(&USART_2);
		uint8_t data_read = USART_get_data(&USART_2);

		//	####################################################################
		if((data_read >= '1') && (data_read <= '9'))
		{
			USART_set_flag_new_data(&USART_2);
			return;
		}

		switch(data_read)
		{
			//	################################################################
			case 'q':
			{
				SM_set_continuous(pSM, SM_CCW, 60);
				break;
			}
			case 'a':
			{
				SM_set_continuous(pSM, SM_CW, 60);
				break;
			}
			//	################################################################
			case 'w':
			{
				SM_set_steps(pSM, SM_CCW, 60, 10);
				break;
			}
			case 's':
			{
				SM_set_steps(pSM, SM_CW, 60, 10);
				break;
			}
			//	################################################################
			case 'r':
			{
				SM_inc_dec_freq(pSM, SM_get_step_value(pSM));
				break;
			}
			case 'f':
			{
				SM_inc_dec_freq(pSM, -SM_get_step_value(pSM));
				break;
			}
			//	################################################################
			case 'z':
			{
				SM_set_standby(pSM);
				break;
			}
			case 'x':
			{
				SM_stop(pSM);
				break;
			}
			//	################################################################
			case 'p':
			{
				SM_change_step_value(pSM);
				break;
			}
			case 'm':
			{
				SM_reset_counters(pSM);
				break;
			}
			case 'l':
			{
				SM_set_rampe(pSM, SM_get_direction(pSM), 1200, 180, 6);
				break;
			}
			case 'k':
			{
				SM_song(pSM, &KOTEK_NA_PLOTEK_song);
				break;
			}
			//	################################################################
			case 'h':
			{
				SM_homing(pSM, LIMIT_X1);
				break;
			}
			case 'n':
			{
				SM_homing(pSM, LIMIT_X2);
				break;
			}
			case 'o':
			{

				break;
			}
			//	################################################################
			case 13:
			{
				if(SM_get_direction(pSM) == SM_CW)
				{
					SM_set_direction(pSM, SM_CCW);
				}
				else if(SM_get_direction(pSM) == SM_CCW)
				{
					SM_set_direction(pSM, SM_CW);
				}
				break;
			}
		}
	}
}

void terminal_axis(void)
{
	TERMINAL_t *pTERMINAL = TERM_DISP.terminal[TERM_DISP.terminal_ptr];
	SM_t *pSM = &SM[4];

	const uint8_t i = 4;
	const uint8_t j = 3;
	const uint8_t legend = 34;
	const uint8_t text_column = 1;
	const uint8_t start_column = 30;

	TERMINAL_line(pTERMINAL, j, text_column, "PARAMETRY SILNIKA:");
	TERMINAL_line(pTERMINAL, j + 1, text_column, pSM->description);

	TERMINAL_line(pTERMINAL, i + 1, text_column, "ILOSC KROKOW NA OBROT");
	TERMINAL_number(pTERMINAL, i + 1, start_column, pSM->steps_per_revolution, 4);

	TERMINAL_line(pTERMINAL, i + 2, text_column, "MAX FREQ:");
	TERMINAL_number(pTERMINAL, i + 2, start_column, (*pSM).max_freq, 4);

	TERMINAL_line(pTERMINAL, i + 3, text_column, "KROKI DO WYKONANIA:");
	TERMINAL_number(pTERMINAL, i + 3, start_column, (*pSM).steps_to_make, 5);

	TERMINAL_line(pTERMINAL, i + 4, text_column, "KROKI WYKONANE:");
	TERMINAL_number(pTERMINAL, i + 4, start_column, (*pSM).steps_counter, 5);

	TERMINAL_line(pTERMINAL, i + 5, text_column, "F KROKOW:");
	TERMINAL_number(pTERMINAL, i + 5, start_column, (*pSM).step_line->timer->frequency, 5);

	TERMINAL_line(pTERMINAL, i + 6, text_column, "TRYB PRACY:");
	TERMINAL_number(pTERMINAL, i + 6, start_column - 3, (*pSM).work_mode, 5);
	TERMINAL_line(pTERMINAL, i + 6, start_column, "                 ");
	TERMINAL_line(pTERMINAL, i + 6, start_column, SM_WORK_MODE[(*pSM).work_mode]);

	TERMINAL_line(pTERMINAL, i + 7, text_column, "SYS_CORE_CLK");
	TERMINAL_number(pTERMINAL, i + 7, start_column, SYS_CORE_CLOCK, 10);

	TERMINAL_line(pTERMINAL, i + 8, text_column, "PRESKALER:");
	TERMINAL_number(pTERMINAL, i + 8, start_column, LL_TIM_GetPrescaler((*pSM).step_line->timer->TIMx), 5);

	TERMINAL_line(pTERMINAL, i + 9, text_column, "F NA WYJSCIU:");
	TERMINAL_number(pTERMINAL, i + 9, start_column, SYS_CORE_CLOCK / (LL_TIM_GetPrescaler((*pSM).step_line->timer->TIMx) + 1), 10);

	TERMINAL_line(pTERMINAL, i + 10, text_column, "ARR");
	TERMINAL_number(pTERMINAL, i + 10, start_column, LL_TIM_GetAutoReload((*pSM).step_line->timer->TIMx), 5);

	TERMINAL_line(pTERMINAL, i + 11, text_column, "CCR");
	TERMINAL_number(pTERMINAL, i + 11, start_column, LL_TIM_OC_GetCompareCH1((*pSM).step_line->timer->TIMx), 5);

	TERMINAL_line(pTERMINAL, i + 12, text_column, "STEP VALUE:");
	TERMINAL_number(pTERMINAL, i + 12, start_column, (*pSM).step_value, 5);

	TERMINAL_line(pTERMINAL, i + 13, text_column, "LICZBA KROKOOW:");
	TERMINAL_number(pTERMINAL, i + 13, start_column, (*pSM).global_step_counter, 5);

	TERMINAL_line(pTERMINAL, i + 14, text_column, "KROKI CW:");
	TERMINAL_number(pTERMINAL, i + 14, start_column, (*pSM).CW_step_counter, 5);

	TERMINAL_line(pTERMINAL, i + 15, text_column, "KROKI CCW:");
	TERMINAL_number(pTERMINAL, i + 15, start_column, (*pSM).CCW_step_counter, 5);

	TERMINAL_line(pTERMINAL, i + 16, text_column, "LICZBA PELNYCH OBROTOW:");
	TERMINAL_number(pTERMINAL, i + 16, start_column, (*pSM).global_step_counter / (*pSM).steps_per_revolution, 5);

	TERMINAL_line(pTERMINAL, i + 17, text_column, "PWM PUL: ");
	TERMINAL_number(pTERMINAL, i + 17, start_column, GPIO_get_input_pin(pSM->step_line->line), 2);

	if(GPIO_get_input_pin(pSM->step_line->line) == 1)
	{
		TERMINAL_line(pTERMINAL, i + 17, start_column + 5, "PWM ON ");
	}
	else
	{
		TERMINAL_line(pTERMINAL, i + 17, start_column + 5, "PWM OFF");
	}

	TERMINAL_line(pTERMINAL, i + 18, text_column, "PWM DIR: ");
	TERMINAL_number(pTERMINAL, i + 18, start_column, GPIO_get_output_pin(pSM->dir_line), 2);

	if(GPIO_get_output_pin(pSM->dir_line) == SM_CW)
	{
		TERMINAL_line(pTERMINAL, i + 18, start_column + 5, "CW ");
	}
	else
	{
		TERMINAL_line(pTERMINAL, i + 18, start_column + 5, "CCW");
	}

	TERMINAL_line(pTERMINAL, i + 19, text_column, "ILE ITERACJI:");
	TERMINAL_number(pTERMINAL, i + 19, start_column, (*pSM).interval, 5);

	TERMINAL_line(pTERMINAL, i + 20, text_column, "ILE ZLICZONYCH ITERACJI:");
	TERMINAL_number(pTERMINAL, i + 20, start_column, (*pSM).interval_counter, 5);

	//	KRAŃCÓWKA X1:
	TERMINAL_line(pTERMINAL, i + 21, text_column, "KRANCOWKA X1:");
	TERMINAL_number(pTERMINAL, i + 21, start_column, GPIO_get_input_pin(&LIMIT_SWITCH_X1_LINE), 2);

	if(GPIO_get_input_pin(&LIMIT_SWITCH_X1_LINE) == 1)
	{
		TERMINAL_line(pTERMINAL, i + 21, start_column + 5, "AKTYWNA");
	}
	else
	{
		TERMINAL_line(pTERMINAL, i + 21, start_column + 5, "       ");
	}

	//	KRAŃCÓWKA X2:
	TERMINAL_line(pTERMINAL, i + 22, text_column, "KRANCOWKA X2:");
	TERMINAL_number(pTERMINAL, i + 22, start_column, GPIO_get_input_pin(&LIMIT_SWITCH_X2_LINE), 2);

	if(GPIO_get_input_pin(&LIMIT_SWITCH_X2_LINE) == 1)
	{
		TERMINAL_line(pTERMINAL, i + 22, start_column + 5, "AKTYWNA");
	}
	else
	{
		TERMINAL_line(pTERMINAL, i + 22, start_column + 5, "       ");
	}

	//	POZYCJA LOKALNA:
	TERMINAL_line(pTERMINAL, i + 23, text_column, "POLOZENIE WZDLUZ OSI X:");
	TERMINAL_number(pTERMINAL, i + 23, start_column, pSM->local_position, 4);

	//	KRAŃCÓWKA X1:
	TERMINAL_line(pTERMINAL, i + 24, text_column, "POLOZENIE KRANCOWKI X1:");

	if(pSM->flag_limit_switch_1 == 0)
	{
		TERMINAL_line(pTERMINAL, i + 24, start_column, "BRAK WYKRYCIA");
	}
	else
	{
		TERMINAL_line(pTERMINAL, i + 24, start_column, "             ");
		TERMINAL_number(pTERMINAL, i + 24, start_column, pSM->limit_switch_1, 4);
	}

	//	KRAŃCÓWKA X2:
	TERMINAL_line(pTERMINAL, i + 25, text_column, "POLOZENIE KRANCOWKI X2:");

	if(pSM->flag_limit_switch_2 == 0)
	{
		TERMINAL_line(pTERMINAL, i + 25, start_column, "BRAK WYKRYCIA");
	}
	else
	{
		TERMINAL_line(pTERMINAL, i + 25, start_column, "             ");
		TERMINAL_number(pTERMINAL, i + 25, start_column, pSM->limit_switch_2, 4);
	}

	//	PO WYKRYCIU KRAŃCÓWEK:
	TERMINAL_line(pTERMINAL, i + 26, text_column, "ILOSC DOSTEPNYCH POLOZEN:");

	if((pSM->flag_limit_switch_1 == 0) || (pSM->flag_limit_switch_2 == 0))
	{
		TERMINAL_line(pTERMINAL, i + 26, start_column, "BRAK WYKRYCIA");
	}
	else
	{
		TERMINAL_line(pTERMINAL, i + 26, start_column, "             ");
		TERMINAL_number(pTERMINAL, i + 26, start_column, SM_get_step_range(pSM), 4);
	}

	//	POZYCJA GLOBALNA:
	TERMINAL_line(pTERMINAL, i + 27, text_column, "POLOZENIE GLOBALNE:");

	if((pSM->flag_limit_switch_1 == 0) || (pSM->flag_limit_switch_2 == 0))
	{
		TERMINAL_line(pTERMINAL, i + 27, start_column, "BRAK WYKRYCIA");
	}
	else
	{
		TERMINAL_line(pTERMINAL, i + 27, start_column, "             ");
		TERMINAL_number(pTERMINAL, i + 27, start_column, pSM->global_position, 4);
	}

	TERMINAL_line(pTERMINAL, i + 28, text_column, "RODZAJ OBROTOW:");
	TERMINAL_number(pTERMINAL, i + 28, start_column - 3, (*pSM).rotation_mode, 5);
	TERMINAL_line(pTERMINAL, i + 28, start_column, "                 ");
	TERMINAL_line(pTERMINAL, i + 28, start_column, SM_WAY_OF_ROTATION[(*pSM).rotation_mode]);

	TERMINAL_line(pTERMINAL, i + 29, text_column, "TRYB PRACY MOD LINIOWEGO:");
	TERMINAL_number(pTERMINAL, i + 29, start_column - 3, (*pSM).linear_module_mode, 5);
	TERMINAL_line(pTERMINAL, i + 29, start_column, "                 ");
	TERMINAL_line(pTERMINAL, i + 29, start_column, LINEAR_MODULE_MODE[(*pSM).linear_module_mode]);

	//	Legenda:
	TERMINAL_line(pTERMINAL, legend + 2, text_column, "z - STANDBY, x - STOP");
	TERMINAL_line(pTERMINAL, legend + 3, text_column, "r - INC, f - DEC");
	TERMINAL_line(pTERMINAL, legend + 4, text_column, "w s - STEP");
	TERMINAL_line(pTERMINAL, legend + 5, text_column, "q a - CONTINUOUS");
	TERMINAL_line(pTERMINAL, legend + 6, text_column, "ENTER - CW <--> CCW");

	SYS_TICK_DMA_info_time(pTERMINAL, pTERMINAL->row - 2, text_column, start_column);	//	2 LINIE,
}













