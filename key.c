/*
 * key.c
 *
 *  Created on: 23 maj 2024
 *      Author: asmnop
 */


#include "..\..\LIB\Inc\key.h"
#include "software_timers.h"
#include "stm32l4xx_ll_gpio.h"
//#include "encoder.h"


#define KEY_PRESSED(x)


//	key_pressed(&PREV, time, time_rep, foo, foo_rep);
//	-pojedyncze odpalenie 'foo' po czasie 'WAIT_KEY_DEBOUNCE',
//	-następnie po czasie 'time' odpalenie 'foo_rep', następnie cykliczne odpalanie 'foo_rep' co 'time_rep',

//	key_pressed(&PREV, time, time_rep, foo, NULL);
//	-pojedyncze odpalenie 'foo' po czasie 'WAIT_KEY_DEBOUNCE',

//	key_pressed(&PREV, time, time_rep, NULL, foo_rep);
//	-odpalenie 'foo_rep' po czasie 'WAIT_KEY_DEBOUNCE' + 'time', następnie cykliczne odpalanie 'foo_rep' co 'time_rep',

//	ON_KEY_UP:
//	key_pressed(&PREV, 0, ON_KEY_UP, foo, NULL);
//	-odpalenie 'foo' po zwolnieniu przycisku,

//	KEY_TIME_OFF:
//	key_pressed(&PREV, time, KEY_TIME_OFF, foo, foo_rep);
//	-odpalenie 'foo' jeśli zwolnimy przycisk przed czasem 'time',
//	-jeśli przytrzymamy przycisk wciśnięty do przekroczenia czasu 'time' to odpalona zostaje 'foo_rep',


//	key_pressed(&PREV, time, time_rep, NULL, NULL);






#define WAIT_KEY_DEBOUNCE	20		//	Czas odczekiwania na ustanie drgań styków,

uint8_t key_pressed_(LINE_t *pLINE, const uint16_t time, const uint16_t time_rep, void(*foo)(void), void(*foo_rep)(void))
{
	//	-sprawdzenie stanu na linii, do której podłączony jest przycisk monostabilny,
	//	-zastosowanie opóźnień czasowych za pomocą timerów programowych,
	//	-działanie funkcji różni się w zależności od podanych wartości parametrów: time, time_rep, foo, foo_rep,
	//	-argumenty wejściowe:
	//	--> key - numer pinu, którego stan sprawdzamy,
	//	--> time - wartość czasu trwania stanu 'wait_for_key_up',
	//	--> time_rep - wartość czasu trwania stanu 'rep',
	//	--> foo - funkcja numer jeden,
	//	--> foo_rep - funkcja numer dwa,

	//	Możliwe stany pracy przycisku. Istnieje możliwośc rozbudowania funkcji poprzez dodanie nowych stanów.
	//	-idle - przycisk nie wciśnięty,
	//	-debounce - czas, w którym mogą wystąpic drgania styków,
	//	-wait_for_key_up - na początku tego stanu zostaje odpalona funkcja 'foo', następnie przez czas 'time' następuje oczekiwanie na zwonienie przycisku,
	//	-rep - czas, co który zachodzi odpalenie funkcji powtarzanej 'foo_rep' - wielokrotne wywołania,
	//	-debounce_exit - czas, w którym mogą wystąpić drgania styków po zwolnieniu przycisku,

	//	CZASY ODPALANE W FUNKCJI:
	//	WAIT_KEY_DEBOUNCE --> time --> rep_time --> ... --> rep_time --> WAIT_KEY_DEBOUNCE,

	//	STANY W FUNKCJI:
	//	-przycisk niewciśnięty,
	//	-przycisk wciśnięty po raz pierwszy i załadowanie timera wartością 'WAIT_KEY_DEBOUNCE',
	//	-przycisk puszczony w momencie oczekiwania na wyzerowanie timera,
	//	-przycisk wciąż wciścięty po wyzerowaniu timera - ponowne załadowanie timera,
	//	-przycisk puszczony w momencie ponownego oczekiwania na wyzerowanie timera - odpalenie funkcji,
	//	-przycisk wciąż wciśnięty po ponownym wyzerowaniu timera - odpalenie funkcji 2,
	//	-oczekiwanie na zajście odpalenia funkcji 2,
	//	-odpalenie funkcji 2,
	//	-przycisk puszczony w trybie oczekiwania na powtórzenia lub w trybie powtórzeń,
	//	-oczekiwanie na ustanie drgań styków przy puszczaniu przycisku,

	//	UWAGI:
	//	-zmienna 'last_key' jest zerowana w każdym miejscu, w którym został wykryty stan wysoki na pinie, tzn. przycisk
	//	przestał byc wciskany,
	//	-należy mieć na uwadze, że jeśli wykorzystujemy funkcje 'key_pressed' do odpalenia pojedynczej funkcji 'foo', w której również wykonujemy
	//	pewne działania na przyciskach to należy ustawić do stanu domyślnego parametry 'key_state',

	enum KEY_STATE {idle, debounce, wait_for_key_up, rep, debounce_exit, other, next_level};

	//////////
	static uint8_t index = 0;
	//static uint8_t key_state_arr[10];

	/////////

	static enum KEY_STATE key_state;	//	Nazwa stanu na aktywnym przycisku,
	static uint8_t key_activ = 0;		//	Informacja, który przycisk jest aktywny czyli ma potwierdzony status wciśniętego,
	uint8_t key_pressed = 0;			//	Zmienna przechowująca informację czy obecnie badany przycisk jest wciśnięty,

	static uint8_t key_activ_old = 0;

	//	Zmienna 'key_activ' nie daje nam informacji jaki przycisk był wciśnięty ostatnio i został zwolniony tylko jaki przycisk
	//	jest uznany za aktywny obecnie po kolejnym wejściu do funkcji,
	//	Jeśli żaden przycisk nie jest aktywny to 'key_activ = 0'. Jeśli któryś jest obecnie wciśnięty to 'key_activ = key'
	//	więc 'key_activ != 0'
	//	Po pierwszym wejściu do funkcji 'key_active == 0' więc return nie działa,
	//	$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	if( key_activ && (key_activ != pLINE->number) ) return 0;	//	Zabezpieczenie przed wciśnięciem dwóch przycisków.
													//	Wyjście z funkcji jeśli jakiś przycisk jest aktywny AND
													//	nie jest to przycisk, którego stan obecnie badamy w funkcji,

	//	$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	//	Sprawdzenie czy na linii panuje stan niski, jeśli tak to oznacza to, że przycisk jest wciśnięty,
	//	Odczytanie stanu badanego klawisza, jeśli jest wciśnięty to 'key_pressed = 1',
	//key_pressed = !HAL_GPIO_ReadPin(ptr->port, ptr->pin);


	//key_pressed = !LL_GPIO_IsInputPinSet(ptr->port, ptr->pin);	//	OK

	if(pLINE->number < 100)			//	Sprawdzenie czy któryś przycisk jest wciśnięty,
	{
		key_pressed = !LL_GPIO_IsInputPinSet(pLINE->port, pLINE->pin);

		/////////////
		if(key_pressed == 1 && key_state == next_level)
		{
			return next_level;
		}
		if(key_state == next_level)
		{
			key_state = idle;
		}
		////////////
	}
	else if(pLINE->number > 100)	//	Sprawdzenie czy inne źródło wyzwalające jest aktywne,
	{
		foo();
	}



	//	##########################################################################################################################
	//	##########################################################################################################################
	//	##########################################################################################################################
	//	Działania w przypadku wykrycia wciśniętego przycisku ORAZ jeśli 'timer_key' jest równy zero,
	//	Są to tak zwane działania zwiększające stan przycisku,
	//	Akcja w momencie odliczenia konkretnego czasu!!!
	if(key_pressed && (timer_key == 0))
	{
		//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		if(key_state == idle)				//	Jeśli stan przycisku to 'idle',
		{
			key_state = debounce;			//	Stan przycisku zmienia się na 'debounce',
			timer_key = WAIT_KEY_DEBOUNCE;	//	Włącza się timer programowy, który odczekuje czas drgań styków,
			return debounce;
		}
		//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		else if(key_state == debounce)		//	Jeśli stan przycisku to 'debounce',
		{
			key_state = wait_for_key_up;	//	Stan przycisku zmienia się na 'wait_for_key_up',
			timer_key = time;				//	Aktywacja timera do zliczania czasu pierwszego oczekiwania na powtarzanie funkcji,
			key_activ = pLINE->number;		//	W tym momencie zostaje oficjalne uznane, że przycisk jest aktywny,

			if(key_activ != key_activ_old)
			{
				key_activ_old = key_activ;
				//ets_printf("Przycisk %d \n\r", key_activ);
			}
			//ets_printf("Stan przycisku %d \n\r", key_state);
			//ESP_LOGI(TAG, "CZAS:");

			if(foo && ( time_rep != KEY_TIME_OFF) )
			{
				index++;
				//key_state_arr[index] = key_state;
				//key_state = next_level;
				foo();
				//key_state = key_state_arr[index];
				index--;
			}

			//if(foo)	foo();

			return wait_for_key_up;
		}
		//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		else if(key_state == wait_for_key_up)	//	Jeśli stan przycisku to 'key_up',
		{
			key_state = rep;					//	Stan przycisku zmienia się na 'rep',
			timer_key = time_rep;
			//ets_printf("Stan przycisku %d \n\r", key_state);
			//ESP_LOGI(TAG, "CZAS:");

			if(foo_rep && ( time_rep != KEY_TIME_OFF) )
			{
				foo_rep();
			}
			else if(foo_rep && ( time_rep == KEY_TIME_OFF) )
			{
				foo_rep();
			}

			//if(foo_rep)	foo_rep();

			return rep;
		}
		//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		else if(key_state == rep)		//	Jeśli stan przycisku to 'rep',
		{
			key_state = rep;			//	Stan przycisku zmienia się na 'rep',
			timer_key = time_rep;		//	Aktywacja timera do zliczania czasu odstępu pomiędzy powtórzeniami funkcji,

			//ets_printf("Stan przycisku rep %d \n\r", key_state);
			//ESP_LOGI(TAG, "CZAS:");

			if(foo_rep && ( time_rep != KEY_TIME_OFF) )
			{
				foo_rep();
			}

			//if(foo_rep)	foo_rep();
			return rep;
		}
	}

	//	##########################################################################################################################
	//	##########################################################################################################################
	//	##########################################################################################################################
	//	Działania w przypadku wykrycia zwolnionego przycisku ORAZ jeśli 'timer_key' nie zliczył jeszcze do zera,
	//	Są to tak zwane działania zmniejszające stan przycisku,
	else if(key_pressed == 0 && timer_key)
	{
		if(key_state == debounce)
		{
			//	Brak działania,
			//	Wejście tutaj nastąpiło w czasie trwania drgań styków, a więc w czasie kiedy może takie działanie nastąpić,
			return debounce;
		}
		//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		else if(key_state == wait_for_key_up)
		{
			if(foo && ( time_rep == KEY_TIME_OFF) )
			{
				foo();
			}

			key_state = debounce_exit;
			timer_key = WAIT_KEY_DEBOUNCE;	//	Włącza się timer programowy, który odczekuje czas drgań styków na wyjściu z obsługi,
			return debounce_exit;
		}
		//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		else if(key_state == rep)
		{
			key_state = debounce_exit;
			timer_key = WAIT_KEY_DEBOUNCE;	//	Włącza się timer programowy, który odczekuje czas drgań styków na wyjściu z obsługi,
			return debounce_exit;
		}
	}
	else if((key_state == debounce_exit) && (timer_key == 0))
	{
		key_state = idle;
		key_activ = 0;
		//ESP_LOGI(TAG, "key active: %i", key_activ);
		return idle;
	}
	/*
	else if((key_state != idle) && (timer_key == 0) && (key_pressed == 0))
	{
		key_state = idle;
		key_activ = 0;
		return idle;
	}
*/
	return key_state + 10;
}


uint8_t key_pressed(LINE_t *pLINE, const uint16_t time, const uint16_t time_rep, void(*foo)(void), void(*foo_rep)(void))
{
	//	-sprawdzenie stanu na linii, do której podłączony jest przycisk monostabilny,
	//	-zastosowanie opóźnień czasowych za pomocą timerów programowych,
	//	-działanie funkcji różni się w zależności od podanych wartości parametrów: time, time_rep, foo, foo_rep,
	//	-argumenty wejściowe:
	//	--> key - numer pinu, którego stan sprawdzamy,
	//	--> time - wartość czasu trwania stanu 'wait_for_key_up',
	//	--> time_rep - wartość czasu trwania stanu 'rep',
	//	--> foo - funkcja numer jeden,
	//	--> foo_rep - funkcja numer dwa,

	//	Możliwe stany pracy przycisku. Istnieje możliwośc rozbudowania funkcji poprzez dodanie nowych stanów.
	//	-idle - przycisk nie wciśnięty,
	//	-debounce - czas, w którym mogą wystąpic drgania styków,
	//	-wait_for_key_up - na początku tego stanu zostaje odpalona funkcja 'foo', następnie przez czas 'time' następuje oczekiwanie na zwonienie przycisku,
	//	-rep - czas, co który zachodzi odpalenie funkcji powtarzanej 'foo_rep' - wielokrotne wywołania,
	//	-debounce_exit - czas, w którym mogą wystąpić drgania styków po zwolnieniu przycisku,

	//	CZASY ODPALANE W FUNKCJI:
	//	WAIT_KEY_DEBOUNCE --> time --> rep_time --> ... --> rep_time --> WAIT_KEY_DEBOUNCE,

	//	STANY W FUNKCJI:
	//	-przycisk niewciśnięty,
	//	-przycisk wciśnięty po raz pierwszy i załadowanie timera wartością 'WAIT_KEY_DEBOUNCE',
	//	-przycisk puszczony w momencie oczekiwania na wyzerowanie timera,
	//	-przycisk wciąż wciścięty po wyzerowaniu timera - ponowne załadowanie timera,
	//	-przycisk puszczony w momencie ponownego oczekiwania na wyzerowanie timera - odpalenie funkcji,
	//	-przycisk wciąż wciśnięty po ponownym wyzerowaniu timera - odpalenie funkcji 2,
	//	-oczekiwanie na zajście odpalenia funkcji 2,
	//	-odpalenie funkcji 2,
	//	-przycisk puszczony w trybie oczekiwania na powtórzenia lub w trybie powtórzeń,
	//	-oczekiwanie na ustanie drgań styków przy puszczaniu przycisku,

	//	UWAGI:
	//	-zmienna 'last_key' jest zerowana w każdym miejscu, w którym został wykryty stan wysoki na pinie, tzn. przycisk
	//	przestał byc wciskany,
	//	-należy mieć na uwadze, że jeśli wykorzystujemy funkcje 'key_pressed' do odpalenia pojedynczej funkcji 'foo', w której również wykonujemy
	//	pewne działania na przyciskach to należy ustawić do stanu domyślnego parametry 'key_state',

	enum KEY_STATE {idle, debounce, wait_for_key_up, rep, debounce_exit, other, next_level};

	//////////
	//static uint8_t index = 0;
	//static uint8_t key_state_arr[10];

	/////////

	static enum KEY_STATE key_state;	//	Nazwa stanu na aktywnym przycisku,
	static uint8_t key_activ = 0;		//	Informacja, który przycisk jest aktywny czyli ma potwierdzony status wciśniętego,
	uint8_t key_pressed = 0;			//	Zmienna przechowująca informację czy obecnie badany przycisk jest wciśnięty,

	static uint8_t key_activ_old = 0;

	//	Zmienna 'key_activ' nie daje nam informacji jaki przycisk był wciśnięty ostatnio i został zwolniony tylko jaki przycisk
	//	jest uznany za aktywny obecnie po kolejnym wejściu do funkcji,
	//	Jeśli żaden przycisk nie jest aktywny to 'key_activ = 0'. Jeśli któryś jest obecnie wciśnięty to 'key_activ = key'
	//	więc 'key_activ != 0'
	//	Po pierwszym wejściu do funkcji 'key_active == 0' więc return nie działa,
	//	$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	if( key_activ && (key_activ != pLINE->number) ) return 0;	//	Zabezpieczenie przed wciśnięciem dwóch przycisków.
													//	Wyjście z funkcji jeśli jakiś przycisk jest aktywny AND
													//	nie jest to przycisk, którego stan obecnie badamy w funkcji,

	//	$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	//	Sprawdzenie czy na linii panuje stan niski, jeśli tak to oznacza to, że przycisk jest wciśnięty,
	//	Odczytanie stanu badanego klawisza, jeśli jest wciśnięty to 'key_pressed = 1',
	//key_pressed = !HAL_GPIO_ReadPin(ptr->port, ptr->pin);


	//key_pressed = !LL_GPIO_IsInputPinSet(ptr->port, ptr->pin);	//	OK

	if(pLINE->number < 100)			//	Sprawdzenie czy któryś przycisk jest wciśnięty,
	{
		key_pressed = !LL_GPIO_IsInputPinSet(pLINE->port, pLINE->pin);
	}
	else if(pLINE->number > 100)	//	Sprawdzenie czy inne źródło wyzwalające jest aktywne,
	{
		foo();
	}


	//	##########################################################################################################################
	//	##########################################################################################################################
	//	##########################################################################################################################
	//	Działania w przypadku wykrycia wciśniętego przycisku ORAZ jeśli 'timer_key' jest równy zero,
	//	Są to tak zwane działania zwiększające stan przycisku,
	//	Akcja w momencie odliczenia konkretnego czasu!!!
	if((key_pressed == 1) && (timer_key == 0))
	{
		//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		if(key_state == idle)				//	Jeśli stan przycisku to 'idle',
		{
			key_state = debounce;			//	Stan przycisku zmienia się na 'debounce',
			timer_key = WAIT_KEY_DEBOUNCE;	//	Włącza się timer programowy, który odczekuje czas drgań styków,
			return debounce;
		}
		//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		else if(key_state == debounce)		//	Jeśli stan przycisku to 'debounce',
		{
			key_state = wait_for_key_up;	//	Stan przycisku zmienia się na 'wait_for_key_up',
			timer_key = time;				//	Aktywacja timera do zliczania czasu pierwszego oczekiwania na powtarzanie funkcji,
			key_activ = pLINE->number;		//	W tym momencie zostaje oficjalne uznane, że przycisk jest aktywny,

			if(key_activ != key_activ_old)
			{
				key_activ_old = key_activ;
			}

			if(foo && (time_rep != ON_KEY_UP) && (time_rep != KEY_TIME_OFF))
			{
				foo();
			}

			return wait_for_key_up;
		}
		//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		else if(key_state == wait_for_key_up)	//	Jeśli stan przycisku to 'wait_for_key_up',
		{
			key_state = rep;					//	Stan przycisku zmienia się na 'rep',
			timer_key = time_rep;

			if(foo_rep && (time_rep != ON_KEY_UP))
			{
				foo_rep();
			}
			else if(foo && (time_rep == KEY_TIME_OFF))
			{
				foo();
			}

			return rep;
		}
		//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		else if(key_state == rep)		//	Jeśli stan przycisku to 'rep',
		{
			key_state = rep;			//	Stan przycisku zmienia się na 'rep',
			timer_key = time_rep;		//	Aktywacja timera do zliczania czasu odstępu pomiędzy powtórzeniami funkcji,

			if(foo_rep && (time_rep != ON_KEY_UP) && (time_rep != KEY_TIME_OFF))
			{
				foo_rep();
			}

			return rep;
		}
	}


	//	##########################################################################################################################
	//	##########################################################################################################################
	//	##########################################################################################################################
	//	Działania w przypadku wykrycia zwolnionego przycisku ORAZ jeśli 'timer_key' nie zliczył jeszcze do zera,
	//	Są to tak zwane działania zmniejszające stan przycisku,
	else if((key_pressed == 0) && (timer_key != 0))
	{
		//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		if(key_state == wait_for_key_up)
		{
			if(foo && ( time_rep == KEY_TIME_OFF) )	//	Akcja jeśli puścimy przycisk przed czasem 'time',
			{
				foo();
			}

			key_state = debounce_exit;
			timer_key = WAIT_KEY_DEBOUNCE;	//	Włącza się timer programowy, który odczekuje czas drgań styków na wyjściu z obsługi,
			return debounce_exit;
		}
		//	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		else if(key_state == rep)
		{
			if(foo && ( time_rep == ON_KEY_UP) )
			{
				foo();
			}
			key_state = debounce_exit;
			timer_key = WAIT_KEY_DEBOUNCE;	//	Włącza się timer programowy, który odczekuje czas drgań styków na wyjściu z obsługi,
			return debounce_exit;
		}
	}

	//	##########################################################################################################################
	//	##########################################################################################################################
	//	##########################################################################################################################
	//	Działania w przypadku zwolnionego przycisku ORAZ jeśli 'timer_key' jest równy zero,
	else if((key_pressed == 0) && (timer_key == 0))
	{
		if((key_state == debounce) || (key_state == debounce_exit))
		{
			key_state = idle;
			key_activ = 0;
			return idle;
		}
		else if(key_state == wait_for_key_up)
		{
			key_state = debounce_exit;
			return debounce_exit;
		}
		else if(key_state == rep)
		{
			key_state = debounce_exit;
			return debounce_exit;
		}
	}

	return key_state + 10;
}













