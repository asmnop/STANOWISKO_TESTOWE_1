/*
 * SM_main.h
 *
 *  Created on: 25 lut 2025
 *      Author: asmnop
 */


#ifndef MAIN_SM_H_
#define MAIN_SM_H_


#include "../../LIB/Inc/gpio.h"
#include "../../LIB/Inc/pwm.h"
#include "../../LIB/Inc/SM_driver.h"


#define LIMIT_SWITCH_X1_PORT GPIOC
#define LIMIT_SWITCH_X1_PIN  LL_GPIO_PIN_2

LINE_t LIMIT_SWITCH_X1_LINE =
{
	.port        = LIMIT_SWITCH_X1_PORT,
	.pin         = LIMIT_SWITCH_X1_PIN,
	.pull        = LL_GPIO_PULL_UP,				//	NO, UP, DOWN,
	.speed       = LL_GPIO_SPEED_FREQ_LOW,		//	LOW, MEDIUM, HIGH, VERY_HIGH,
	.mode        = LL_GPIO_MODE_INPUT,			//	INPUT, OUTPUT, ALTERNATE, ANALOG,
	.output_type = LL_GPIO_OUTPUT_PUSHPULL,		//	PUSHPULL, OPENDRAIN,
	.AF          = 0,
	.level       = 0,
};


#define LIMIT_SWITCH_X2_PORT GPIOC
#define LIMIT_SWITCH_X2_PIN  LL_GPIO_PIN_3

LINE_t LIMIT_SWITCH_X2_LINE =
{
	.port        = LIMIT_SWITCH_X2_PORT,
	.pin         = LIMIT_SWITCH_X2_PIN,
	.pull        = LL_GPIO_PULL_UP,				//	NO, UP, DOWN,
	.speed       = LL_GPIO_SPEED_FREQ_LOW,		//	LOW, MEDIUM, HIGH, VERY_HIGH,
	.mode        = LL_GPIO_MODE_INPUT,			//	INPUT, OUTPUT, ALTERNATE, ANALOG,
	.output_type = LL_GPIO_OUTPUT_PUSHPULL,		//	PUSHPULL, OPENDRAIN,
	.AF          = 0,
	.level       = 0,
};



//	################################### SM_1 ###################################
#define SM_1_DIR_PORT GPIOA
#define SM_1_DIR_PIN LL_GPIO_PIN_4

LINE_t SM_1_DIR_LINE =
{
	.port = SM_1_DIR_PORT,
	.pin = SM_1_DIR_PIN,
	.pull = LL_GPIO_PULL_NO,					//	NO, UP, DOWN,
	.speed = LL_GPIO_SPEED_FREQ_LOW,			//	LOW, MEDIUM, HIGH, VERY_HIGH,
	.mode = LL_GPIO_MODE_OUTPUT,				//	INPUT, OUTPUT, ALTERNATE, ANALOG,
	.output_type = LL_GPIO_OUTPUT_PUSHPULL,		//	PUSHPULL, OPENDRAIN,
	.AF = 0,
	.level = 0,
};

#define SM_1_STEP_PORT GPIOA
#define SM_1_STEP_PIN LL_GPIO_PIN_0

LINE_t SM_1_STEP_LINE =
{
	.port = SM_1_STEP_PORT,
	.pin = SM_1_STEP_PIN,
	.pull = LL_GPIO_PULL_NO,					//	NO, UP, DOWN,
	.speed = LL_GPIO_SPEED_FREQ_LOW,			//	LOW, MEDIUM, HIGH, VERY_HIGH,
	.mode = LL_GPIO_MODE_ALTERNATE,				//	INPUT, OUTPUT, ALTERNATE, ANALOG,
	.output_type = LL_GPIO_OUTPUT_PUSHPULL,		//	PUSHPULL, OPENDRAIN,
	.AF = LL_GPIO_AF_1,
	.level = 0,
};

//	################################### SM_2 ###################################
#define SM_2_DIR_PORT GPIOA
#define SM_2_DIR_PIN LL_GPIO_PIN_1

LINE_t SM_2_DIR_LINE =
{
	.port = SM_2_DIR_PORT,
	.pin = SM_2_DIR_PIN,
	.pull = LL_GPIO_PULL_NO,					//	NO, UP, DOWN,
	.speed = LL_GPIO_SPEED_FREQ_LOW,			//	LOW, MEDIUM, HIGH, VERY_HIGH,
	.mode = LL_GPIO_MODE_OUTPUT,				//	INPUT, OUTPUT, ALTERNATE, ANALOG,
	.output_type = LL_GPIO_OUTPUT_PUSHPULL,		//	PUSHPULL, OPENDRAIN,
	.AF = 0,
	.level = 0,
};

#define SM_2_STEP_PORT GPIOB
#define SM_2_STEP_PIN LL_GPIO_PIN_4

LINE_t SM_2_STEP_LINE =
{
	.port = SM_2_STEP_PORT,
	.pin = SM_2_STEP_PIN,
	.pull = LL_GPIO_PULL_NO,					//	NO, UP, DOWN,
	.speed = LL_GPIO_SPEED_FREQ_LOW,			//	LOW, MEDIUM, HIGH, VERY_HIGH,
	.mode = LL_GPIO_MODE_ALTERNATE,				//	INPUT, OUTPUT, ALTERNATE, ANALOG,
	.output_type = LL_GPIO_OUTPUT_PUSHPULL,		//	PUSHPULL, OPENDRAIN,
	.AF = LL_GPIO_AF_2,
	.level = 0,
};

//	################################### SM_3 ###################################
#define SM_3_DIR_PORT GPIOB
#define SM_3_DIR_PIN LL_GPIO_PIN_7

LINE_t SM_3_DIR_LINE =
{
	.port = SM_3_DIR_PORT,
	.pin = SM_3_DIR_PIN,
	.pull = LL_GPIO_PULL_NO,					//	NO, UP, DOWN,
	.speed = LL_GPIO_SPEED_FREQ_LOW,			//	LOW, MEDIUM, HIGH, VERY_HIGH,
	.mode = LL_GPIO_MODE_OUTPUT,				//	INPUT, OUTPUT, ALTERNATE, ANALOG,
	.output_type = LL_GPIO_OUTPUT_PUSHPULL,		//	PUSHPULL, OPENDRAIN,
	.AF = 0,
	.level = 0,
};

#define SM_3_STEP_PORT GPIOB
#define SM_3_STEP_PIN LL_GPIO_PIN_6

LINE_t SM_3_STEP_LINE =
{
	.port = SM_3_STEP_PORT,
	.pin = SM_3_STEP_PIN,
	.pull = LL_GPIO_PULL_NO,					//	NO, UP, DOWN,
	.speed = LL_GPIO_SPEED_FREQ_LOW,			//	LOW, MEDIUM, HIGH, VERY_HIGH,
	.mode = LL_GPIO_MODE_ALTERNATE,				//	INPUT, OUTPUT, ALTERNATE, ANALOG,
	.output_type = LL_GPIO_OUTPUT_PUSHPULL,		//	PUSHPULL, OPENDRAIN,
	.AF = LL_GPIO_AF_2,
	.level = 0,
};

//	################################## ENABLE ##################################
#define ENABLE_PORT GPIOB
#define ENABLE_PIN LL_GPIO_PIN_0

LINE_t ENABLE_LINE =
{
	.port = ENABLE_PORT,
	.pin = ENABLE_PIN,
	.pull = LL_GPIO_PULL_NO,					//	NO, UP, DOWN,
	.speed = LL_GPIO_SPEED_FREQ_LOW,			//	LOW, MEDIUM, HIGH, VERY_HIGH,
	.mode = LL_GPIO_MODE_OUTPUT,				//	INPUT, OUTPUT, ALTERNATE, ANALOG,
	.output_type = LL_GPIO_OUTPUT_PUSHPULL,		//	PUSHPULL, OPENDRAIN,
	.AF = 0,
	.level = 0,
};


//	############################## TIMx - SYGNAŁ PWM ###########################
//	SYSCLK = 10 000 000 [Hz],
//	preskaler = 10,
//	PRES_F = 10 000 000 / 10 = 1 000 000

TIM_t SM_1_TIM =
{
	.TIMx = TIM2,									//	Wybór jednostki używanego timera,
	.clock_source = LL_TIM_CLOCKSOURCE_INTERNAL, 	//	Wybór źrdła zegara: INTERNAL, EXT_MODE1, EXT_MODE2,
	.counter_mode = LL_TIM_COUNTERMODE_UP,			//	Kierunek zliczania licznika: UP, DOWN, CENTER_UP, CENTER_DOWN, CENTER_UP_DOWN,
	//.preskaler = 10,								//	Preskaler częstotliwości SYSCLK,
	.frequency = 0,									//	Częstotliwość generowania przerwania w [Hz],
	.NEW_FREQ = 1000000,
};

CHANNEL_t SM_1_CH =
{
	.line = &SM_1_STEP_LINE,
	.timer = &SM_1_TIM,
	.channel = LL_TIM_CHANNEL_CH1,		//	Numer kanału, na który będzie podawany sygnał PWM: CH1, CH1N, CH2, CH2N, CH3, CH3N, CH4, CH4N,
	.work_mode = LL_TIM_OCMODE_PWM2,	//	Wybór trybu pracy: FROZEN, ACTIVE,
	.duty = 0,
};


TIM_t SM_2_TIM =
{
	.TIMx = TIM3,									//	Wybór jednostki używanego timera,
	.clock_source = LL_TIM_CLOCKSOURCE_INTERNAL, 	//	Wybór źrdła zegara: INTERNAL, EXT_MODE1, EXT_MODE2,
	.counter_mode = LL_TIM_COUNTERMODE_UP,			//	Kierunek zliczania licznika: UP, DOWN, CENTER_UP, CENTER_DOWN, CENTER_UP_DOWN,
	//.preskaler = 10,								//	Preskaler częstotliwości SYSCLK,
	.frequency = 0,									//	Częstotliwość generowania przerwania w [Hz],
	.NEW_FREQ = 1000000,
};

CHANNEL_t SM_2_CH =
{
	.line = &SM_2_STEP_LINE,
	.timer = &SM_2_TIM,
	.channel = LL_TIM_CHANNEL_CH1,		//	Numer kanału, na który będzie podawany sygnał PWM: CH1, CH1N, CH2, CH2N, CH3, CH3N, CH4, CH4N,
	.work_mode = LL_TIM_OCMODE_PWM2,	//	Wybór trybu pracy: FROZEN, ACTIVE,
	.duty = 0,
};


TIM_t SM_3_TIM =
{
	.TIMx = TIM4,									//	Wybór jednostki używanego timera,
	.clock_source = LL_TIM_CLOCKSOURCE_INTERNAL, 	//	Wybór źrdła zegara: INTERNAL, EXT_MODE1, EXT_MODE2,
	.counter_mode = LL_TIM_COUNTERMODE_UP,			//	Kierunek zliczania licznika: UP, DOWN, CENTER_UP, CENTER_DOWN, CENTER_UP_DOWN,
	//.preskaler = 10,								//	Preskaler częstotliwości SYSCLK,
	.frequency = 0,									//	Częstotliwość generowania przerwania w [Hz],
	.NEW_FREQ = 1000000,
};

CHANNEL_t SM_3_CH =
{
	.line = &SM_3_STEP_LINE,
	.timer = &SM_3_TIM,
	.channel = LL_TIM_CHANNEL_CH1,		//	Numer kanału, na który będzie podawany sygnał PWM: CH1, CH1N, CH2, CH2N, CH3, CH3N, CH4, CH4N,
	.work_mode = LL_TIM_OCMODE_PWM2,	//	Wybór trybu pracy: FROZEN, ACTIVE,
	.duty = 0,
};

SM_t SM[5] =
{
	[0].description = "SILNIK nr 1 - AABB, z=12, NEOCENE, BI, 40 [Ohm]",
	[0].step_line = &SM_1_CH,
	[0].dir_line = &SM_1_DIR_LINE,
	[0].microstepping_1 = NULL,
	[0].microstepping_2 = NULL,
	[0].microstepping_3 = NULL,
	[0].enable_line = &ENABLE_LINE,
	[0].work_mode = SM_STANDBY,
	[0].microstep_mode = FULL_STEP,
	[0].steps_per_revolution = 96,
	[0].max_freq = 1000,
	[0].steps_counter = 0,
	[0].steps_to_make = 0,
	[0].step_value = 20,
	[0].interval = 0,
	[0].interval_counter = 0,
	[0].global_step_counter = 0,
	[0].local_position = 0,
	[0].CW_step_counter = 0,
	[0].CCW_step_counter = 0,

	[1].description = "SILNIK nr 2 - ABAB, z=12, MITSUMI, BI, 6.4 [Ohm]",
	[1].step_line = &SM_2_CH,
	[1].dir_line = &SM_2_DIR_LINE,
	[1].microstepping_1 = NULL,
	[1].microstepping_2 = NULL,
	[1].microstepping_3 = NULL,
	[1].enable_line = &ENABLE_LINE,
	[1].work_mode = SM_STANDBY,
	[1].microstep_mode = FULL_STEP,
	[1].steps_per_revolution = 48,
	[1].max_freq = 800,
	[1].steps_counter = 0,
	[1].steps_to_make = 0,
	[1].step_value = 20,
	[1].interval = 0,
	[1].interval_counter = 0,

	[2].description = "SILNIK nr 3 - AABB, z=xx, D3NAS, BI, 3.4 [Ohm]",
	[2].step_line = &SM_3_CH,
	[2].dir_line = &SM_3_DIR_LINE,
	[2].microstepping_1 = NULL,
	[2].microstepping_2 = NULL,
	[2].microstepping_3 = NULL,
	[2].enable_line = &ENABLE_LINE,
	[2].work_mode = SM_STANDBY,
	[2].microstep_mode = FULL_STEP,
	[2].steps_per_revolution = 48,
	[2].max_freq = 1600,
	[2].steps_counter = 0,
	[2].steps_to_make = 0,
	[2].step_value = 20,
	[2].interval = 0,
	[2].interval_counter = 0,

	[3].description = "SILNIK nr 4 - A_AB_B, z=14, PF-42 48, UNI, 120 [Ohm]",
	[3].step_line = &SM_3_CH,
	[3].dir_line = &SM_3_DIR_LINE,
	[3].microstepping_1 = NULL,
	[3].microstepping_2 = NULL,
	[3].microstepping_3 = &ENABLE_LINE,
	[3].enable_line = NULL,
	[3].work_mode = SM_STANDBY,
	[3].microstep_mode = FULL_STEP,
	[3].steps_per_revolution = 48,
	[3].max_freq = 100,
	[3].steps_counter = 0,
	[3].steps_to_make = 0,
	[3].step_value = 20,
	[3].interval = 0,
	[3].interval_counter = 0,

	[4].description          = "SHENGYANG MOTOR - 60BYGH7501-N-21DH, MSY-0668-230414",
	[4].step_line            = &SM_1_CH,
	[4].dir_line             = &SM_1_DIR_LINE,
	[4].microstepping_1      = NULL,
	[4].microstepping_2      = NULL,
	[4].microstepping_3      = NULL,
	[4].enable_line          = &ENABLE_LINE,
	[4].work_mode            = SM_STANDBY,
	[4].rotation_mode        = SM_NONE,
	[4].linear_module_mode   = LM_MANUAL,
	[4].microstep_mode       = FULL_STEP,
	[4].steps_per_revolution = 96,
	[4].max_freq             = 600,
	[4].steps_counter        = 0,
	[4].steps_to_make        = 0,
	[4].step_value           = 20,
	[4].interval             = 0,
	[4].interval_counter     = 0,
	[4].global_step_counter  = 0,
	[4].local_position       = 0,
	[4].CW_step_counter      = 0,
	[4].CCW_step_counter     = 0,
	[4].limit_switch_1       = 0,
	[4].limit_switch_2       = 0,
	[4].homing_value         = 10,
	[4].flag_limit_switch_1  = 0,
	[4].flag_limit_switch_2  = 0,
};


#endif /* MAIN_SM_H_ */









