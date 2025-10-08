/*
 * irq.c
 *
 *  Created on: 22 paź 2024
 *      Author: asmnop
 */


#include "irq.h"

#include "main_1.h"
#include "software_timers.h"

#include "../../LIB/Inc/buzzer.h"
#include "../../LIB/Inc/DRV8833.h"
#include "../../LIB/Inc/led.h"
#include "../../LIB/Inc/notes.h"
#include "../../LIB/Inc/pwm.h"
#include "../../LIB/Inc/servo.h"
#include "../../LIB/Inc/songs.h"
#include "../../LIB/Inc/universal_functions.h"
#include "../../LIB/Inc/VT100.h"

#define H	32
#define L	16

	uint8_t WS2812_ar[120 + 48 + 48] = {

			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,

			L, L, L, L, L, H, H, H,
			L, L, L, L, L, L, L, L,
			L, L, L, L, L, L, L, L,

			L, L, L, L, L, L, L, L,
			L, L, L, L, L, H, H, H,
			L, L, L, L, L, L, L, L,

			L, L, L, L, L, L, L, L,
			L, L, L, L, L, L, H, H,
			L, L, L, L, L, L, L, L,

			L, L, L, L, L, L, L, L,
			L, L, L, L, L, L, H, H,
			L, L, L, L, L, L, L, L,

			L, L, L, L, L, L, L, L,
			L, L, L, L, L, L, H, H,
			L, L, L, L, L, L, L, L,

			L, L, L, L, L, L, L, L,
			L, L, L, L, L, L, H, H,
			L, L, L, L, L, L, L, L,

			L, L, L, L, L, L, L, L,
			L, L, L, L, L, L, L, L,
			L, L, L, L, L, L, H, H,
	};


const char* MICRO_STEP_[] =
{
	"FULL_STEP",
	"HALF_STEP",
	"QUARTER_STEP",
	"EIGHT_STEP",
	"SIXTEENTH_STEP",
};


const char END_[1] = {255};

void SM_IRQHandler(void)
{
	//	-zliczanie wszystkich możliwych wartości liczników kroków, kroków w kierunku CW,
	//	kroków w kierunku CCW, pozycji lokalnych, globalnych, itp itd,

	uint8_t number = TERM_DISP.instance[TERM_DISP.terminal_ptr];

	//	###########################################################
	//	LICZNIKI ZLICZAJĄCE WEJŚCIA DO PRZERWANIA:
	SM[number].global_step_counter++;

	if(SM_get_direction(&SM[number]) == SM_CW)
	{
		SM[number].CW_step_counter++;
		SM[number].local_position++;

		if((SM[number].flag_limit_switch_1 == 1) && (SM[number].flag_limit_switch_2 == 1))
		{
			SM[number].global_position++;
		}
	}
	else if(SM_get_direction(&SM[number]) == SM_CCW)
	{
		SM[number].CCW_step_counter++;
		SM[number].local_position--;

		if((SM[number].flag_limit_switch_1 == 1) && (SM[number].flag_limit_switch_2 == 1))
		{
			SM[number].global_position--;
		}
	}

	//	SPRAWDZANIE CZY NIE NASTAPIŁO ZLICZENIE POZA ZAKRES WYZNACZONY PRZEZ KRAŃCÓWKI:
	if(  ((SM[number].flag_limit_switch_1 == 1) && (SM[number].local_position == SM[number].limit_switch_1 + SM[number].homing_value)) ||
		 ((SM[number].flag_limit_switch_2 == 1) && (SM[number].local_position == SM[number].limit_switch_2 - SM[number].homing_value)) )
	{
		SM_stop(&SM[number]);
	}

	static uint8_t X1_i = 0;
	if(SM[number].flag_limit_switch_1 == 1 && GPIO_get_input_pin(&LIMIT_SWITCH_X1_LINE) == 1)
	{
		X1_i++;
		if(X1_i > 3){
			SM_stop(&SM[number]);
		}
	}
	else
	{
		X1_i = 0;
	}


	static uint8_t X2_i = 0;
	if(SM[number].flag_limit_switch_2 == 1 && GPIO_get_input_pin(&LIMIT_SWITCH_X2_LINE) == 1)
	{
		X2_i++;
		if(X2_i > 3){
			SM_stop(&SM[number]);
		}
	}
	else
	{
		X2_i = 0;
	}

	//	###########################################################
	if(SM[number].interval != 0)
	{
		if(SM[number].steps_to_make != 0)
		{
			SM[number].steps_counter++;

			if(SM[number].steps_to_make == SM[number].steps_counter)
			{
				SM[number].steps_counter = 0;
				SM[number].interval_counter++;

				if(SM[number].interval_counter == SM[number].interval)
				{
					SM[number].interval_counter = 0;
					SM_stop(&SM[number]);
				}
				else
				{
					if(SM[number].param_table[SM[number].interval_counter][1] == 20000)
					{
						//	TYLKO DLA OBSŁUGI MELODII!!!!!!!!!!!!!!!!
						uint32_t auto_reload = __LL_TIM_CALC_ARR(SYS_CORE_CLOCK, LL_TIM_GetPrescaler(SM[number].step_line->timer->TIMx), 20000) + 1;
						LL_TIM_SetAutoReload(SM[number].step_line->timer->TIMx, auto_reload - 1);
						LL_TIM_OC_SetCompareCH1(SM[number].step_line->timer->TIMx, 0);
						SM[number].steps_to_make = SM[number].param_table[SM[number].interval_counter][2];
					}
					else
					{
						if(SM[number].param_table[SM[number].interval_counter][0] == SM_CW)
						{
							GPIO_set_pin(SM[number].dir_line);
							//return SM_CW;
						}
						//	Jeżeli kierunki obrotów są przeciwne:
						else if(SM[number].param_table[SM[number].interval_counter][0] == SM_CCW)
						{
							GPIO_reset_pin(SM[number].dir_line);
							//return SM_CCW;
						}

						SM_set_freq(&SM[number], SM[number].param_table[SM[number].interval_counter][1]);		//	f
						SM[number].steps_to_make = SM[number].param_table[SM[number].interval_counter][2];		//	steps
					}
				}
			}
		}
	}
}


void DMA1_CH1_IRQHandler(void)	//	DMA Z USART2_TX:
{
	latch_DMA = 1;
	//static uint8_t i = 0;

	//	CIALO OBSLUGI PRZERWANIA OD ZAKOŃCZENIA TRANSFERU n DANYCH:
	if(LL_DMA_IsActiveFlag_TC1(DMA1))
	{


		LL_DMA_ClearFlag_TC1(DMA1);	//	Zdjęcie flagi TC oraz GI,
	}

	//	CIALO OBSLUGI PRZERWANIA OD ZAKOŃCZENIA TRANSFERU n/2 DANYCH:
	if(LL_DMA_IsActiveFlag_HT1(DMA1))
	{
		LL_DMA_ClearFlag_HT1(DMA1);	//	Zdjęcie flagi HT oraz GI,
		//*DMA_USART2_TX.ptr_8 = 'k';
		//*(DMA_USART2_TX.ptr_8+1) = 'k'
		//uint16_t steps_pulser = ENCO_get_steps(&PULSER_1);
		//uint8_t xxx[1] = {steps_pulser};
		//TERMINAL_line(&TERMINAL_SM, 10, 0, ((char*)&steps_pulser));

		//	~~~~~~~~~~~~~~~~~~~~~
		//	TERMINALE DO OBSŁUGI:
		//	~~~~~~~~~~~~~~~~~~~~~
		if(TERM_DISP.foo_term[TERM_DISP.terminal_ptr] != NULL)
		{
			TERM_DISP.foo_term[TERM_DISP.terminal_ptr]();
		}




		/*
		//	~~~~~~~~~~~~~~~~~~~
		//	TERMINAL SILNIK DC:
		//	~~~~~~~~~~~~~~~~~~~
		if(TERMINAL_DC.menu != NULL)
		{
			const uint8_t i = 7;
			const uint8_t info = 15;
			const uint8_t text_column = 1;
			const uint8_t start_column = 20;

			TERMINAL_line(&TERMINAL_DC, i - 3, text_column, "SYS_CORE_CLOCK:");
			TERMINAL_number(&TERMINAL_DC, i - 3, start_column, SYS_CORE_CLOCK / 1000000, 5);
			TERMINAL_line(&TERMINAL_DC, i - 3, start_column + 2, " 000 000");

			TERMINAL_line(&TERMINAL_DC, i - 2, text_column, "PRESKALER:");
			TERMINAL_number(&TERMINAL_DC, i - 2, start_column, LL_TIM_GetPrescaler(DRIVER_1.tim_mode_1->timer->TIMx) + 1, 5);

			TERMINAL_line(&TERMINAL_DC, i - 1, text_column, "ARR: ");
			TERMINAL_number(&TERMINAL_DC, i - 1, start_column, LL_TIM_GetAutoReload(DRIVER_1.tim_mode_1->timer->TIMx) + 1, 5);

			TERMINAL_line(&TERMINAL_DC, i, text_column, "CCR1: ");
			TERMINAL_number(&TERMINAL_DC, i, start_column, DRIVER_1.tim_mode_1->duty, 5);

			TERMINAL_line(&TERMINAL_DC, i + 1, text_column, "CCR2: ");
			TERMINAL_number(&TERMINAL_DC, i + 1, start_column, DRIVER_1.tim_mode_2->duty, 5);

			TERMINAL_line(&TERMINAL_DC, i + 2, text_column, "TRYB PRACY:                              ");
			TERMINAL_line(&TERMINAL_DC, i + 2, start_column, MOTOR_STATE[DRIVER_1.driver_state]);

			TERMINAL_line(&TERMINAL_DC, i + 3, text_column, "KROK:");
			TERMINAL_line(&TERMINAL_DC, i + 3, start_column, "   ");
			TERMINAL_number(&TERMINAL_DC, i + 3, start_column, DRV8833_inc_dec_value(&DRIVER_1, 0), 3);

			TERMINAL_line(&TERMINAL_DC, info, text_column, "q - bieg jalowy, e - hamulec");
			TERMINAL_line(&TERMINAL_DC, info + 1, text_column, "w - inc, s - dec, r - step");
			TERMINAL_line(&TERMINAL_DC, info + 2, text_column, "z - 000, x - 000, c - 000, v - 000");
		}
		//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


			if(TERMINAL_PID.menu != NULL)
			{
				//char tmp_arr[10];
				//TERMINAL_line((&TERMINAL_PID), 5, TERMINAL_PID.place[2], ftoa(MOTOR_1_PID.k_p, tmp_arr, 3));
				//TERMINAL_line((&TERMINAL_PID), 6, TERMINAL_PID.place[2], ftoa(MOTOR_1_PID.k_i, tmp_arr, 3));
				//TERMINAL_line((&TERMINAL_PID), 7, TERMINAL_PID.place[2], ftoa(MOTOR_1_PID.k_d, tmp_arr, 3));


				//TERMINAL_number(&TERMINAL_PID, 10, TERMINAL_PID.place[2], PID_TIMER.frequency, 4);



				//TERMINAL_float(&TERMINAL_PID, 5, 20, MOTOR_1_PID.k_p, 8);
				//TERMINAL_float(&TERMINAL_PID, 6, 20, MOTOR_1_PID.k_i, 8);
				//TERMINAL_float(&TERMINAL_PID, 7, 20, MOTOR_1_PID.k_d, 8);
			}
	*/


	}

	/*
	//	CIALO OBSLUGI PRZERWANIA OD WYSTĄPIENIA BLEDU W TRANSFERZE:
	if(LL_DMA_IsActiveFlag_TE1(DMA1))
	{
		LL_DMA_ClearFlag_TE1(DMA1);	//	Zdjęcie flagi TE1 oraz GI,
	}

	//	CIALO OBSLUGI GLOBALNEGO PRZERWANIA:
	if(LL_DMA_IsActiveFlag_GI1(DMA1))
	{
		LL_DMA_ClearFlag_GI1(DMA1);	//	Zdjęcie wszystkich flag z wybranego kanału,
	}
	*/

	latch_DMA_END = 1;
}

void DMA1_CH3_IRQHandler(void)	//	DMA Z ADC1:
{
	//	CIALO OBSLUGI PRZERWANIA OD ZAKOŃCZENIA TRANSFERU n DANYCH:
	if(LL_DMA_IsActiveFlag_TC1(DMA1))
	{
		LL_DMA_ClearFlag_TC1(DMA1);	//	Zdjęcie flagi TC oraz GI,
	}

	//	CIALO OBSLUGI PRZERWANIA OD ZAKOŃCZENIA TRANSFERU n/2 DANYCH:
	if(LL_DMA_IsActiveFlag_HT1(DMA1))
	{
		LL_DMA_ClearFlag_HT1(DMA1);	//	Zdjęcie flagi HT oraz GI,
	}
}

void DMA2_CH5_IRQHandler(void)	//	WS2812:
{

	static uint16_t counter = 0;
	counter++;

	GPIO_set_pin(&TIM);
	if(counter == 1000)
	{
		//WS2812_light(WS2812_LED_LINE, WS2812_LED_LINE_DATA, 7);
		WS2812_rotate(WS2812_LED_LINE, WS2812_LED_LINE_DATA, 7, 0);
		counter = 0;
	}

	GPIO_reset_pin(&TIM);

	//GPIO_toggle_pin(&TIM);


	//	CIALO OBSLUGI PRZERWANIA OD ZAKOŃCZENIA TRANSFERU n DANYCH:
	if(LL_DMA_IsActiveFlag_TC5(DMA2))
	{
		LL_DMA_ClearFlag_GI5(DMA2);
		//WS2812_ar[52] = L;
		//LL_DMA_ClearFlag_TC5(DMA2);	//	Zdjęcie flagi TC oraz GI,

	}

	//	CIALO OBSLUGI PRZERWANIA OD ZAKOŃCZENIA TRANSFERU n/2 DANYCH:
	if(LL_DMA_IsActiveFlag_HT5(DMA2))
	{
		LL_DMA_ClearFlag_GI5(DMA2);


		//WS2812_ar[52] = H;
		//LL_DMA_ClearFlag_HT5(DMA2);	//	Zdjęcie flagi HT oraz GI,

/*
		DMA_TIM8_CH1.ptr_8 = &WS2812_arrr[0];

		//	Funkcja ustawiająca adresy początkowe źródła i celu oraz rodzaj trybu transmisji:
		LL_DMA_ConfigAddresses(DMA2, LL_DMA_CHANNEL_5,
		(uint32_t)(DMA_TIM8_CH1.ptr_8),											//	Źródło
		(uint32_t)(&TIM8->CCR1),											//	Cel,
		LL_DMA_GetDataTransferDirection(DMA2, LL_DMA_CHANNEL_5));		//	Kierunek,
*/
	}

}


void TIM1_UP_TIM16_IRQHandler(void)	//	DRIVER_1, DRIVER_2:
{
	//	PRZERWANIE OD AKTUALIZACJI:
	if(LL_TIM_IsEnabledIT_UPDATE(TIM1) && LL_TIM_IsActiveFlag_UPDATE(TIM1))
	{
		LL_TIM_ClearFlag_UPDATE(TIM1);

		LL_TIM_OC_SetCompareCH1(DRIVER_1.tim_mode_1->timer->TIMx, DRIVER_1.tim_mode_1->duty);
		LL_TIM_OC_SetCompareCH2(DRIVER_1.tim_mode_1->timer->TIMx, DRIVER_1.tim_mode_2->duty);
		LL_TIM_OC_SetCompareCH3(DRIVER_2.tim_mode_1->timer->TIMx, DRIVER_2.tim_mode_1->duty);
		LL_TIM_OC_SetCompareCH4(DRIVER_2.tim_mode_1->timer->TIMx, DRIVER_2.tim_mode_2->duty);

		/*
		if(DRV8833_get_driver_state(&DRIVER_1) > STANDBY)
		{
			DRV8833_set_driver_state(&DRIVER_1, DRV8833_get_driver_state(&DRIVER_1) - GOING_FORWARD_IDLE);
		}
		if(DRV8833_get_driver_state(&DRIVER_2) > STANDBY)
		{
			DRV8833_set_driver_state(&DRIVER_2, DRV8833_get_driver_state(&DRIVER_2) - GOING_FORWARD_IDLE);
		}
*/
	}
}

void TIM2_IRQHandler(void)	//	SM_1:
{
	//	PRZERWANIE OD ZGODNEGO PORÓWNANIA NA KANALE 1:
	if(LL_TIM_IsEnabledIT_CC1(TIM2) && LL_TIM_IsActiveFlag_CC1(TIM2))
	{
		LL_TIM_ClearFlag_CC1(TIM2);

		SM_IRQHandler();


/*
		if(SM[TERMINAL_SM.instance].steps_to_make != 0)
		{
			SM[TERMINAL_SM.instance].steps_counter++;

			if(SM[TERMINAL_SM.instance].steps_to_make == SM[TERMINAL_SM.instance].steps_counter)
			{
				SM[TERMINAL_SM.instance].steps_counter = 0;
				SM[TERMINAL_SM.instance].steps_to_make = 0;
				SM_stop(&SM[TERMINAL_SM.instance]);
			}
		}
*/


		/*
		//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//	OBSŁUGA PŁYNNEGO ZWIĘKSZANIA PRĘDKOŚCI OBROTOWEJ:
		//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		if((SM[TERMINAL_SM.instance].work_mode == SM_ACCEL) || (SM[TERMINAL_SM.instance].work_mode == SM_DECCEL))
		{
			static uint8_t i = 1;

			if(SM[TERMINAL_SM.instance].steps_to_make != 0)
			{
				SM[TERMINAL_SM.instance].steps_counter++;

				if(SM[TERMINAL_SM.instance].steps_to_make == SM[TERMINAL_SM.instance].steps_counter)
				{
					if(SM[TERMINAL_SM.instance].work_mode == SM_ACCEL)
					{
						i++;
					}
					else if(SM[TERMINAL_SM.instance].work_mode == SM_DECCEL)
					{
						i--;

						if(i == 0)
						{
							SM[TERMINAL_SM.instance].interval = 0;
							SM[TERMINAL_SM.instance].work_mode = SM_STOP;
							SM_stop(&SM[TERMINAL_SM.instance]);
							i = 1;
							return;
						}
					}

					if(i == SM[TERMINAL_SM.instance].interval + 1)
					{
						SM[TERMINAL_SM.instance].work_mode = SM_DECCEL;
						i = SM[TERMINAL_SM.instance].interval;
					}

					SM[TERMINAL_SM.instance].steps_counter = 0;
					SM_set_freq(&SM[TERMINAL_SM.instance], ((SM[TERMINAL_SM.instance].freq_base) * ((i<<1) - 1)));
					SM[TERMINAL_SM.instance].steps_to_make = (((SM[TERMINAL_SM.instance].time_base  * SM[TERMINAL_SM.instance].freq_base * ((i<<1) - 1))) / 1000);
				}
			}
			else
			{
				i++;
				SM[TERMINAL_SM.instance].steps_to_make++;
			}
		}

*/



		/*
		//if(SM[TERMINAL_SM.instance].work_mode == SM_ACCEL)
		{
			//	Jeżeli chcemy zliczać ilość kroków do zatrzymania:
			if(SM[TERMINAL_SM.instance].steps_to_make != 0)
			{
				SM[TERMINAL_SM.instance].steps_counter++;

				if(SM[TERMINAL_SM.instance].steps_to_make == SM[TERMINAL_SM.instance].steps_counter)
				{
					SM[TERMINAL_SM.instance].steps_to_make = 0;
					SM[TERMINAL_SM.instance].steps_counter = 0;
					//SM_stop(&SM[TERMINAL_SM.instance]);
				}
			}
		}
		*/
	}
	//	PRZERWANIE OD ZGODNEGO PORÓWNANIA NA KANALE 2:
	if(LL_TIM_IsEnabledIT_CC2(TIM2) && LL_TIM_IsActiveFlag_CC2(TIM2))
	{
		LL_TIM_ClearFlag_CC2(TIM2);
	}
	//	PRZERWANIE OD ZGODNEGO PORÓWNANIA NA KANALE 3:
	if(LL_TIM_IsEnabledIT_CC3(TIM2) && LL_TIM_IsActiveFlag_CC3(TIM2))
	{
		LL_TIM_ClearFlag_CC3(TIM2);
	}
	//	PRZERWANIE OD ZGODNEGO PORÓWNANIA NA KANALE 4:
	if(LL_TIM_IsEnabledIT_CC4(TIM2) && LL_TIM_IsActiveFlag_CC4(TIM2))
	{
		LL_TIM_ClearFlag_CC4(TIM2);
	}
}

void TIM3_IRQHandler(void)	//	SM_2:
{
	//	PRZERWANIE OD ZGODNEGO PORÓWNANIA NA KANALE 1:
	if(LL_TIM_IsEnabledIT_CC1(TIM3) && LL_TIM_IsActiveFlag_CC1(TIM3))
	{
		LL_TIM_ClearFlag_CC1(TIM3);

		SM_IRQHandler();
	}

	//	PRZERWANIE OD ZGODNEGO PORÓWNANIA NA KANALE 2:
	if(LL_TIM_IsEnabledIT_CC2(TIM3) && LL_TIM_IsActiveFlag_CC2(TIM3))
	{
		LL_TIM_ClearFlag_CC2(TIM3);
/*
		static uint8_t PWM_counter = 0;
		static uint8_t flag_up_down = 1;
		LL_TIM_OC_SetCompareCH2(pTIMx, PWM_counter);
		if(flag_up_down == 1)
			PWM_counter++;
		else
			PWM_counter--;

		if(PWM_counter>100)
		{
			flag_up_down = 0;
		}
		if(PWM_counter == 0)
		{
			flag_up_down = 1;
		}
		*/
	}

	//	PRZERWANIE OD ZGODNEGO PORÓWNANIA NA KANALE 3:
	if(LL_TIM_IsEnabledIT_CC3(TIM3) && LL_TIM_IsActiveFlag_CC3(TIM3))
	{
		LL_TIM_ClearFlag_CC3(TIM3);
		//	PONIŻEJ TO OD WYSWIETLANIA STANU LICZNIKA ENKODERA
		//LL_TIM_OC_SetCompareCH3(pTIMx, LL_TIM_GetCounter(TIM4));
/*
		static uint8_t PWM_counter = 100;
		LL_TIM_OC_SetCompareCH3(pTIMx, PWM_counter);
		PWM_counter--;
		if(PWM_counter == 255)
			PWM_counter = 100;
			*/
	}

	//	PRZERWANIE OD ZGODNEGO PORÓWNANIA NA KANALE 4:
	if(LL_TIM_IsEnabledIT_CC4(TIM3) && LL_TIM_IsActiveFlag_CC4(TIM3))
	{
		LL_TIM_ClearFlag_CC4(TIM3);
		//	PONIŻEJ TO OD WYSWIETLANIA STANU LICZNIKA ENKODERA
		//LL_TIM_OC_SetCompareCH4(pTIMx, LL_TIM_GetCounter(TIM4)/4);
/*
		static uint8_t PWM_counter = 0;
		LL_TIM_OC_SetCompareCH4(pTIMx, PWM_counter);
		PWM_counter++;
		if(PWM_counter>10)
			PWM_counter = 0;
			*/
	}
}

void TIM4_IRQHandler(void)	//	SM_3:
{
	//	PRZERWANIE OD ZGODNEGO PORÓWNANIA NA KANALE 1:
	if(LL_TIM_IsEnabledIT_CC1(TIM4) && LL_TIM_IsActiveFlag_CC1(TIM4))
	{
		LL_TIM_ClearFlag_CC1(TIM4);

		SM_IRQHandler();
	}

	//	PRZERWANIE OD ZGODNEGO PORÓWNANIA NA KANALE 2:
	if(LL_TIM_IsEnabledIT_CC2(TIM4) && LL_TIM_IsActiveFlag_CC2(TIM4))
	{
		LL_TIM_ClearFlag_CC2(TIM4);
/*
		static uint8_t PWM_counter = 0;
		static uint8_t flag_up_down = 1;
		LL_TIM_OC_SetCompareCH2(pTIMx, PWM_counter);
		if(flag_up_down == 1)
			PWM_counter++;
		else
			PWM_counter--;

		if(PWM_counter>100)
		{
			flag_up_down = 0;
		}
		if(PWM_counter == 0)
		{
			flag_up_down = 1;
		}
		*/
	}

	//	PRZERWANIE OD ZGODNEGO PORÓWNANIA NA KANALE 3:
	if(LL_TIM_IsEnabledIT_CC3(TIM4) && LL_TIM_IsActiveFlag_CC3(TIM4))
	{
		LL_TIM_ClearFlag_CC3(TIM4);
		//	PONIŻEJ TO OD WYSWIETLANIA STANU LICZNIKA ENKODERA
		//LL_TIM_OC_SetCompareCH3(pTIMx, LL_TIM_GetCounter(TIM4));
/*
		static uint8_t PWM_counter = 100;
		LL_TIM_OC_SetCompareCH3(pTIMx, PWM_counter);
		PWM_counter--;
		if(PWM_counter == 255)
			PWM_counter = 100;
			*/
	}

	//	PRZERWANIE OD ZGODNEGO PORÓWNANIA NA KANALE 4:
	if(LL_TIM_IsEnabledIT_CC4(TIM4) && LL_TIM_IsActiveFlag_CC4(TIM4))
	{
		LL_TIM_ClearFlag_CC4(TIM4);
		//	PONIŻEJ TO OD WYSWIETLANIA STANU LICZNIKA ENKODERA
		//LL_TIM_OC_SetCompareCH4(pTIMx, LL_TIM_GetCounter(TIM4)/4);
/*
		static uint8_t PWM_counter = 0;
		LL_TIM_OC_SetCompareCH4(pTIMx, PWM_counter);
		PWM_counter++;
		if(PWM_counter>10)
			PWM_counter = 0;
			*/
	}
}

void TIM8_CC_IRQHandler(void)	//	WS2812:
{
	static uint8_t counter = 0;
	//GPIO_toggle_pin(&TIM);

	if(LL_TIM_IsEnabledIT_CC1(TIM8) && LL_TIM_IsActiveFlag_CC1(TIM8))
	{
		LL_TIM_ClearFlag_CC1(TIM8);
		counter++;
	}
}

void TIM8_UP_IRQHandler(void)
{
	static uint8_t counter = 0;
	//GPIO_toggle_pin(&TIM);

	if(LL_TIM_IsEnabledIT_UPDATE(TIM8) && LL_TIM_IsActiveFlag_UPDATE(TIM8))
	{
		LL_TIM_ClearFlag_UPDATE(TIM8);

		if(counter % 3)
		{
			LL_TIM_OC_SetCompareCH1(TIM8, 10);
		}
		else
		{
			LL_TIM_OC_SetCompareCH1(TIM8, 22);
		}

		counter++;
	}
	//GPIO_toggle_pin(&TIM);
}

void TIM4_IRQHandler_(void)
{
	//	PRZERWANIE OD AKTUALIZACJI:
	if(LL_TIM_IsEnabledIT_UPDATE(TIM4) && LL_TIM_IsActiveFlag_UPDATE(TIM4))
	{
		LL_TIM_ClearFlag_UPDATE(TIM4);

		static uint16_t i = 0;
		static uint8_t note_duration = 0;
		static uint8_t note_duration_base = 0;
		static uint16_t last_note = 0;	//	Ostatnio generowana nuta,
		static uint8_t flag_the_same = 0;

		if(note_duration == 0)
		{
			note_duration = NYAN_CAT_song.notes[i + 1];
			//	Obliczenie ile najmniejszych stopni dźwięku trwa dana nuta:
			if(note_duration == 1)			note_duration = NOTE_MIN;
			else if(note_duration == 2)		note_duration = NOTE_MIN>>1;
			else if(note_duration == 4)		note_duration = NOTE_MIN>>2;
			else if(note_duration == 8)		note_duration = NOTE_MIN>>3;
			else if(note_duration == 16)	note_duration = NOTE_MIN>>4;
			else if(note_duration == 32)	note_duration = NOTE_MIN>>5;
			else if(note_duration == 64)	note_duration = NOTE_MIN>>6;
			else if(note_duration == 128)	note_duration = NOTE_MIN>>7;

			else if(note_duration == 24)	note_duration = 6;	//	???

			note_duration_base = note_duration;

			//	KONFIGURACJA SLAVE:
			uint16_t prescal = 1000;
			uint16_t freq = NYAN_CAT_song.notes[i];

			if(freq == last_note && flag_the_same == 0)
			{

				uint16_t prescal = 0;
				uint32_t auto_reload = __LL_TIM_CALC_ARR(SYS_CORE_CLOCK, prescal - 1, freq) + 1;

				LL_TIM_SetPrescaler        (TIM3, prescal - 1);
				LL_TIM_SetAutoReload       (TIM3, auto_reload - 1);
				LL_TIM_OC_SetCompareCH1    (TIM3, (auto_reload>>1) );

				LL_TIM_GenerateEvent_UPDATE(TIM3);
				LL_TIM_ClearFlag_UPDATE    (TIM3);
				note_duration = 1;
				//i = i - 2;
				flag_the_same = 1;

				VT100_EOL();
				VT100_text("PAUZA");
				VT100_EOL();
				//delay_ms(1);
			}
			else
			{
				flag_the_same = 0;

				if(freq == 1)
				{
					freq = 4;
				}

				if(freq == END_SONG)
				{
					LL_TIM_DisableCounter(TIM3);
					LL_TIM_DisableCounter(TIM4);
					LL_TIM_SetCounter(TIM3, 0);
					LL_TIM_SetCounter(TIM4, 0);
					//LL_APB1_GRP1_DisableClock    (LL_APB1_GRP1_PERIPH_TIM3);
					//LL_APB1_GRP1_DisableClock    (LL_APB1_GRP1_PERIPH_TIM4);

					i = 0;
					note_duration = 1;
					note_duration_base = 0;
					last_note = 0;	//	Ostatnio generowana nuta,
					flag_the_same = 0;
				}
				else
				{

					uint32_t auto_reload = __LL_TIM_CALC_ARR(SYS_CORE_CLOCK, prescal - 1, freq) + 1;

					LL_TIM_SetPrescaler        (TIM3, prescal - 1);
					LL_TIM_SetAutoReload       (TIM3, auto_reload - 1);
					LL_TIM_OC_SetCompareCH1    (TIM3, (auto_reload>>1) );

					LL_TIM_GenerateEvent_UPDATE(TIM3);
					LL_TIM_ClearFlag_UPDATE    (TIM3);

					LL_TIM_EnableCounter       (TIM3);
					LL_TIM_EnableCounter       (TIM4);
					i = i + 2;
					last_note = freq;	//	Zapis wygenerowanej nuty, jako ostatnio zagranej,
				}

			}

		}


		VT100_EOL();
		VT100_text("freq - ");
		VT100_number(NYAN_CAT_song.notes[i - 2]);
		VT100_text("czas trwania nuty - ");
		VT100_number(note_duration);
		VT100_text(" / ");
		VT100_number(note_duration_base);
		VT100_EOL();

		note_duration--;


	}
}



/*

void TIM5_IRQHandler(void)		//	IMPULSATOR:
{
	//	Liczniki wystąpień wykonania obsługi przerwania:
	static uint8_t TIM5_IRQ_counter = 0;
	static uint8_t CC1_IRQ_counter = 0;
	static uint8_t CC2_IRQ_counter = 0;
	static uint8_t UPDATE_IRQ_counter = 0;

	TIM5_IRQ_counter++;

	//	PRZERWANIA ZWIĄZANE Z IMPULSATOREM:

	//	PRZERWANIE OD PRZELADOWANIA:
	if(LL_TIM_IsEnabledIT_UPDATE(PULSER_1.TIMx) && LL_TIM_IsActiveFlag_UPDATE(PULSER_1.TIMx))
	{
		LL_TIM_ClearFlag_UPDATE(PULSER_1.TIMx);
		UPDATE_IRQ_counter++;
		if(LL_TIM_GetDirection(PULSER_1.TIMx) == LL_TIM_COUNTERDIRECTION_UP)
		{
			PULSER_1.UPDATE_counter++;
		}
		else
		{
			PULSER_1.UPDATE_counter--;
		}
		//return;
	}

	//	PRZERWANIE OD DETEKCJI WYBRANEGO ZBOCZA NA KANALE 1:
	if(LL_TIM_IsEnabledIT_CC1(TIM5) && LL_TIM_IsActiveFlag_CC1(TIM5))
	{
		CC1_IRQ_counter++;
		LL_TIM_ClearFlag_CC1(TIM5);
		ENCO_set_flag(&PULSER_1);

		PULSER_1.get_counts--;
		if(PULSER_1.get_counts == 0)
		{
			LL_TIM_CC_DisableChannel(PULSER_1.TIMx, LL_TIM_CHANNEL_CH1);
			LL_TIM_EnableIT_CC1(PULSER_1.TIMx);
			LL_TIM_DisableIT_CC1(PULSER_1.TIMx);
		}
	}

	//	PRZERWANIE OD DETEKCJI WYBRANEGO ZBOCZA NA KANALE 2:
	if(LL_TIM_IsEnabledIT_CC2(TIM5) && LL_TIM_IsActiveFlag_CC2(TIM5))
	{
		CC2_IRQ_counter++;
		LL_TIM_ClearFlag_CC2(PULSER_1.TIMx);

		PULSER_1.get_counts--;
		if(PULSER_1.get_counts == 0)
		{
			LL_TIM_CC_DisableChannel(PULSER_1.TIMx, LL_TIM_CHANNEL_CH2);
			LL_TIM_EnableIT_CC2(PULSER_1.TIMx);
			LL_TIM_DisableIT_CC2(PULSER_1.TIMx);
		}
	}


}
*/

uint16_t MONITOR_set_point = 0;
uint16_t MONITOR_counts = 0;
int16_t MONITOR_output_signal = 0;
int16_t MONITOR_PWM_motor = 0;



void USART2_IRQHandler(void)	//	TERMINAL:
{
	//	Liczniki wystąpień wykonania obsługi przerwania:
	static uint8_t USART_IRQ_counter = 0;
	static uint8_t RXNE_IRQ_counter = 0;
	static uint8_t TXE_IRQ_counter = 0;
	static uint8_t TC_IRQ_counter = 0;
	static uint8_t IDLE_IRQ_counter = 0;
	//static uint8_t other_IRQ_counter = 0;

	//static uint8_t *ptr_8 = NULL;	//	Wskaźnik na pierwszy element tablicy z informacją o odebranej danej,
	//static uint32_t tx_buffer_count = 0;


	//static uint8_t DMA_TC_counter = 0;
	//static uint8_t DMA_HT_counter = 0;
	//static uint8_t DMA_IT_counter = 0;


	//	UWAGA - w przerwaniu należy sprawdzać czy przerwania od konkretnego zdarzenia
	//	są włączone ORAZ czy konkretna flaga zajścia zdarzenia jest ustawiona !!!

	USART_IRQ_counter++;

	//	Sprawdzenie, które przerwanie wygenerowało obsługę przerwania:

	//	################################# TXE ##################################
	//	Jeżeli dane zostały przesłane z rejestru nadawczego do bufora nadawczego:
	if(LL_USART_IsEnabledIT_TXE(USART2) && LL_USART_IsActiveFlag_TXE(USART2))
	{
		TXE_IRQ_counter++;	//	Zliczenia zajścia obsługi przerwania,

		LL_USART_TransmitData8(USART2, *USART_2.ptr);	//	Wysył danych, ustawienie flagi TXE,
		USART_change_ptr(&USART_2);					//	Sprawdzenie czy to koniec łańcucha znaków,
	}

/*
 *	PIERWSZA WERSJA

	AKTYWACJA
 *	if(data_read == 'o')
		{
			LL_USART_EnableIT_TXE(USART2);
			ptr_8 = o_text;
		}
 *
	if(LL_USART_IsEnabledIT_TXE(USART2) && LL_USART_IsActiveFlag_TXE(USART2))
	{
		TXE_IRQ_counter++;	//	Zliczenia zajścia obsługi przerwania,

		//	Obliczenie ilości znaków w tablicy, na którą wskazuje wskaźnik,
		uint8_t arr_size = 0;
		do
		{
			if(ptr_8[arr_size] != 0)
			{
				arr_size++;
			}
			else
			{
				break;
			}
		}while(1);

		//	Jeżeli wysłaliśmy ostatni znak:
		if(tx_buffer_count == arr_size)
		{
			tx_buffer_count = 0;
			LL_USART_DisableIT_TXE(USART2);
			return;
		}

		LL_USART_TransmitData8(USART2, ptr_8[tx_buffer_count]);
		tx_buffer_count++;
	}

*/


	//	################################## TC ##################################
	//	Jeżeli dane zostały wysłane z bufora nadawczego ORAZ nie nadano nowych danych:
	if(LL_USART_IsEnabledIT_TC(USART2) && LL_USART_IsActiveFlag_TC(USART2))
	{
		TC_IRQ_counter++;

		LL_USART_ClearFlag_TC(USART2);
		//LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_7);
		//data_read = LL_USART_ReceiveData8(USART2);	//	Odczyt danych,
	}

	//	################################# RXNE #################################
	//	Jeżeli nadeszły nowe dane i oczekują na odczyt:
	if(LL_USART_IsEnabledIT_RXNE(USART2) && LL_USART_IsActiveFlag_RXNE(USART2))
	{
		//	WYJAŚNIENIE:
		//	-po odebraniu danej zgodnej z poszukiwaną, może nastąpić zgoda na włączenie
		//	przerwań od pustego rejestru TDR. Jeśli takowa nastąpi to wygenerowanie zostanie
		//	przerwanie od pustego rejestru TDR, z przyczyny iż obsługa tego zdarzenia została
		//	wyłączona po załadowaniu rejestru TDR ostatnio wysyłaną daną, po jej wysłaniu
		//	nastąpiło ustawienie flagi zajścia zdarzenia ALE nie zostało ono obsłużone
		//	z racji wyłączenia jego obsługi w tymże momencie,

		//	-jeśli otrzymamy informację o nowej danej do odbioru to możemy w obsłudze
		//	przerwania wykonać akcję jeśli odebrana dana jest zgodna z poszukiwaną ALBO
		//	możemy ją zapisać do zmiennej np 'data_read' i następnie w pętli głównej wykonać
		//	odpowiednią akcję. MOŻNA też zapisać daną do tablicy odebranych danych 'RX_data'
		//	i w pętli głównej sprawdzać czy odebrany łańcuch znaków jest zgodny z poszukiwanym
		//	wzorcem,

		latch = 1;

		RXNE_IRQ_counter++;

		uint8_t data_read = LL_USART_ReceiveData8(USART2);	//	Odczyt nowo odebranej danej,
		USART_set_flag_new_data(&USART_2);
		USART_RX_new_data(&USART_2, data_read);

/*
		//	OBSLUGA PRZYCISKÓW:
		if(data_read == 'w')
		{
			USART_RX_new_data(&USART_2, 'w');
			LL_USART_EnableIT_TXE(USART2);
		}
		if(data_read == 'd')
		{
			USART_RX_new_data(&USART_2, 'd');
			LL_USART_EnableIT_TXE(USART2);
		}
		if(data_read == 's')
		{
			USART_RX_new_data(&USART_2, 's');
			LL_USART_EnableIT_TXE(USART2);
		}
		if(data_read == 'a')
		{
			USART_RX_new_data(&USART_2, 'a');
			LL_USART_EnableIT_TXE(USART2);
		}

		if(data_read == 'd')
		{
			LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_7);
			//LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_7, (uint32_t)DMA_buffer, LL_USART_DMA_GetRegAddr(USART2, LL_USART_DMA_REG_DATA_TRANSMIT), LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_7));
			LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_7, sizeof(DMA_buffer_));
			LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_7);
		}
		*/
	}

	//	################################# IDLE #################################
	//	Jeżli nastąpi sytuacja wykrycia stanu bezczynności czyli końca transmisji:
	if(LL_USART_IsEnabledIT_IDLE(USART2) && LL_USART_IsActiveFlag_IDLE(USART2))
	{
		IDLE_IRQ_counter++;

		LL_USART_ClearFlag_IDLE(USART2);
		//GPIO_toggle_pin(&LED_GREEN);
	}

	//LL_DMA_Init(DMAx, Channel, DMA_InitStruct);
}

void USART3_IRQHandler(void)	//	BLUETOOTH:
{
	//	Liczniki wystąpień wykonania obsługi przerwania:
	static uint8_t USART_IRQ_counter = 0;
	static uint8_t RXNE_IRQ_counter = 0;
	static uint8_t TXE_IRQ_counter = 0;
	static uint8_t TC_IRQ_counter = 0;
	static uint8_t IDLE_IRQ_counter = 0;
	//static uint8_t other_IRQ_counter = 0;

	//static uint8_t *ptr_8 = NULL;	//	Wskaźnik na pierwszy element tablicy z informacją o odebranej danej,
	//static uint32_t tx_buffer_count = 0;


	//static uint8_t DMA_TC_counter = 0;
	//static uint8_t DMA_HT_counter = 0;
	//static uint8_t DMA_IT_counter = 0;


	//	UWAGA - w przerwaniu należy sprawdzać czy przerwania od konkretnego zdarzenia
	//	są włączone ORAZ czy konkretna flaga zajścia zdarzenia jest ustawiona !!!

	USART_IRQ_counter++;

	//	Sprawdzenie, które przerwanie wygenerowało obsługę przerwania:

	//	################################# TXE ##################################
	//	Jeżeli dane zostały przesłane z rejestru nadawczego do bufora nadawczego:
	if(LL_USART_IsEnabledIT_TXE(USART3) && LL_USART_IsActiveFlag_TXE(USART3))
	{
		TXE_IRQ_counter++;	//	Zliczenia zajścia obsługi przerwania,

		LL_USART_TransmitData8(USART3, *USART_3.ptr);	//	Wysył danych, ustawienie flagi TXE,
		USART_change_ptr(&USART_3);						//	Sprawdzenie czy to koniec łańcucha znaków,
	}

/*
 *	PIERWSZA WERSJA

	AKTYWACJA
 *	if(data_read == 'o')
		{
			LL_USART_EnableIT_TXE(USART2);
			ptr_8 = o_text;
		}
 *
	if(LL_USART_IsEnabledIT_TXE(USART2) && LL_USART_IsActiveFlag_TXE(USART2))
	{
		TXE_IRQ_counter++;	//	Zliczenia zajścia obsługi przerwania,

		//	Obliczenie ilości znaków w tablicy, na którą wskazuje wskaźnik,
		uint8_t arr_size = 0;
		do
		{
			if(ptr_8[arr_size] != 0)
			{
				arr_size++;
			}
			else
			{
				break;
			}
		}while(1);

		//	Jeżeli wysłaliśmy ostatni znak:
		if(tx_buffer_count == arr_size)
		{
			tx_buffer_count = 0;
			LL_USART_DisableIT_TXE(USART2);
			return;
		}

		LL_USART_TransmitData8(USART2, ptr_8[tx_buffer_count]);
		tx_buffer_count++;
	}

*/


	//	################################## TC ##################################
	//	Jeżeli dane zostały wysłane z bufora nadawczego ORAZ nie nadano nowych danych:
	if(LL_USART_IsEnabledIT_TC(USART3) && LL_USART_IsActiveFlag_TC(USART3))
	{
		TC_IRQ_counter++;

		LL_USART_ClearFlag_TC(USART3);
		//LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_7);
		//data_read = LL_USART_ReceiveData8(USART2);	//	Odczyt danych,
	}

	//	################################# RXNE #################################
	//	Jeżeli nadeszły nowe dane i oczekują na odczyt:
	if(LL_USART_IsEnabledIT_RXNE(USART3) && LL_USART_IsActiveFlag_RXNE(USART3))
	{
		//	WYJAŚNIENIE:
		//	-po odebraniu danej zgodnej z poszukiwaną, może nastąpić zgoda na włączenie
		//	przerwań od pustego rejestru TDR. Jeśli takowa nastąpi to wygenerowanie zostanie
		//	przerwanie od pustego rejestru TDR, z przyczyny iż obsługa tego zdarzenia została
		//	wyłączona po załadowaniu rejestru TDR ostatnio wysyłaną daną, po jej wysłaniu
		//	nastąpiło ustawienie flagi zajścia zdarzenia ALE nie zostało ono obsłużone
		//	z racji wyłączenia jego obsługi w tymże momencie,

		//	-jeśli otrzymamy informację o nowej danej do odbioru to możemy w obsłudze
		//	przerwania wykonać akcję jeśli odebrana dana jest zgodna z poszukiwaną ALBO
		//	możemy ją zapisać do zmiennej np 'data_read' i następnie w pętli głównej wykonać
		//	odpowiednią akcję. MOŻNA też zapisać daną do tablicy odebranych danych 'RX_data'
		//	i w pętli głównej sprawdzać czy odebrany łańcuch znaków jest zgodny z poszukiwanym
		//	wzorcem,

		RXNE_IRQ_counter++;

		uint8_t data_read = LL_USART_ReceiveData8(USART3);	//	Odczyt nowo odebranej danej,
		USART_set_flag_new_data(&USART_3);
		USART_RX_new_data(&USART_3, data_read);
		USART_TX_char(&USART_2, data_read);

/*
		//	OBSLUGA PRZYCISKÓW:
		if(data_read == 'w')
		{
			USART_RX_new_data(&USART_2, 'w');
			LL_USART_EnableIT_TXE(USART2);
		}
		if(data_read == 'd')
		{
			USART_RX_new_data(&USART_2, 'd');
			LL_USART_EnableIT_TXE(USART2);
		}
		if(data_read == 's')
		{
			USART_RX_new_data(&USART_2, 's');
			LL_USART_EnableIT_TXE(USART2);
		}
		if(data_read == 'a')
		{
			USART_RX_new_data(&USART_2, 'a');
			LL_USART_EnableIT_TXE(USART2);
		}

		if(data_read == 'd')
		{
			LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_7);
			//LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_7, (uint32_t)DMA_buffer, LL_USART_DMA_GetRegAddr(USART2, LL_USART_DMA_REG_DATA_TRANSMIT), LL_DMA_GetDataTransferDirection(DMA1, LL_DMA_CHANNEL_7));
			LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_7, sizeof(DMA_buffer_));
			LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_7);
		}
		*/
	}

	//	################################# IDLE #################################
	//	Jeżli nastąpi sytuacja wykrycia stanu bezczynności czyli końca transmisji:
	if(LL_USART_IsEnabledIT_IDLE(USART3) && LL_USART_IsActiveFlag_IDLE(USART3))
	{
		IDLE_IRQ_counter++;

		LL_USART_ClearFlag_IDLE(USART3);
	}
}

void WWDG_IRQHandler(void)
{
	GPIO_set_pin(&LED_GREEN);
	//while(1);
}

void TAMP_STAMP_IRQHandler(void)
{
	GPIO_toggle_pin(&LED_GREEN);
}

void EXTI2_IRQHandler(void)		//	LIMIT_SWITCH_X1_LINE:
{
	//	-stan wysoki oznacza:
	//	--> do linii nie jest podłączone żadne źródło,
	//	--> nastąpiło przesłonięcie czujnika,
	//	-domyślny stan na linii: stan wysoki,
	//	-podczas normalnej pracy na linii panuje stan niski,
	//	-tak więc zbocze rosnące oznacza zajście błędu,

	uint32_t pin_state = GPIO_get_input_pin(&LIMIT_SWITCH_X1_LINE);

	if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_2) != RESET)
	{
		if(LL_EXTI_IsEnabledRisingTrig_0_31(LL_EXTI_LINE_2) && (pin_state == 1))		//	__|""
		{
			LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_2);

			GPIO_set_pin(&LED_GREEN);

			SM_stop(&SM[4]);

			//	Przechwycenie wartości kroków, która oznacza zadziałanie krańcówki:
			if(SM[4].flag_limit_switch_1 == 0)
			{
				//	Dojazd wózka do pozycji bazowej:
				SM_set_steps(&SM[4], SM_CW, 60, SM[4].homing_value);
				SM[4].flag_limit_switch_1 = 1;
				SM[4].limit_switch_1 = SM[4].local_position;

				if((SM[4].flag_limit_switch_1 == 1) && (SM[4].flag_limit_switch_2 == 1))
				{
					SM[4].global_position = 0;
				}

				NVIC_DisableIRQ(EXTI2_IRQn);
			}
			else if((SM[4].flag_limit_switch_1 == 1) && (SM[4].flag_limit_switch_2 == 0))
			{
				SM_set_steps(&SM[4], SM_CW, 60, SM[4].homing_value);
				while(SM[4].steps_to_make != 0);
			}



			//NVIC_DisableIRQ(EXTI2_IRQn);
		}
		else if(LL_EXTI_IsEnabledFallingTrig_0_31(LL_EXTI_LINE_2) && (pin_state == 0))	//	""|__
		{
			LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_2);
		}
	}
}

void EXTI3_IRQHandler(void)		//	LIMIT_SWITCH_X2_LINE:
{
	uint32_t pin_state = GPIO_get_input_pin(&LIMIT_SWITCH_X2_LINE);

	if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_3) != RESET)
	{
		if(LL_EXTI_IsEnabledRisingTrig_0_31(LL_EXTI_LINE_3) && pin_state == 1)
		{
			LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_3);

			GPIO_set_pin(&LED_GREEN);

			SM_stop(&SM[4]);

			//	Przechwycenie wartości kroków, która oznacza zadziałanie krańcówki:
			if(SM[4].limit_switch_2 == 0)
			{
				//	Dojazd wózka do pozycji bazowej:
				SM_set_steps(&SM[4], SM_CCW, 60, SM[4].homing_value);
				SM[4].flag_limit_switch_2 = 1;
				SM[4].limit_switch_2 = SM[4].local_position;

				if((SM[4].flag_limit_switch_1 == 1) && (SM[4].flag_limit_switch_2 == 1))
				{
					SM[4].global_position = SM_get_step_range(&SM[4]);
				}

				NVIC_DisableIRQ(EXTI3_IRQn);
			}
			else if((SM[4].flag_limit_switch_1 == 0) && (SM[4].flag_limit_switch_2 == 1))
			{
				SM_set_steps(&SM[4], SM_CCW, 60, SM[4].homing_value);
				while(SM[4].steps_to_make != 0);
			}

			//NVIC_DisableIRQ(EXTI3_IRQn);
		}
		else if(LL_EXTI_IsEnabledFallingTrig_0_31(LL_EXTI_LINE_3) && pin_state == 0)
		{
			LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_3);
		}
	}
}

void EXTI15_10_IRQHandler(void)	//	USER_BUTTON:
{
	//	USER zwolniony - stan niski,
	//	USER wciśnięty - stan wysoki,

	uint32_t pin_state = GPIO_get_input_pin(&BUTTON_LINE);

	if(LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_13) != RESET)
	{
		if(LL_EXTI_IsEnabledRisingTrig_0_31(LL_EXTI_LINE_13) && pin_state == 1)			//	__|""
		{
			LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_13);
			LL_TIM_OC_SetCompareCH1(TIM16, 0);
			GPIO_set_pin(&LED_GREEN);
		}
		else if(LL_EXTI_IsEnabledFallingTrig_0_31(LL_EXTI_LINE_13) && pin_state == 0)	//	""|__
		{
			LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_13);
			LL_TIM_OC_SetCompareCH1(TIM16, 8);

			GPIO_reset_pin(&LED_GREEN);
		}
	}
}


void IRQ_init(void)
{
	//NVIC_SetPriority(TIM1_UP_TIM16_IRQn, 1);
	//NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);

	//NVIC_SetPriority(ADC1_2_IRQn, 1);
	//NVIC_EnableIRQ(ADC1_2_IRQn);

	NVIC_SetPriority(TIM2_IRQn, 4);
	NVIC_EnableIRQ(TIM2_IRQn);

	NVIC_SetPriority(TIM3_IRQn, 4);
	NVIC_EnableIRQ(TIM3_IRQn);

	NVIC_SetPriority(TIM4_IRQn, 4);
	NVIC_EnableIRQ(TIM4_IRQn);

	NVIC_SetPriority(TIM8_UP_IRQn, 4);
	NVIC_EnableIRQ(TIM8_UP_IRQn);

	NVIC_SetPriority(TIM8_CC_IRQn, 4);
	NVIC_EnableIRQ(TIM8_CC_IRQn);


	//NVIC_SetPriority(USART1_IRQn, 3);
	//NVIC_EnableIRQ(USART1_IRQn);

	NVIC_SetPriority(USART2_IRQn, 2);
	NVIC_EnableIRQ(USART2_IRQn);

	NVIC_SetPriority(USART3_IRQn, 3);
	NVIC_EnableIRQ(USART3_IRQn);

	//NVIC_SetPriority(WWDG_IRQn, 1);
	//NVIC_EnableIRQ(WWDG_IRQn);
}

void EXTI2_X1_init(void)		//	PC2:
{
	//	-włączenie obsługi przerwania od zmiany stanu na krańcówce na osi X,
	//	-przerwanie pochodzi od krańcówki nr 1 - bliższa silnikowi krokowemu,

	//	Włączenie zegara APB2 dla urządzeń peryferyjnych:
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);	//	RCC->APB2ENR->SYSCFGEN

	//	Konfiguracja źródła wejściowego dla przerwania zewnętrznego EXTI:
	LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTC, LL_SYSCFG_EXTI_LINE2);

	//	Włączenie wyzwalania przerwań od zbocza opadającego na liniach od 0 d0 31:
	LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_2);	//	EXTI->FTSR1

	//	Włączenie wyzwalania przerwań od zbocza rosnącego na liniach od 0 d0 31:
	LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_2);

	//	Zezwolenie na przerwanie:
	LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_2);				//	EXTI->IMR1
	//LL_EXTI_GenerateSWI_0_31(LL_EXTI_LINE_13);

	//	Ustawienie priorytetu przerwania
	NVIC_SetPriority(EXTI2_IRQn, 1);

	//	Zdjęcie informacji o tym, że przerwanie zostało zgłoszone ale jest w oczekiwaniu,
	LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_2);
	NVIC_ClearPendingIRQ(EXTI2_IRQn);

	//	Włączenie przerwania
	NVIC_EnableIRQ(EXTI2_IRQn);

	//LL_EXTI_GenerateSWI_0_31(LL_EXTI_LINE_13);
}

void EXTI3_X2_init(void)		//	PC3:
{
	//	-włączenie obsługi przerwania od zmiany stanu na krańcówce na osi X,
	//	-przerwanie pochodzi od krańcówki nr 2 - dalsza silnikowi krokowemu,

	//	Włączenie zegara APB2 dla urządzeń peryferyjnych:
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);	//	RCC->APB2ENR->SYSCFGEN

	//	Konfiguracja źródła wejściowego dla przerwania zewnętrznego EXTI:
	LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTC, LL_SYSCFG_EXTI_LINE3);

	//	Włączenie wyzwalania przerwań od zbocza opadającego na liniach od 0 d0 31:
	LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_3);	//	EXTI->FTSR1

	//	Włączenie wyzwalania przerwań od zbocza rosnącego na liniach od 0 d0 31:
	LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_3);

	//	Zezwolenie na przerwanie:
	LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_3);				//	EXTI->IMR1
	//LL_EXTI_GenerateSWI_0_31(LL_EXTI_LINE_13);

	//	Ustawienie priorytetu przerwania
	NVIC_SetPriority(EXTI3_IRQn, 1);

	//	Zdjęcie informacji o tym, że przerwanie zostało zgłoszone ale jest w oczekiwaniu,
	LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_3);
	NVIC_ClearPendingIRQ(EXTI3_IRQn);

	//	Włączenie przerwania
	NVIC_EnableIRQ(EXTI3_IRQn);

	//LL_EXTI_GenerateSWI_0_31(LL_EXTI_LINE_13);
}

void EXTI_button_init(void)		//	BUTTON:
{
	//	-włączenie obsługi przerwania od zmiany stanu na krańcówce na osi X,
	//	-przerwanie pochodzi od krańcówki nr 2 - dalsza silnikowi krokowemu,

	//	Włączenie zegara APB2 dla urządzeń peryferyjnych:
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);	//	RCC->APB2ENR->SYSCFGEN

	//	Konfiguracja źródła wejściowego dla przerwania zewnętrznego EXTI:
	LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTC, LL_SYSCFG_EXTI_LINE13);

	//	Włączenie wyzwalania przerwań od zbocza opadającego na liniach od 0 d0 31:
	LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_13);	//	EXTI->FTSR1

	//	Włączenie wyzwalania przerwań od zbocza rosnącego na liniach od 0 d0 31:
	LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_13);

	//	Zezwolenie na przerwanie:
	LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_13);				//	EXTI->IMR1
	//LL_EXTI_GenerateSWI_0_31(LL_EXTI_LINE_13);

	//	Ustawienie priorytetu przerwania
	NVIC_SetPriority(EXTI15_10_IRQn, 1);

	//	Zdjęcie informacji o tym, że przerwanie zostało zgłoszone ale jest w oczekiwaniu,
	LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_13);
	NVIC_ClearPendingIRQ(EXTI15_10_IRQn);

	//	Włączenie przerwania
	NVIC_EnableIRQ(EXTI15_10_IRQn);

	//LL_EXTI_GenerateSWI_0_31(LL_EXTI_LINE_13);
}










