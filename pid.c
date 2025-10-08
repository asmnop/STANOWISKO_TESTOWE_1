/*
 * pid.c
 *
 *  Created on: 6 lis 2024
 *      Author: asmnop
 */



#include "..\..\LIB\Inc\pid.h"




void PID_init(PID_t *PID, const float k_p, const float k_i, const float k_d, const uint16_t anti_windup)
{
	//	-inicjalizacja zawartości struktury z parametrami regulatora PID,
	//	-w obliczeniach występuje także zmienna 'error', ale jej wartość jest potrzebna tylko w momencie
	//	obliczeń, jej wartość jest zapisywana do 'zmiennej error_prev',

	PID->k_p = k_p;					//	Wzmocnienie członu proporcjonalnego,
	PID->k_i = k_i;					//	Wzmocnienie członu całkowego,
	PID->k_d = k_d;					//	Wzmocnienie członu różniczkowego,
	PID->error_sum = 0;				//	Suma wartości sygnałów błędu regulacji, suma uchybów,
	PID->error_prev = 0;			//	Wartość błędu regulacji z poprzedniego wyliczenia,
	PID->anti_windup = anti_windup;	//	Wartość filtru przeciwnasyceniowego,

	//ets_printf("%d.%02d		%d.%02d		%d.%02d", (int)(PID->k_p), (int)(PID->k_p*100), (int)(PID->k_i), (int)(PID->k_i*100), (int)(PID->k_d), (int)(PID->k_d*100));
	//next_line();
}

void PID_reset(PID_t *PID)
{
	//	-reset regulatora PID, wyzerowanie odpowiednich zmiennych,

	PID->error_prev = 0;
	PID->error_sum = 0;
}

int16_t PID_compute(PID_t *PID, const int16_t set_point, const int16_t process_variable)
{
	//	-regulacja PID,
	//	-na wyjściu otrzymujemy wartość wyjściową o nie regulowanej wartości,
	//	-wartość zmiennej sterującej zostaje ograniczona w późniejszym kroku,
	//	-funkcja jest odpalana co pewien stały czas, czas próbkowania,
	//	-w tym czasie zbierane są dane procesu,

	//	-u(t) - zmienna procesu, process variable,
	//	-w niektórych zastosowaniach nie można dopuścić do przeregulowania zmiennej procesu ponad wartość zadaną,

	//	-wartość zadana, set point - wartość do jakiej dąży proces,
	//	-zmienna procesu, process variable - obecna wartość procesu,
	//	-wartość zadania i zmienna procesu muszą być tego samego typu np: ilość zmian stanu na liniach enkodera,
	//	RPS, RPM, temperatura, ciśnienie,
	//	-wartość zadana to dla stabilizacji prędkości obrotu kół ilość obrotów na minutę - RPM,
	//	-można pod wartość zadaną wybrać np prędkość pojazdu w [km/h], ALE potem należy doprowadzić zmienną procesu do
	//	jednostki jaka opisuje wartość zadana,
	//	-dla przykładu w sterowaniem prędkością obrotów kół zmienna procesu to wartość sygnału PWM podawanego jako jego wypełnienie,
	//	-zmienna procesu może cechować się wartościami granicznymi np. dla sygnału PWM to [0, 1024]
	//	-algorytm PID nie interesuje skąd pochodzi 'set_point', czy jest to wartość zmian stanu na liniach enkodera czy
	//	może jest to prędkość obrotowa wału w obr/min lub obr/s czy może jest to już prędkość obrotowa na wyjściu z
	//	przekładni,

	//	P:
	//	-człon proporcjonalny,
	//	-jego znak zależy od znaku wartości błędu uchybu,
	//	-jeśli zmienna procesu jest mniejsza niż wartość zadana to błąd uchybu przyjmuje wartość dodatnią,
	//	-jeśli zmienna procesu jest większa niż wartość zadana to błąd chybu przyjmuje wartość ujemną,

	//	I:
	//	-człon całkujący,
	//	-jest interpretowany jako pole pod wykresem funkcji zmiany błędu uchybu w czasie,
	//	-w przypadku zwiększania prędkości obrotowej silnika DC wartość członu całkującego jest dodatnia jeśli suma wartości błędów uchybu
	//	w czasie od [0 do t] była dodatnia,
	//

	//	D:
	//	-człon różniczkujący,
	//	-jest interpretowany jako przewidywanie co wydarzy się w przyszłości,
	//	-jest obliczany jako różnica obecnego błędu uchybu oraz poprzedniego błędu uchybu,
	//	-jeżeli różnica jest większa od 0 to oznacza to, że błąd się zmniejsza,
	//	-jeżeli różnica jest mniejsza od 0 to oznacza to, że błąd się zwiększa,

	int16_t error = 0;
	int16_t P_I_D = 0;

	float P = 0.0;
	float I = 0.0;
	float D = 0.0;

	//	CZĘŚĆ OBLICZENIOWA:
	error = set_point - process_variable;
	PID->error_sum = PID->error_sum + error;

	P = (float)(PID->k_p * error);
	I = (float)(PID->k_i * PID->error_sum);
	D = (float)(PID->k_d * (error - PID->error_prev));

	if(I > PID->anti_windup)		I = PID->anti_windup;
	else if(I < -PID->anti_windup)	I = -PID->anti_windup;

	P_I_D = (int16_t)(P + I + D);

	PID->error_prev = error;

	//ets_printf(" %10d %10d %10d %10d %10d %10d %10d ", error, set_point, process_variable, (int)(P), (int)(I), (int)(D), P_I_D);

	return P_I_D;
}

void PID_set_flag(PID_t *PID)
{
	//	-ustawienie flagi zajścia przerwania od obliczenia nowych wartości sterujących,

	 PID->flag = 1;
}

void PID_reset_flag(PID_t *PID)
{
	//	-zdjęcie flagi zajścia przerwania od obliczenia nowych wartości sterujących,

	 PID->flag = 0;
}

uint8_t PID_get_flag(PID_t *PID)
{
	//	-sprawdzenie stanu flagi zajścia przerwania od obliczenia nowych wartości sterujących,

	 return PID->flag;
}





