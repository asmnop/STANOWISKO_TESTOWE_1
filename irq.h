/*
 * irq.h
 *
 *  Created on: 22 paź 2024
 *      Author: asmnop
 */

#ifndef IRQ_H_
#define IRQ_H_

#include "universal_lib.h"

#include "../../LIB/Inc/adc.h"
#include "../../LIB/Inc/dma.h"
#include "../../LIB/Inc/DS3231.h"
#include "../../LIB/Inc/encoder.h"
#include "../../LIB/Inc/gpio.h"
#include "../../LIB/Inc/motor.h"
#include "../../LIB/Inc/pid.h"
#include "../../LIB/Inc/pwm.h"
#include "../../LIB/Inc/SM_driver.h"
#include "../../LIB/Inc/terminal.h"
#include "../../LIB/Inc/universal_data.h"
#include "../../LIB/Inc/usart.h"
#include "../../LIB/Inc/VT100.h"
#include "../../LIB/Inc/ws2812.h"


void IRQ_init(void);
void EXTI2_X1_init(void);
void EXTI3_X2_init(void);
void EXTI_button_init(void);


extern ADC_t ADC_2;
extern CHANNEL_t *LED[];
extern DMA_t DMA_ADC_1;
extern DMA_t DMA_USART2_TX;
extern DRV8833_t DRIVER_1;
extern DRV8833_t DRIVER_2;
extern ENCODER_t ENCO_1;
extern ENCODER_t PULSER_1;
extern LINE_t TIM;
extern LINE_t BUTTON_LINE;
extern LINE_t DS3231_INT_SQW;
extern LINE_t LED_GREEN;
extern LINE_t LIMIT_SWITCH_X1_LINE;
extern LINE_t LIMIT_SWITCH_X2_LINE;
extern MOTOR_t MOTOR_1;
extern RTC_t RTC_1;
extern SM_t SM[];
extern TERMINAL_DISPLAY_t TERM_DISP;
extern TERMINAL_t TERMINAL_ADC;
extern TERMINAL_t TERMINAL_BASE;
extern TERMINAL_t TERMINAL_DC;
extern TERMINAL_t TERMINAL_PID;
extern TERMINAL_t TERMINAL_SERVO;
extern TERMINAL_t TERMINAL_SM;
extern TERMINAL_t TERMINAL_SONGS;
extern TIM_t PID_TIMER;
extern TIM_t SM_1_TIM;
extern USART_t USART_1;
extern USART_t USART_2;
extern USART_t USART_3;
extern DMA_t DMA_TIM8_CH1;

extern uint8_t WS2812_ar[];

extern WS2812_LED_t WS2812_LED_LINE[];

extern WS2812_TO_SEND_t WS2812_LED_LINE_DATA[];

#endif /* IRQ_H_ */








