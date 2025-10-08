/*
 * register.h
 *
 *  Created on: 26 paź 2024
 *      Author: asmnop
 */

#ifndef REGISTER_H_
#define REGISTER_H_


const char END[1] = {255};

//	############################################################################
const char GPIO_0[] = "MODER";
const char GPIO_1[] = "OTYPER";
const char GPIO_2[] = "OSPEEDR";
const char GPIO_3[] = "PUPDR";
const char GPIO_4[] = "IDR";
const char GPIO_5[] = "ODR";
const char GPIO_6[] = "BSRR";
const char GPIO_7[] = "LCKR";
const char GPIO_8[] = "AFR[0]";
const char GPIO_9[] = "AFR[1]";
const char GPIO_10[] = "BRR";
const char GPIO_11[] = "ASCR";

const char* const pGPIO[] =
{
	GPIO_0,
	GPIO_1,
	GPIO_2,
	GPIO_3,
	GPIO_4,
	GPIO_5,
	GPIO_6,
	GPIO_7,
	GPIO_8,
	GPIO_9,
	GPIO_10,
	GPIO_11,
	END,
};


//	############################################################################
const char TIM_0[] = "CR1";
const char TIM_1[] = "CR2";
const char TIM_2[] = "SMCR";
const char TIM_3[] = "DIER";
const char TIM_4[] = "SR";
const char TIM_5[] = "EGR";
const char TIM_6[] = "CCMR1";
const char TIM_7[] = "CCMR2";
const char TIM_8[] = "CCER";
const char TIM_9[] = "CNT";
const char TIM_10[] = "PSC";
const char TIM_11[] = "ARR";
const char TIM_12[] = "RCR";
const char TIM_13[] = "CCR1";
const char TIM_14[] = "CCR2";
const char TIM_15[] = "CCR3";
const char TIM_16[] = "CCR4";
const char TIM_17[] = "BDTR";
const char TIM_18[] = "DCR";
const char TIM_19[] = "DMAR";
const char TIM_20[] = "OR1";
const char TIM_21[] = "CCMR3";
const char TIM_22[] = "CCR5";
const char TIM_23[] = "CCR6";
const char TIM_24[] = "OR2";
const char TIM_25[] = "OR3";

const char* const pTIM[] =
{
	TIM_0,
	TIM_1,
	TIM_2,
	TIM_3,
	TIM_4,
	TIM_5,
	TIM_6,
	TIM_7,
	TIM_8,
	TIM_9,
	TIM_10,
	TIM_11,
	TIM_12,
	TIM_13,
	TIM_14,
	TIM_15,
	TIM_16,
	TIM_17,
	TIM_18,
	TIM_19,
	TIM_20,
	TIM_21,
	TIM_22,
	TIM_23,
	TIM_24,
	TIM_25,
	END,
};






#endif /* REGISTER_H_ */








