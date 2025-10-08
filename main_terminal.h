/*
 * main_terminal.h
 *
 *  Created on: 13 mar 2025
 *      Author: asmnop
 */


#ifndef MAIN_TERMINAL_H_
#define MAIN_TERMINAL_H_


//	/////////////////////////////// TERMINAL EMPTY /////////////////////////////
#define TERM_EMPTY_ROW				20
#define TERM_EMPTY_COLUMN			38
#define TERM_EMPTY_COMMAND_COLUMN	2
#define TERM_EMPTY_TOTAL_COLUMN		(TERM_EMPTY_COLUMN + TERM_EMPTY_COMMAND_COLUMN)
#define TERM_EMPTY_END_COMMAND		9
#define TERM_EMPTY_TOTAL_SIZE		(((TERM_EMPTY_ROW * TERM_EMPTY_TOTAL_COLUMN) - TERM_EMPTY_COMMAND_COLUMN) + TERM_EMPTY_END_COMMAND)

TERMINAL_t TERMINAL_EMPTY =
{
	.row = TERM_EMPTY_ROW,
	.column = TERM_EMPTY_COLUMN,
	.command_column = TERM_EMPTY_COMMAND_COLUMN,
	.total_column = TERM_EMPTY_TOTAL_COLUMN,
	.total_size = TERM_EMPTY_TOTAL_SIZE,
	.menu = NULL,
	//.instance = 0,
	.name = " ### EMPTY SLOT ### ",
};

//	/////////////////////////////// TERMINAL_SM ////////////////////////////////
#define TERM_SM_ROW				35
#define TERM_SM_COLUMN			55
#define TERM_SM_COMMAND_COLUMN	2
#define TERM_SM_TOTAL_COLUMN	(TERM_SM_COLUMN + TERM_SM_COMMAND_COLUMN)
#define TERM_SM_END_COMMAND		9
#define TERM_SM_TOTAL_SIZE		(((TERM_SM_ROW * TERM_SM_TOTAL_COLUMN) - TERM_SM_COMMAND_COLUMN) + TERM_SM_END_COMMAND)

TERMINAL_t TERMINAL_SM =
{
	.row = TERM_SM_ROW,
	.column = TERM_SM_COLUMN,
	.command_column = TERM_SM_COMMAND_COLUMN,
	.total_column = TERM_SM_TOTAL_COLUMN,
	.total_size = TERM_SM_TOTAL_SIZE,
	.menu = NULL,
	//.instance = 1,
	.name = " ### SILNIK KROKOWY ### ",
};

//	/////////////////////////////// STANDARD_TERMINAL ////////////////////////////////
#define TERM_BASE_ROW				20
#define TERM_BASE_COLUMN			48
#define TERM_BASE_COMMAND_COLUMN	2
#define TERM_BASE_TOTAL_COLUMN		(TERM_BASE_COLUMN + TERM_BASE_COMMAND_COLUMN)
#define TERM_BASE_END_COMMAND		9
#define TERM_BASE_TOTAL_SIZE		(((TERM_BASE_ROW * TERM_BASE_TOTAL_COLUMN) - TERM_BASE_COMMAND_COLUMN) + TERM_BASE_END_COMMAND)

TERMINAL_t TERMINAL_BASE =
{
	.row = TERM_BASE_ROW,
	.column = TERM_BASE_COLUMN,
	.command_column = TERM_BASE_COMMAND_COLUMN,
	.total_column = TERM_BASE_TOTAL_COLUMN,
	.total_size = TERM_BASE_TOTAL_SIZE,
	.menu = NULL,
	.name = " ### MENU GLOWNE :) ### ",
};


//	//////////////////////////////// AXIS_TERMINAL /////////////////////////////
#define TERM_AXIS_ROW				43
#define TERM_AXIS_COLUMN			58
#define TERM_AXIS_COMMAND_COLUMN	2
#define TERM_AXIS_TOTAL_COLUMN		(TERM_AXIS_COLUMN + TERM_AXIS_COMMAND_COLUMN)
#define TERM_AXIS_END_COMMAND		9
#define TERM_AXIS_TOTAL_SIZE		(((TERM_AXIS_ROW * TERM_AXIS_TOTAL_COLUMN) - TERM_AXIS_COMMAND_COLUMN) + TERM_AXIS_END_COMMAND)

TERMINAL_t TERMINAL_AXIS =
{
	.row            = TERM_AXIS_ROW,
	.column         = TERM_AXIS_COLUMN,
	.command_column = TERM_AXIS_COMMAND_COLUMN,
	.total_column   = TERM_AXIS_TOTAL_COLUMN,
	.total_size     = TERM_AXIS_TOTAL_SIZE,
	.menu           = NULL,
	.name           = " <<< MODUL LINIOWY - OS X >>> ",
};


//	/////////////////////////////// TERMINAL SONGS ////////////////////////////////
#define TERM_SONGS_ROW				40
#define TERM_SONGS_COLUMN			48
#define TERM_SONGS_COMMAND_COLUMN	2
#define TERM_SONGS_TOTAL_COLUMN		(TERM_SONGS_COLUMN + TERM_SONGS_COMMAND_COLUMN)
#define TERM_SONGS_END_COMMAND		9
#define TERM_SONGS_TOTAL_SIZE		(((TERM_SONGS_ROW * TERM_SONGS_TOTAL_COLUMN) - TERM_SONGS_COMMAND_COLUMN) + TERM_SONGS_END_COMMAND)

TERMINAL_t TERMINAL_SONGS =
{
	.row = TERM_SONGS_ROW,
	.column = TERM_SONGS_COLUMN,
	.command_column = TERM_SONGS_COMMAND_COLUMN,
	.total_column = TERM_SONGS_TOTAL_COLUMN,
	.total_size = TERM_SONGS_TOTAL_SIZE,
	.menu = NULL,
	.name = " ### GRA MUZYKA - MJUSIK SZPILEN :) ### ",
};


//	/////////////////////////////// TERMINAL SERVO /////////////////////////////
#define TERM_SERVO_ROW				40
#define TERM_SERVO_COLUMN			48
#define TERM_SERVO_COMMAND_COLUMN	2
#define TERM_SERVO_TOTAL_COLUMN		(TERM_SERVO_COLUMN + TERM_SERVO_COMMAND_COLUMN)
#define TERM_SERVO_END_COMMAND		9
#define TERM_SERVO_TOTAL_SIZE		(((TERM_SERVO_ROW * TERM_SERVO_TOTAL_COLUMN) - TERM_SERVO_COMMAND_COLUMN) + TERM_SERVO_END_COMMAND)

TERMINAL_t TERMINAL_SERVO =
{
	.row = TERM_SERVO_ROW,
	.column = TERM_SERVO_COLUMN,
	.command_column = TERM_SERVO_COMMAND_COLUMN,
	.total_column = TERM_SERVO_TOTAL_COLUMN,
	.total_size = TERM_SERVO_TOTAL_SIZE,
	.menu = NULL,
	//.instance = 0,
	.name = " ### SERVO MOTOR ### ",
};


//	/////////////////////////////// TERMINAL ADC ///////////////////////////////
#define TERM_ADC_ROW				20
#define TERM_ADC_COLUMN				48
#define TERM_ADC_COMMAND_COLUMN		2
#define TERM_ADC_TOTAL_COLUMN		(TERM_ADC_COLUMN + TERM_ADC_COMMAND_COLUMN)
#define TERM_ADC_END_COMMAND		9
#define TERM_ADC_TOTAL_SIZE			(((TERM_ADC_ROW * TERM_ADC_TOTAL_COLUMN) - TERM_ADC_COMMAND_COLUMN) + TERM_ADC_END_COMMAND)

TERMINAL_t TERMINAL_ADC =
{
	.row            = TERM_ADC_ROW,
	.column         = TERM_ADC_COLUMN,
	.command_column = TERM_ADC_COMMAND_COLUMN,
	.total_column   = TERM_ADC_TOTAL_COLUMN,
	.total_size     = TERM_ADC_TOTAL_SIZE,
	.menu           = NULL,
	.name           = " ### ADC KONWERTER ### ",
};


//	/////////////////////////////// TERMINAL DC ////////////////////////////////
#define TERM_DC_ROW				20
#define TERM_DC_COLUMN			48
#define TERM_DC_COMMAND_COLUMN	2
#define TERM_DC_TOTAL_COLUMN	(TERM_DC_COLUMN + TERM_DC_COMMAND_COLUMN)
#define TERM_DC_END_COMMAND		9
#define TERM_DC_TOTAL_SIZE		(((TERM_DC_ROW * TERM_DC_TOTAL_COLUMN) - TERM_DC_COMMAND_COLUMN) + TERM_DC_END_COMMAND)

TERMINAL_t TERMINAL_DC =
{
	.row = TERM_DC_ROW,
	.column = TERM_DC_COLUMN,
	.command_column = TERM_DC_COMMAND_COLUMN,
	.total_column = TERM_DC_TOTAL_COLUMN,
	.total_size = TERM_DC_TOTAL_SIZE,
	.menu = NULL,
	.name = "SILNIK DC - DRV8833",
};


//	/////////////////////////////// TERMINAL_PID ////////////////////////////////
#define PID_ROW				20
#define PID_COLUMN			58
#define PID_COMMAND_COLUMN	2
#define PID_TOTAL_COLUMN	(PID_COLUMN + PID_COMMAND_COLUMN)
#define PID_END_COMMAND		9
#define PID_TOTAL_SIZE		(((PID_ROW * PID_TOTAL_COLUMN) - PID_COMMAND_COLUMN) + PID_END_COMMAND)


TERMINAL_LINE_t TERMINAL_PID_MENU[20] =
{
	{.text_position = 0, .text = " - ", .number_1_position = 10, .number_1 = 0, .number_2_position = 20, .number_2 = NULL},
	{.text_position = 0, .text = " - ", .number_1_position = 10, .number_1 = 0, .number_2_position = 20, .number_2 = NULL},
	{.text_position = 0, .text = " - ", .number_1_position = 10, .number_1 = 0, .number_2_position = 20, .number_2 = NULL},
	{.text_position = 0, .text = " - ", .number_1_position = 10, .number_1 = 0, .number_2_position = 20, .number_2 = NULL},
	//{.text_position = 0, .text = "K_p - ", .number_1_position = 10, .number_1 = 0, .number_2_position = 20, .number_2 = &MOTOR_1_PID.k_p},
	//{.text_position = 0, .text = "K_i - ", .number_1_position = 10, .number_1 = 0, .number_2_position = 20, .number_2 = &MOTOR_1_PID.k_i},
	//{.text_position = 0, .text = "K_d - ", .number_1_position = 10, .number_1 = 0, .number_2_position = 20, .number_2 = &MOTOR_1_PID.k_d},
};

TERMINAL_t TERMINAL_PID =
{
	.row = PID_ROW,
	.column = PID_COLUMN,
	.command_column = PID_COMMAND_COLUMN,
	.total_column = PID_TOTAL_COLUMN,
	.total_size = PID_TOTAL_SIZE,
	//.menu_item = TERMINAL_MENU_PID,
	.menu = NULL,
	.place[0] = 0,
	.place[1] = 30,
	.place[2] = 20,
	.place[3] = 40,
	.place[4] = 50,

/*
	.line[0] = 	{.text_position = 0, .text = " - ", 		.number_1_position = 10, .number_1 = NULL, .number_2_position = 20, .number_2 = NULL},
	.line[1] = 	{.text_position = 0, .text = " - ", 		.number_1_position = 10, .number_1 = NULL, .number_2_position = 20, .number_2 = NULL},
	.line[2] = 	{.text_position = 0, .text = " - ", 		.number_1_position = 10, .number_1 = NULL, .number_2_position = 20, .number_2 = NULL},
	.line[3] = 	{.text_position = 0, .text = " - ", 		.number_1_position = 10, .number_1 = NULL, .number_2_position = 20, .number_2 = NULL},
	.line[4] = 	{.text_position = 0, .text = " - ", 		.number_1_position = 10, .number_1 = NULL, .number_2_position = 20, .number_2 = NULL},
	.line[5] = 	{.text_position = 0, .text = "K_p - ",		.number_1_position = 10, .number_1 = NULL, .number_2_position = 20, .number_2 = &MOTOR_1_PID.k_p},
	.line[6] = 	{.text_position = 0, .text = "K_i - ",		.number_1_position = 10, .number_1 = NULL, .number_2_position = 20, .number_2 = &MOTOR_1_PID.k_i},
	.line[7] = 	{.text_position = 0, .text = "K_d - ",		.number_1_position = 10, .number_1 = NULL, .number_2_position = 20, .number_2 = &MOTOR_1_PID.k_d},
	.line[8] = 	{.text_position = 0, .text = " - ",			.number_1_position = 10, .number_1 = NULL, .number_2_position = 20, .number_2 = NULL},
	.line[9] = 	{.text_position = 0, .text = "Set point",	.number_1_position = 10, .number_1 = NULL, .number_2_position = 20, .number_2 = &MOTOR_1.set_point},
	.line[10] = {.text_position = 0, .text = "f PID",		.number_1_position = 10, .number_1 = NULL, .number_2_position = 20, .number_2 = &PID_TIMER.frequency},
*/

};


TERMINAL_DISPLAY_t TERM_DISP =
{
	.terminal[0] = &TERMINAL_SM,
	.foo_term[0] = terminal_SM,
	.foo_menu[0] = main_SM,
	.instance[0] = 0,
	.sign[0]     = '1',

	.terminal[1] = &TERMINAL_SM,
	.foo_term[1] = terminal_SM,
	.foo_menu[1] = main_SM,
	.instance[1] = 1,
	.sign[1]     = '2',

	.terminal[2] = &TERMINAL_SM,
	.foo_term[2] = terminal_SM,
	.foo_menu[2] = main_SM,
	.instance[2] = 2,
	.sign[2]     = '3',

	.terminal[3] = &TERMINAL_SM,
	.foo_term[3] = terminal_SM,
	.foo_menu[3] = main_SM,
	.instance[3] = 3,
	.sign[3]     = '4',

	.terminal[4] = &TERMINAL_SERVO,
	.foo_term[4] = terminal_servo,
	.foo_menu[4] = main_servo,
	.instance[4] = 0,
	.sign[4]     = '5',

	.terminal[5] = &TERMINAL_ADC,
	.foo_term[5] = terminal_adc,
	.foo_menu[5] = main_adc,
	.instance[5] = 0,
	.sign[5]     = '6',

	.terminal[6] = &TERMINAL_BASE,
	.foo_term[6] = terminal_base,
	.foo_menu[6] = main_base,
	.instance[6] = 0,
	.sign[6]     = '7',

	.terminal[7] = &TERMINAL_AXIS,
	.foo_term[7] = terminal_axis,
	.foo_menu[7] = main_axis,
	.instance[7] = 4,
	.sign[7]     = '8',

	.terminal[8] = &TERMINAL_EMPTY,
	.foo_term[8] = NULL,
	.foo_menu[8] = NULL,
	.instance[8] = 0,
	.sign[8]     = '9',

	.terminal_ptr = 7,
};



#endif /* MAIN_TERMINAL_H_ */






