/*
 * gpio.c
 *
 *  Created on: 21 sie 2024
 *      Author: asmnop
 */


#include "../../LIB/Inc/gpio.h"

//#warning "6666666666666666666666"

//	////////////////////////////////////////////////////////////////////////////
void GPIO_set_pin(LINE_t *pLine)
{
	//	-ustawienie stanu wysokiego na pinie,

	LL_GPIO_SetOutputPin(pLine->port, pLine->pin);
}

void GPIO_reset_pin(LINE_t *pLine)
{
	//	-ustawienie stanu niskiego na pinie,

	LL_GPIO_ResetOutputPin(pLine->port, pLine->pin);
}

void GPIO_toggle_pin(LINE_t *pLine)
{
	//	-zmiana stanu na pinie,

	LL_GPIO_TogglePin(pLine->port, pLine->pin);
}

void GPIO_set_pin_as_input(LINE_t *pLine)
{
	//	-ustawienie pinu jako linia wejściowa,
	//	-poszczególne parametry są definiowane w strukturze 'LINE_t',
	//	-funkcja jednocześnie wykrywa czy linia ma sprawować funkcję alternatywną,

#if defined(STM32F3)
	if(pLine->port == GPIOA)		LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	else if(pLine->port == GPIOB)	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	else if(pLine->port == GPIOC)	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
	else if(pLine->port == GPIOF)	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOF);
#endif

#if defined(STM32G4) || defined(STM32L4)
	if(pLine->port == GPIOA)		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
	else if(pLine->port == GPIOB)	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
	else if(pLine->port == GPIOC)	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
#endif

	LL_GPIO_SetPinPull(pLine->port, pLine->pin, pLine->pull);
	LL_GPIO_SetPinSpeed(pLine->port, pLine->pin, pLine->speed);
	LL_GPIO_SetPinMode(pLine->port, pLine->pin, pLine->mode);

	if(pLine->mode == LL_GPIO_MODE_ALTERNATE)
	{
		if(pLine->pin <= LL_GPIO_PIN_7)	LL_GPIO_SetAFPin_0_7(pLine->port, pLine->pin, pLine->AF);
		if(pLine->pin >= LL_GPIO_PIN_8)	LL_GPIO_SetAFPin_8_15(pLine->port, pLine->pin, pLine->AF);
		LL_GPIO_SetPinMode(pLine->port, pLine->pin, pLine->mode);
	}
	else
	{
		LL_GPIO_SetPinMode(pLine->port, pLine->pin, pLine->mode);
	}
}

void GPIO_set_pin_as_output(LINE_t *pLine)
{
	//	-ustawienie pinu jako linia wyjściowa,
	//	-poszczególne parametry są definiowane w strukturze 'LINE_t',
	//	-funkcja jednocześnie wykrywa czy linia ma sprawować funkcję alternatywną,

#if defined(STM32F3)
	if(pLine->port == GPIOA)		LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	else if(pLine->port == GPIOB)	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	else if(pLine->port == GPIOC)	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
	else if(pLine->port == GPIOF)	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOF);
#endif

#if defined(STM32G4) || defined(STM32L4)
	if(pLine->port == GPIOA)		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
	else if(pLine->port == GPIOB)	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
	else if(pLine->port == GPIOC)	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
#endif

	LL_GPIO_SetPinOutputType(pLine->port, pLine->pin, pLine->output_type);
	LL_GPIO_SetPinPull(pLine->port, pLine->pin, pLine->pull);
	LL_GPIO_SetPinSpeed(pLine->port, pLine->pin, pLine->speed);
	LL_GPIO_SetPinMode(pLine->port, pLine->pin, pLine->mode);

	if(pLine->mode == LL_GPIO_MODE_ALTERNATE)
	{
		if(pLine->pin <= LL_GPIO_PIN_7)	LL_GPIO_SetAFPin_0_7(pLine->port, pLine->pin, pLine->AF);
		if(pLine->pin >= LL_GPIO_PIN_8)	LL_GPIO_SetAFPin_8_15(pLine->port, pLine->pin, pLine->AF);
		LL_GPIO_SetPinMode(pLine->port, pLine->pin, pLine->mode);
	}
	else
	{
		if(pLine->level == 0)
		{
			LL_GPIO_ResetOutputPin(pLine->port, pLine->pin);
		}
		else if(pLine->level == 1)
		{
			LL_GPIO_SetOutputPin(pLine->port, pLine->pin);
		}
	}
}

void GPIO_set_pin_as_analog(LINE_t *pLine)
{
	//	-ustawienie pinu jako linia analogowa podpięta do ADC,
	//	-poszczególne parametry są definiowane w strukturze 'LINE_t',

#if defined(STM32F3)
	if(pLine->port == GPIOA)		LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	else if(pLine->port == GPIOB)	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	else if(pLine->port == GPIOC)	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC);
	else if(pLine->port == GPIOF)	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOF);
#endif

#if defined(STM32G4) || defined(STM32L4)
	if(pLine->port == GPIOA)		LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
	else if(pLine->port == GPIOB)	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
	else if(pLine->port == GPIOC)	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
#endif

	LL_GPIO_SetPinMode(pLine->port, pLine->pin, pLine->mode);
#if defined(GPIO_ASCR_ASC0)
	LL_GPIO_EnablePinAnalogControl(pLine->port, pLine->pin);
#endif /* GPIO_ASCR_ASC0 */
}

uint32_t GPIO_get_input_pin(LINE_t *pLine)
{
	//	-odczyt stanu na pinie wejściowym,

	return LL_GPIO_IsInputPinSet(pLine->port, pLine->pin);
}

uint8_t GPIO_get_output_pin(LINE_t *pLine)
{
	//	-odczyt ustawionego stanu na pinie wyjściowym,

	return LL_GPIO_IsOutputPinSet(pLine->port, pLine->pin);
}


