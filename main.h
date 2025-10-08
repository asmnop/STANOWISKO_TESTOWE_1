/*
 * main.h
 *
 *  Created on: Jul 10, 2024
 *      Author: asmnop
 */


#ifndef MAIN_H_
#define MAIN_H_


#include <main_SM.h>
#include "universal_lib.h"
#include "stm32g491xx.h"
#include "stm32g4xx.h"

#include "irq.h"
#include "main_1.h"
#include "main_terminal.h"
#include "stddef.h"

#include "../../LIB/Inc/adc.h"
#include "../../LIB/Inc/buzzer.h"
#include "../../LIB/Inc/dmamux.h"
#include "../../LIB/Inc/DS3231.h"
#include "../../LIB/Inc/gpio.h"
#include "../../LIB/Inc/i2c.h"
#include "../../LIB/Inc/key.h"
#include "../../LIB/Inc/pwm.h"
#include "../../LIB/Inc/servo.h"
#include "../../LIB/Inc/SM_driver.h"
#include "../../LIB/Inc/terminal.h"
#include "../../LIB/Inc/TM1637.h"
#include "../../LIB/Inc/universal_functions.h"
#include "../../LIB/Inc/usart.h"
#include "../../LIB/Inc/VT100.h"
#include "../../LIB/Inc/ws2812.h"



#define TIM_PORT GPIOC
#define TIM_PIN LL_GPIO_PIN_6

LINE_t TIM =
{
	.port = TIM_PORT,
	.pin = TIM_PIN,
	.pull = LL_GPIO_PULL_NO,					//	NO, UP, DOWN,
	.speed = LL_GPIO_SPEED_FREQ_HIGH,			//	LOW, MEDIUM, HIGH, VERY_HIGH,
	.mode = LL_GPIO_MODE_OUTPUT,				//	INPUT, OUTPUT, ALTERNATE, ANALOG,
	.output_type = LL_GPIO_OUTPUT_PUSHPULL,		//	PUSHPULL, OPENDRAIN,
	.AF = 0,
	.level = 1,
};

#define TEST_2_PORT GPIOC
#define TEST_2_PIN LL_GPIO_PIN_7

LINE_t TEST_2 =
{
	.port = TEST_2_PORT,
	.pin = TEST_2_PIN,
	.pull = LL_GPIO_PULL_NO,					//	NO, UP, DOWN,
	.speed = LL_GPIO_SPEED_FREQ_LOW,			//	LOW, MEDIUM, HIGH, VERY_HIGH,
	.mode = LL_GPIO_MODE_ALTERNATE,				//	INPUT, OUTPUT, ALTERNATE, ANALOG,
	.output_type = LL_GPIO_OUTPUT_PUSHPULL,		//	PUSHPULL, OPENDRAIN,
	.AF = LL_GPIO_AF_4,
	.level = 1,
};

#define TEST_3_PORT GPIOC
#define TEST_3_PIN LL_GPIO_PIN_8

LINE_t TEST_3 =
{
	.port = TEST_3_PORT,
	.pin = TEST_3_PIN,
	.pull = LL_GPIO_PULL_NO,					//	NO, UP, DOWN,
	.speed = LL_GPIO_SPEED_FREQ_LOW,			//	LOW, MEDIUM, HIGH, VERY_HIGH,
	.mode = LL_GPIO_MODE_ALTERNATE,				//	INPUT, OUTPUT, ALTERNATE, ANALOG,
	.output_type = LL_GPIO_OUTPUT_PUSHPULL,		//	PUSHPULL, OPENDRAIN,
	.AF = LL_GPIO_AF_4,
	.level = 1,
};

#define TEST_4_PORT GPIOC
#define TEST_4_PIN LL_GPIO_PIN_9

LINE_t TEST_4 =
{
	.port = TEST_4_PORT,
	.pin = TEST_4_PIN,
	.pull = LL_GPIO_PULL_NO,					//	NO, UP, DOWN,
	.speed = LL_GPIO_SPEED_FREQ_LOW,			//	LOW, MEDIUM, HIGH, VERY_HIGH,
	.mode = LL_GPIO_MODE_ALTERNATE,				//	INPUT, OUTPUT, ALTERNATE, ANALOG,
	.output_type = LL_GPIO_OUTPUT_PUSHPULL,		//	PUSHPULL, OPENDRAIN,
	.AF = LL_GPIO_AF_4,
	.level = 1,
};

//	############################################################################
#define LED_GREEN_PORT GPIOA
#define LED_GREEN_PIN LL_GPIO_PIN_5

LINE_t LED_GREEN =
{
	.port = LED_GREEN_PORT,
	.pin = LED_GREEN_PIN,
	.pull = LL_GPIO_PULL_NO,					//	NO, UP, DOWN,
	.speed = LL_GPIO_SPEED_FREQ_LOW,			//	LOW, MEDIUM, HIGH, VERY_HIGH,
	.mode = LL_GPIO_MODE_OUTPUT,				//	INPUT, OUTPUT, ALTERNATE, ANALOG,
	.output_type = LL_GPIO_OUTPUT_PUSHPULL,		//	PUSHPULL, OPENDRAIN,
	.AF = 0,
	.level = 0,
};

#define BUTTON_PORT GPIOC
#define BUTTON_PIN LL_GPIO_PIN_13

LINE_t BUTTON_LINE =
{
	.port = BUTTON_PORT,
	.pin = BUTTON_PIN,
	.pull = LL_GPIO_PULL_NO,					//	NO, UP, DOWN,
	.speed = LL_GPIO_SPEED_FREQ_LOW,			//	LOW, MEDIUM, HIGH, VERY_HIGH,
	.mode = LL_GPIO_MODE_INPUT,					//	INPUT, OUTPUT, ALTERNATE, ANALOG,
	.output_type = LL_GPIO_OUTPUT_PUSHPULL,		//	PUSHPULL, OPENDRAIN,
	.AF = 0,
	.level = 1,
};


#define DCF77_PORT GPIOD
#define DCF77_PIN LL_GPIO_PIN_2

LINE_t DCF77_LINE =
{
	.port = DCF77_PORT,
	.pin = DCF77_PIN,
	.pull = LL_GPIO_PULL_UP,					//	NO, UP, DOWN,
	.speed = LL_GPIO_SPEED_FREQ_LOW,			//	LOW, MEDIUM, HIGH, VERY_HIGH,
	.mode = LL_GPIO_MODE_INPUT,					//	INPUT, OUTPUT, ALTERNATE, ANALOG,
	.output_type = LL_GPIO_OUTPUT_PUSHPULL,		//	PUSHPULL, OPENDRAIN,
	.AF = 0,
	.level = 1,
};


//	############################################################################
#define AIN1_PORT GPIOC
#define AIN1_PIN LL_GPIO_PIN_0

LINE_t AIN1 =
{
	.port = AIN1_PORT,
	.pin = AIN1_PIN,
	.pull = LL_GPIO_PULL_NO,					//	NO, UP, DOWN,
	.speed = LL_GPIO_SPEED_FREQ_LOW,			//	LOW, MEDIUM, HIGH, VERY_HIGH,
	.mode = LL_GPIO_MODE_ALTERNATE,				//	INPUT, OUTPUT, ALTERNATE, ANALOG,
	.output_type = LL_GPIO_OUTPUT_PUSHPULL,		//	PUSHPULL, OPENDRAIN,
	.AF = LL_GPIO_AF_2,
	.level = 0,
};

#define AIN2_PORT GPIOC
#define AIN2_PIN LL_GPIO_PIN_1

LINE_t AIN2 =
{
	.port = AIN2_PORT,
	.pin = AIN2_PIN,
	.pull = LL_GPIO_PULL_NO,					//	NO, UP, DOWN,
	.speed = LL_GPIO_SPEED_FREQ_LOW,			//	LOW, MEDIUM, HIGH, VERY_HIGH,
	.mode = LL_GPIO_MODE_ALTERNATE,				//	INPUT, OUTPUT, ALTERNATE, ANALOG,
	.output_type = LL_GPIO_OUTPUT_PUSHPULL,		//	PUSHPULL, OPENDRAIN,
	.AF = LL_GPIO_AF_2,
	.level = 0,
};

#define BIN1_PORT GPIOC
#define BIN1_PIN LL_GPIO_PIN_2

LINE_t BIN1 =
{
	.port = BIN1_PORT,
	.pin = BIN1_PIN,
	.pull = LL_GPIO_PULL_NO,					//	NO, UP, DOWN,
	.speed = LL_GPIO_SPEED_FREQ_LOW,			//	LOW, MEDIUM, HIGH, VERY_HIGH,
	.mode = LL_GPIO_MODE_ALTERNATE,				//	INPUT, OUTPUT, ALTERNATE, ANALOG,
	.output_type = LL_GPIO_OUTPUT_PUSHPULL,		//	PUSHPULL, OPENDRAIN,
	.AF = LL_GPIO_AF_2,
	.level = 0,
};

#define BIN2_PORT GPIOC
#define BIN2_PIN LL_GPIO_PIN_3

LINE_t BIN2 =
{
	.port = BIN2_PORT,
	.pin = BIN2_PIN,
	.pull = LL_GPIO_PULL_NO,					//	NO, UP, DOWN,
	.speed = LL_GPIO_SPEED_FREQ_LOW,			//	LOW, MEDIUM, HIGH, VERY_HIGH,
	.mode = LL_GPIO_MODE_ALTERNATE,				//	INPUT, OUTPUT, ALTERNATE, ANALOG,
	.output_type = LL_GPIO_OUTPUT_PUSHPULL,		//	PUSHPULL, OPENDRAIN,
	.AF = LL_GPIO_AF_2,
	.level = 0,
};

//	############################ MOTOR + ENKODER + PID #########################
//	SYSCLK = 10 000 000 [Hz],
//	preskaler = 5,
//	P_CLK = 2 000 000
//	frequency = 20 000 [Hz],
//	( ( SYSCLK / preskaler ) / frequency ) = 100 --> rozdzielczość - ARR,

TIM_t MOTOR_TIM =
{
	.TIMx = TIM1,									//	Wybór jednostki używanego timera,
	.clock_source = LL_TIM_CLOCKSOURCE_INTERNAL, 	//	Wybór źrdła zegara: INTERNAL, EXT_MODE1, EXT_MODE2,
	.counter_mode = LL_TIM_COUNTERMODE_UP,			//	Kierunek zliczania licznika: UP, DOWN, CENTER_UP, CENTER_DOWN, CENTER_UP_DOWN,
	//.preskaler = 1,									//	Preskaler częstotliwości SYSCLK,
	.frequency = 20000,								//	Częstotliwość generowania przerwania w [Hz],
	.NEW_FREQ = 10000000,
};

CHANNEL_t MOTOR_1_TIM_CH1 =
{
	.timer = &MOTOR_TIM,
	.channel = LL_TIM_CHANNEL_CH1,		//	Numer kanału, na który będzie podawany sygnał PWM: CH1, CH1N, CH2, CH2N, CH3, CH3N, CH4, CH4N,
	.work_mode = LL_TIM_OCMODE_PWM1,	//	Wybór trybu pracy: FROZEN, ACTIVE,
	.duty = 0,
};

CHANNEL_t MOTOR_1_TIM_CH2 =
{
	.timer = &MOTOR_TIM,
	.channel = LL_TIM_CHANNEL_CH2,		//	Numer kanału, na który będzie podawany sygnał PWM: CH1, CH1N, CH2, CH2N, CH3, CH3N, CH4, CH4N,
	.work_mode = LL_TIM_OCMODE_PWM1,	//	Wybór trybu pracy: FROZEN, ACTIVE,
	.duty = 0,
};

CHANNEL_t MOTOR_2_TIM_CH1 =
{
	.timer = &MOTOR_TIM,
	.channel = LL_TIM_CHANNEL_CH3,		//	Numer kanału, na który będzie podawany sygnał PWM: CH1, CH1N, CH2, CH2N, CH3, CH3N, CH4, CH4N,
	.work_mode = LL_TIM_OCMODE_PWM1,	//	Wybór trybu pracy: FROZEN, ACTIVE,
	.duty = 0,
};

CHANNEL_t MOTOR_2_TIM_CH2 =
{
	.timer = &MOTOR_TIM,
	.channel = LL_TIM_CHANNEL_CH4,		//	Numer kanału, na który będzie podawany sygnał PWM: CH1, CH1N, CH2, CH2N, CH3, CH3N, CH4, CH4N,
	.work_mode = LL_TIM_OCMODE_PWM1,	//	Wybór trybu pracy: FROZEN, ACTIVE,
	.duty = 0,
};

DRV8833_t DRIVER_1 =
{
	.input_1 = &AIN1,
	.input_2 = &AIN2,
	.tim_mode_1 = &MOTOR_1_TIM_CH1,
	.tim_mode_2 = &MOTOR_1_TIM_CH2,
	.driver_state = 0,
};

DRV8833_t DRIVER_2 =
{
	.input_1 = &BIN1,
	.input_2 = &BIN2,
	.tim_mode_1 = &MOTOR_2_TIM_CH1,
	.tim_mode_2 = &MOTOR_2_TIM_CH2,
	.driver_state = 0,
};

DC_DRIVER_t MOTOR_1_DRIVER =
{
	.DRV8833_driver = &DRIVER_1,
	.TB6612FNG_driver = NULL,
};

MOTOR_t MOTOR_1 =
{
	.driver = &MOTOR_1_DRIVER,
	.encoder = NULL,
	.pid = NULL,
	.driver_state = &DRIVER_1.driver_state,	//	Kierunek obrotów,
	//.duty = &DRIVER_1.tim_mode->duty,
	.set_point = 128,		//	Wartość do uzyskania na wyjściu, ilość zmian stanu na liniach enkodera,
	//.number_of_steps = 100,				//	Liczba kroków wypełnienia sygnału PWM, najlepiej 100,
};





//	/////////////////////////////////// USART //////////////////////////////////
//	Numer portu: COM14,
//	Nazwa sesji: STM32 - G4,
#define USART2_TX_PORT GPIOA
#define USART2_TX_PIN LL_GPIO_PIN_2

#define USART2_RX_PORT GPIOA
#define USART2_RX_PIN LL_GPIO_PIN_3

LINE_t USART2_TX =
{
	.port = USART2_TX_PORT,
	.pin = USART2_TX_PIN,
	.pull = LL_GPIO_PULL_NO,				//	NO, UP, DOWN,
	.speed = LL_GPIO_SPEED_FREQ_LOW,		//	LOW, MEDIUM, HIGH, VERY_HIGH,
	.mode = LL_GPIO_MODE_ALTERNATE,			//	INPUT, OUTPUT, ALTERNATE, ANALOG,
	.output_type = LL_GPIO_OUTPUT_PUSHPULL,	//	PUSHPULL, OPENDRAIN,
	.AF = LL_GPIO_AF_7,
};

LINE_t USART2_RX =
{
	.port = USART2_RX_PORT,
	.pin = USART2_RX_PIN,
	.pull = LL_GPIO_PULL_NO,				//	NO, UP, DOWN,
	.speed = LL_GPIO_SPEED_FREQ_LOW,		//	LOW, MEDIUM, HIGH, VERY_HIGH,
	.mode = LL_GPIO_MODE_ALTERNATE,			//	INPUT, OUTPUT, ALTERNATE, ANALOG,
	.output_type = LL_GPIO_OUTPUT_PUSHPULL,	//	PUSHPULL, OPENDRAIN,
	.AF = LL_GPIO_AF_7,
};

USART_t USART_2 =
{
	.pUSARTx = USART2,
	.clock_source = LL_RCC_USART2_CLKSOURCE_PCLK1,
	.periphs = LL_APB1_GRP1_PERIPH_USART2,
	.data_width = LL_USART_DATAWIDTH_8B,
	.parity = LL_USART_PARITY_NONE,
	.stop_bits = LL_USART_STOPBITS_1,
	.over_sampling = LL_USART_OVERSAMPLING_16,
	.baud_rate = 460800,

	//.data = 0,
	.ptr = NULL,
	.flag = 0,
	.buf = 10,
	.RX_data[0] = 0,
};


//	///////////////////////////////// USART_3 //////////////////////////////////
//	Numer portu: COM4,
//	Nazwa sesji: BTM-222,
#define USART3_TX_PORT GPIOB
#define USART3_TX_PIN LL_GPIO_PIN_9

#define USART3_RX_PORT GPIOB
#define USART3_RX_PIN LL_GPIO_PIN_8

LINE_t USART3_TX =
{
	.port = USART3_TX_PORT,
	.pin = USART3_TX_PIN,
	.pull = LL_GPIO_PULL_NO,				//	NO, UP, DOWN,
	.speed = LL_GPIO_SPEED_FREQ_LOW,		//	LOW, MEDIUM, HIGH, VERY_HIGH,
	.mode = LL_GPIO_MODE_ALTERNATE,			//	INPUT, OUTPUT, ALTERNATE, ANALOG,
	.output_type = LL_GPIO_OUTPUT_PUSHPULL,	//	PUSHPULL, OPENDRAIN,
	.AF = LL_GPIO_AF_7,
};

LINE_t USART3_RX =
{
	.port = USART3_RX_PORT,
	.pin = USART3_RX_PIN,
	.pull = LL_GPIO_PULL_NO,				//	NO, UP, DOWN,
	.speed = LL_GPIO_SPEED_FREQ_LOW,		//	LOW, MEDIUM, HIGH, VERY_HIGH,
	.mode = LL_GPIO_MODE_ALTERNATE,			//	INPUT, OUTPUT, ALTERNATE, ANALOG,
	.output_type = LL_GPIO_OUTPUT_PUSHPULL,	//	PUSHPULL, OPENDRAIN,
	.AF = LL_GPIO_AF_7,
};

USART_t USART_3 =
{
	.pUSARTx = USART3,
	.clock_source = LL_RCC_USART3_CLKSOURCE_PCLK1,
	.periphs = LL_APB1_GRP1_PERIPH_USART3,
	.data_width = LL_USART_DATAWIDTH_8B,
	.parity = LL_USART_PARITY_NONE,
	.stop_bits = LL_USART_STOPBITS_1,
	.over_sampling = LL_USART_OVERSAMPLING_16,
	.baud_rate = 9600,

	//.data = 0,
	.ptr = NULL,
	.flag = 0,
	.buf = 10,
	.RX_data[0] = 0,
};


//	///////////////////////////////// SERVO //////////////////////////////////
#define SERVO_PORT GPIOB
#define SERVO_PIN LL_GPIO_PIN_14

LINE_t SERVO_LINE =
{
	.port = SERVO_PORT,
	.pin = SERVO_PIN,
	.pull = LL_GPIO_PULL_NO,					//	NO, UP, DOWN,
	.speed = LL_GPIO_SPEED_FREQ_LOW,			//	LOW, MEDIUM, HIGH, VERY_HIGH,
	.mode = LL_GPIO_MODE_ALTERNATE,				//	INPUT, OUTPUT, ALTERNATE, ANALOG,
	.output_type = LL_GPIO_OUTPUT_PUSHPULL,		//	PUSHPULL, OPENDRAIN,
	.AF = LL_GPIO_AF_1,
	.level = 0,
};

//	############################## TIMx - SYGNAŁ PWM ###########################
//	SYSCLK = 10 000 000 [Hz],
//	preskaler = 10,
//	PRES_F = 10 000 000 / 10 = 1 000 000

TIM_t SERVO_TIM =
{
	.TIMx = TIM15,									//	Wybór jednostki używanego timera,
	.clock_source = LL_TIM_CLOCKSOURCE_INTERNAL, 	//	Wybór źrdła zegara: INTERNAL, EXT_MODE1, EXT_MODE2,
	.counter_mode = LL_TIM_COUNTERMODE_UP,			//	Kierunek zliczania licznika: UP, DOWN, CENTER_UP, CENTER_DOWN, CENTER_UP_DOWN,
	.NEW_FREQ = 50000,
	.frequency = 50,								//	Częstotliwość generowania przerwania w [Hz],
};

CHANNEL_t SERVO_CH =
{
	.line = &SERVO_LINE,
	.timer = &SERVO_TIM,
	.channel = LL_TIM_CHANNEL_CH1,		//	Numer kanału, na który będzie podawany sygnał PWM: CH1, CH1N, CH2, CH2N, CH3, CH3N, CH4, CH4N,
	.work_mode = LL_TIM_OCMODE_PWM1,	//	Wybór trybu pracy: FROZEN, ACTIVE,
	.duty = 26,
};

#define SERVO_MIN_VALUE		26
#define SERVO_MAX_VALUE		120
#define SERVO_TOTAL_SIZE	(SERVO_MAX_VALUE - SERVO_MIN_VALUE)

SERVO_t SERVO =
{
	.signal_line = &SERVO_CH,
	.min_value = SERVO_MIN_VALUE,
	.max_value = SERVO_MAX_VALUE,
	.total_size = SERVO_TOTAL_SIZE,
	.data[SERVO_TOTAL_SIZE<<1] = 0,
};



//	############################################################################
#define SM1_PORT GPIOC
#define SM1_PIN  LL_GPIO_PIN_12

LINE_t SM1 =
{
	.port = SM1_PORT,
	.pin = SM1_PIN,
	.pull = LL_GPIO_PULL_NO,					//	NO, UP, DOWN,
	.speed = LL_GPIO_SPEED_FREQ_LOW,			//	LOW, MEDIUM, HIGH, VERY_HIGH,
	.mode = LL_GPIO_MODE_OUTPUT,				//	INPUT, OUTPUT, ALTERNATE, ANALOG,
	.output_type = LL_GPIO_OUTPUT_PUSHPULL,		//	PUSHPULL, OPENDRAIN,
	.AF = 0,
	.level = 0,
};

#define SM2_PORT GPIOC
#define SM2_PIN  LL_GPIO_PIN_10

LINE_t SM2 =
{
	.port = SM2_PORT,
	.pin = SM2_PIN,
	.pull = LL_GPIO_PULL_NO,					//	NO, UP, DOWN,
	.speed = LL_GPIO_SPEED_FREQ_LOW,			//	LOW, MEDIUM, HIGH, VERY_HIGH,
	.mode = LL_GPIO_MODE_OUTPUT,				//	INPUT, OUTPUT, ALTERNATE, ANALOG,
	.output_type = LL_GPIO_OUTPUT_PUSHPULL,		//	PUSHPULL, OPENDRAIN,
	.AF = 0,
	.level = 0,
};

#define SM3_PORT GPIOC
#define SM3_PIN  LL_GPIO_PIN_4

LINE_t SM3 =
{
	.port = SM3_PORT,
	.pin = SM3_PIN,
	.pull = LL_GPIO_PULL_NO,					//	NO, UP, DOWN,
	.speed = LL_GPIO_SPEED_FREQ_LOW,			//	LOW, MEDIUM, HIGH, VERY_HIGH,
	.mode = LL_GPIO_MODE_OUTPUT,				//	INPUT, OUTPUT, ALTERNATE, ANALOG,
	.output_type = LL_GPIO_OUTPUT_PUSHPULL,		//	PUSHPULL, OPENDRAIN,
	.AF = 0,
	.level = 0,
};

//	///////////////////////////////// DMA_USART2_TX ////////////////////////////
DMA_t DMA_USART2_TX =
{
	.DMAx            = DMA1,								//	Jednostka DMA,
	.channel         = LL_DMA_CHANNEL_1,					//	Kanał, który obsługuje źródło,
	.periph_request  = LL_DMAMUX_REQ_USART2_TX, 			//	Źródło, peryferium, z którego korzystamy,
	.direction       = LL_DMA_DIRECTION_MEMORY_TO_PERIPH,	//	Rodzaj transmisji,
	.priority        = LL_DMA_PRIORITY_HIGH,				//	Priorytet transmisji,
	.work_mode       = LL_DMA_MODE_CIRCULAR,				//	Tryb pracy,
	.periph_inc_mode = LL_DMA_PERIPH_NOINCREMENT,	//	Tryb obsługi zmiany adresu w peryferium,
	.memory_inc_mode = LL_DMA_MEMORY_INCREMENT,		//	Tryb obsługi zmiany adresu w pamięci,
	.periph_size     = LL_DMA_PDATAALIGN_BYTE,		//	Rozmiar elementu w peryferium,
	.memory_size     = LL_DMA_MDATAALIGN_BYTE,		//	Rozmiar elementu w pamięci,
	.enable_IT_TC    = 0,		//	Zezwolenie na obsługę przerwania od TC,
	.enable_IT_HT    = 1,		//	Zezwolenie na obsługę przerwania od HT,
	.enable_IT_TE    = 0,		//	Zezwolenie na obsługę przerwania od TE,
	.IT_priority     = 10,		//	Priorytet przerwania,
	.number_of_data  = 10,		//	Ilość danych do transmisji,
	.average[0]      = 0,
	.average[1]      = 0,
	.flag            = 0,
	.ptr_8           = NULL,
	.ptr_16          = NULL,
	.ptr_32          = NULL,
};

//	////////////////////////////////// DMA_ADC /////////////////////////////////
DMA_t DMA_ADC =
{
	.DMAx            = DMA1,								//	Jednostka DMA,
	.channel         = LL_DMA_CHANNEL_3,					//	Kanał, który obsługuje źródło,
	.periph_request  = LL_DMAMUX_REQ_ADC2, 					//	Źródło, peryferium, z którego korzystamy,
	.direction       = LL_DMA_DIRECTION_PERIPH_TO_MEMORY,	//	Rodzaj transmisji,
	.priority        = LL_DMA_PRIORITY_HIGH,				//	Priorytet transmisji,
	.work_mode       = LL_DMA_MODE_CIRCULAR,				//	Tryb pracy,
	.periph_inc_mode = LL_DMA_PERIPH_NOINCREMENT,	//	Tryb obsługi zmiany adresu w peryferium,
	.memory_inc_mode = LL_DMA_MEMORY_INCREMENT,		//	Tryb obsługi zmiany adresu w pamięci,
	.periph_size     = LL_DMA_PDATAALIGN_WORD,		//	Rozmiar elementu w peryferium,
	.memory_size     = LL_DMA_MDATAALIGN_WORD,		//	Rozmiar elementu w pamięci,
	.enable_IT_TC    = 0,		//	Zezwolenie na obsługę przerwania od TC,
	.enable_IT_HT    = 0,		//	Zezwolenie na obsługę przerwania od HT,
	.enable_IT_TE    = 0,		//	Zezwolenie na obsługę przerwania od TE,
	.IT_priority     = 10,		//	Priorytet przerwania,
	.number_of_data  = 4,		//	Ilość danych do transmisji,
	.average[0]      = 0,
	.average[1]      = 0,
	.flag            = 0,
	.ptr_8           = NULL,
	.ptr_16          = NULL,
	.ptr_32          = &ADC_2.data_received[0],
};

//	///////////////////////////////// DMA_TIM15_CH1 ////////////////////////////
DMA_t DMA_TIM15_CH1 =
{
	.DMAx = DMA1,									//	Jednostka DMA,
	.channel = LL_DMA_CHANNEL_2,					//	Kanał, który obsługuje źródło,
	.periph_request = LL_DMAMUX_REQ_TIM15_CH1, 		//	Źródło, peryferium, z którego korzystamy,
	.direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH,	//	Rodzaj transmisji,
	.priority = LL_DMA_PRIORITY_HIGH,				//	Priorytet transmisji,
	.work_mode = LL_DMA_MODE_CIRCULAR,				//	Tryb pracy,
	.periph_inc_mode = LL_DMA_PERIPH_NOINCREMENT,	//	Tryb obsługi zmiany adresu w peryferium,
	.memory_inc_mode = LL_DMA_MEMORY_INCREMENT,		//	Tryb obsługi zmiany adresu w pamięci,
	.periph_size = LL_DMA_PDATAALIGN_HALFWORD,		//	Rozmiar elementu w peryferium,
	.memory_size = LL_DMA_MDATAALIGN_HALFWORD,		//	Rozmiar elementu w pamięci,
	.enable_IT_TC = 0,		//	Zezwolenie nPrioryteta obsługę przerwania od TC,
	.enable_IT_HT = 0,		//	Zezwolenie na obsługę przerwania od HT,
	.enable_IT_TE = 0,		//	Zezwolenie na obsługę przerwania od TE,
	.IT_priority  = 10,		//	Priorytet przerwania,
	.number_of_data = 10,	//	Ilość danych do transmisji,
	.average[0] = 0,
	.average[1] = 0,
	.flag = 0,
	.ptr_8 = NULL,
	.ptr_16 = NULL,
	.ptr_32 = NULL,
};



//	//////////////////////////////////// ADC ///////////////////////////////////
ADC_t ADC_2 =
{
	.pADCx = ADC2,
	.V_DDA = 3030,

	.resolution     = LL_ADC_RESOLUTION_8B,				//	6, 8, 10, 12,
	.data_alignment = LL_ADC_DATA_ALIGN_RIGHT,			//	RIGHT, LEFT
	.trigger_source = LL_ADC_REG_TRIG_SOFTWARE,			//	SOFTWARE, ...
	.mode           = LL_ADC_REG_CONV_CONTINUOUS,		//	SINGLE, CONTINUOUS,
	.overrun        = LL_ADC_REG_OVR_DATA_OVERWRITTEN,	//	PRESERVED, OVERWRITTEN,

	.sequencer_length   = LL_ADC_REG_SEQ_SCAN_ENABLE_4RANKS,	//	DISABLE, ENABLE_xRANKS - x=<2, 16>,

	.rank[0]            = LL_ADC_REG_RANK_1,
	.channel[0]         = LL_ADC_CHANNEL_3,					//	0...18, VREFINT, TEMPSENSOR, VBAT,
	.sampling_time[0]   = LL_ADC_SAMPLINGTIME_640CYCLES_5,	//	2, 6, 12, 24, 47, 92, 247, 940,

	.rank[1]            = LL_ADC_REG_RANK_2,
	.channel[1]         = LL_ADC_CHANNEL_4,					//	0...18, VREFINT, TEMPSENSOR, VBAT,
	.sampling_time[1]   = LL_ADC_SAMPLINGTIME_640CYCLES_5,	//	2, 6, 12, 24, 47, 92, 247, 940,

	.rank[2]            = LL_ADC_REG_RANK_3,
	.channel[2]         = LL_ADC_CHANNEL_14,				//	0...18, VREFINT, TEMPSENSOR, VBAT,
	.sampling_time[2]   = LL_ADC_SAMPLINGTIME_640CYCLES_5,	//	2, 6, 12, 24, 47, 92, 247, 940,

	.rank[3]            = LL_ADC_REG_RANK_4,
	.channel[3]         = LL_ADC_CHANNEL_15,				//	0...18, VREFINT, TEMPSENSOR, VBAT,
	.sampling_time[3]   = LL_ADC_SAMPLINGTIME_640CYCLES_5,	//	2, 6, 12, 24, 47, 92, 247, 940,

	.enable_IT_EOC = 0,
	.enable_IT_EOS = 0,
	.enable_IT_OVR = 0,
};



//	############################################################################
#define WS2812_PORT GPIOA
#define WS2812_PIN  LL_GPIO_PIN_15

LINE_t WS2812_LINE =
{
	.port        = WS2812_PORT,
	.pin         = WS2812_PIN,
	.pull        = LL_GPIO_PULL_NO,				//	NO, UP, DOWN,
	.speed       = LL_GPIO_SPEED_FREQ_HIGH,		//	LOW, MEDIUM, HIGH, VERY_HIGH,
	.mode        = LL_GPIO_MODE_ALTERNATE,		//	INPUT, OUTPUT, ALTERNATE, ANALOG,
	.output_type = LL_GPIO_OUTPUT_PUSHPULL,		//	PUSHPULL, OPENDRAIN,
	.AF          = LL_GPIO_AF_2,
	.level       = 0,
};

TIM_t WS2812_TIM =
{
	.TIMx = TIM8,									//	Wybór jednostki używanego timera,
	.clock_source = LL_TIM_CLOCKSOURCE_INTERNAL, 	//	Wybór źrdła zegara: INTERNAL, EXT_MODE1, EXT_MODE2,
	.counter_mode = LL_TIM_COUNTERMODE_UP,			//	Kierunek zliczania licznika: UP, DOWN, CENTER_UP, CENTER_DOWN, CENTER_UP_DOWN,
	.NEW_FREQ = SYS_CORE_CLOCK,
	.frequency = 800000,							//	Częstotliwość generowania przerwania w [Hz],

};

CHANNEL_t WS2812_TIM_CH1 =
{
	.line = &WS2812_LINE,
	.timer = &WS2812_TIM,
	.channel = LL_TIM_CHANNEL_CH1,		//	Numer kanału, na który będzie podawany sygnał PWM: CH1, CH1N, CH2, CH2N, CH3, CH3N, CH4, CH4N,
	.work_mode = LL_TIM_OCMODE_PWM1,	//	Wybór trybu pracy: FROZEN, ACTIVE,
	.duty = 0,
};

WS2812_t WS2812 =
{
	.signal_line = &WS2812_TIM_CH1,
};

//	///////////////////////////////// WS2812_CH1 ///////////////////////////////
#define WS2812_LEDS_NUMBER	7
#define WS2812_LEDS_ARR		((WS2812_RESET + WS2812_LEDS_NUMBER) * 24)

WS2812_LED_t WS2812_LED_LINE[WS2812_LEDS_NUMBER] =
{
	{0, 0, 0},
	{2, 0, 0},
	{0, 2, 0},
	{0, 0, 2},
	{0, 0, 0},
	{0, 0, 0},
	{0, 0, 0},
};

WS2812_TO_SEND_t WS2812_LED_LINE_DATA[WS2812_RESET + WS2812_LEDS_NUMBER];

DMA_t DMA_TIM8_CH1 =
{
	.DMAx            = DMA2,								//	Jednostka DMA,
	.pCHANNEL        = &WS2812_TIM_CH1,
	.channel         = LL_DMA_CHANNEL_5,					//	Kanał, który obsługuje źródło,
	.periph_request  = LL_DMAMUX_REQ_TIM8_CH1, 				//	Źródło, peryferium, z którego korzystamy,
	.direction       = LL_DMA_DIRECTION_MEMORY_TO_PERIPH,	//	Rodzaj transmisji,
	.priority        = LL_DMA_PRIORITY_VERYHIGH,			//	Priorytet transmisji,
	.work_mode       = LL_DMA_MODE_CIRCULAR,				//	Tryb pracy,
	.periph_inc_mode = LL_DMA_PERIPH_NOINCREMENT,	//	Tryb obsługi zmiany adresu w peryferium,
	.memory_inc_mode = LL_DMA_MEMORY_INCREMENT,		//	Tryb obsługi zmiany adresu w pamięci,
	.periph_size     = LL_DMA_PDATAALIGN_HALFWORD,	//	Rozmiar elementu w peryferium,
	.memory_size     = LL_DMA_MDATAALIGN_BYTE,		//	Rozmiar elementu w pamięci,
	.enable_IT_TC    = 1,				//	Zezwolenie nPrioryteta obsługę przerwania od TC,
	.enable_IT_HT    = 1,				//	Zezwolenie na obsługę przerwania od HT,
	.enable_IT_TE    = 0,				//	Zezwolenie na obsługę przerwania od TE,
	.IT_priority     = 8,				//	Priorytet przerwania,
	.number_of_data  = WS2812_LEDS_ARR,	//	Ilość danych do transmisji,
	.average[0]      = 0,
	.average[1]      = 0,
	.flag            = 0,
	.ptr_8           = (uint8_t*)WS2812_LED_LINE_DATA,
	.ptr_16          = NULL,
	.ptr_32          = NULL,
};



#endif /* MAIN_H_ */





