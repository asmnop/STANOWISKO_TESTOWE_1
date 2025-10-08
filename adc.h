/*
 * adc.h
 *
 *  Created on: 17 paź 2024
 *      Author: asmnop
 */

#ifndef ADC_H_
#define ADC_H_


#include "universal_lib.h"

#include "..\..\LIB\Inc\gpio.h"
#include "..\..\LIB\Inc\dma.h"


typedef struct
{
	ADC_TypeDef *pADCx;			//	Jednostka ADC,
	uint32_t enable_periph;		//	Zmienna włączająca zegar dla wybranej jednostki ADC,
	const uint16_t V_DDA;		//	Wartość napięcia referencyjnego ADC, domyślnie wartość V_DD,

	uint32_t resolution;		//	Rozdzielczość pomiarowa: 6, 8, 10, 12 bitów,
	uint32_t data_alignment;	//	Wyrównanie danych do lewej lub prawej,
	uint32_t trigger_source;	//	Źródło wyzwalania konwersji, programowe lub zewnętrzne,
	uint32_t mode;				//	Tryb pracy,
	uint32_t overrun;			//	Co zrobić w przypadku nadejścia danych, a stare nie odczytane,

	uint32_t sequencer_length;		//	Ilość konwersji do wykonania w jednym cyklu pomiarowym,
	uint32_t rank[16];				//	Wybór kolejności wykonywania konwersji,
	uint32_t channel[16];			//	Kanał, z którego wykonywana będzie konwersja,
	uint32_t sampling_time[16];		//	Czas próbkowania,
	uint32_t data_received[16];		//	Odebrane dane z poszczególnych kanałów,

	uint8_t enable_IT_EOC;		//	Zezwolenie na obsługę przerwania od EOC,
	uint8_t enable_IT_EOS;		//	Zezwolenie na obsługę przerwania od EOS,
	uint8_t enable_IT_OVR;		//	Zezwolenie na obsługę przerwania od OVR,

	uint32_t number_of_data;	//	Ilość danych do transmisji,
	uint32_t sum;
}ADC_t;


void ADC_init(ADC_t *pADCx);
void ADC_init_G4(ADC_t *pADCx);
void ADC_start(ADC_t *pADCx);
void ADC_read_data(ADC_t *pADCx);
void ADC_IT_init(ADC_t *pADCx);
void ADC_stop_conversion(ADC_t *pADCx);

void Configure_ADC(void);
void Activate_ADC(void);
void ConversionStartPoll_ADC_GrpRegular(void);

uint16_t ADC_LL_1_SINGLE(void);
uint16_t ADC_LL_1_continuous(void);
void ADC_LL_2_SINGLE(uint16_t data[]);
void ADC_LL_2_continuous(uint16_t data[]);


void ADC_LL_TEMP(void);
uint32_t ADC_LL_TEMP_COMPUTE(void);

void ADC_LL_POT(uint32_t Channel);
uint32_t ADC_LL_POT_COMPUTE(void);

uint16_t ADC_LL_SINGLE_G4(void);
uint16_t ADC_compute_voltage(ADC_t *pADC);
uint32_t ADC_joystick_G4(void);
uint32_t ADC_joystick_G4_DMA(void);




#endif /* ADC_H_ */








