/*
 * universal_functions.c
 *
 *  Created on: 25 sie 2024
 *      Author: asmnop
 */


#include "..\..\LIB\Inc\universal_functions.h"

#include "software_timers.h"
#include "../../LIB/Inc/sys_tick.h"

void delay_ms(const uint16_t delay_ms)
{
	timer_1 = delay_ms;
	while(timer_1);
}

uint8_t dec_to_bcd(const uint8_t num)
{
   return ((num / 10) << 4) | (num % 10);
}

uint8_t bcd_to_int(const uint8_t bcd)
{
	//	-zamiana liczby w formacie BCD na liczbę rzeczywistą,

	return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

uint8_t inc(uint8_t variable, const uint8_t max_value)
{
	if(variable == max_value)
	{
		variable = 0;
	}
	else
	{
		variable++;
	}

	return variable;
}

uint8_t dec(uint8_t variable, const uint8_t max_value)
{
	if(variable == 0)
	{
		variable = max_value;
	}
	else
	{
		variable--;
	}

	return variable;
}

uint8_t arr8_length(uint8_t arr[])
{
	//return sizeof(arr)/sizeof(arr[0]);
	return 0;
}

uint16_t measure_time(void)
{
	//	-funkcja do pomiaru czasu pomiędzy dwoma wywołaniami funkcji,
	//	-bazuje na SYS_TICK, a więc może zarejestrować minimalny czas wyrażony
	//	w milisekundach,

	uint64_t time_begin = SYS_TICK.counter;
	static uint64_t time_end = 0;
	uint64_t time_measured = 0;

	time_measured = time_begin - time_end;
	time_end = time_begin;

	return time_measured;
}

//Odwrócenie wartości o podanej dlugosc
void reverse(char *str, int len)
{
    //Deklaracja zmiennych
    int i=0, j=len-1, temp;
    while (i<j)
    {
        //Przypisanie wartosci do zmiennej
        temp = str[i];
        //Przepisanie wartosci
        str[i] = str[j];
        //Wpisanie przechowywanej wartosci do drugiej tablicy
        str[j] = temp;
        i++; j--;
    }
}

/*
    * konwertsja x na wartosc string.
    * zmienna d okresla ilosc cyfr jakie beda na wyjsciu.
*/
int intToStr(int x, char str[], int d)
{
    //Deklaracja zmiennej i
    int i = 0;
    //Petla dzia³a dopóki x jest mniejsze od 1
    while (x)
    {
        //Przypisanie do tablicy z kolejnym numerem wartoœci
        //x podzielonej bez reszty przez 10 oraz cyfry 0
        str[i++] = (x%10) + '0';
        //Podzielenie wartoœci x normalnie przez 10
        x = x/10;
    }

    // Jesli jest wymagana wieksza ilosc cyfr
    // zostana dodane zera
    while (i < d)
        str[i++] = '0';

    //Odwrocenie wartosci w funkcji reverse
    reverse(str, i);
    //Dopisanie znaku na koncu tablicy
    str[i] = '\0';
    return i;
}

/*
    * Konwersja formatu zmiennoprzecinkowego na znakowy
    * Jako argumenty nalezy podac odpowiednio
    * liczby w formacie float,
    * nazwe zdefiniowanej tablicy w formacie char,
    * ilosc miejsc po przecinku (format int)
*/
char* ftoa(float n, char *res, int afterpoint)
{
	//char tmp_arr[10];

    //Wyciagniecie calkowitej czesci liczby
    int ipart = (int)n;

    //wyciagniêcie czesci ulamkowej
    float fpart = n - (float)ipart;

    //konwerska liczby int na string
    int i = intToStr(ipart, res, 0);

    //	Jeżeli liczba całkowita równa zero to wypisujemy tą wartość,
    if(i == 0)
    {
    	res[0] = '0';
    	i++;
    }

    //Sprawdzenie czy ilosc wyswietlanych liczb po przecinku
    //jest wieksza od 0
    if (afterpoint != 0)
    {
        //Dodadanie kropki po liczbie calkowitej
        res[i] = '.';

        /*
            * Wartosc liczb po przecinku zostaje pomnozone
            * przez funkcję pow
            * Funkcja pow podnosi wartość do potegi,
            * Jako argumenty podstawa, oraz wartość potegi
        */
        fpart = fpart * pow(10, afterpoint);

        //Zamiana wartości znajdującej się po przecinku na wartosc
        //znakowa z wartosci int
        intToStr((int)fpart, res + i + 1, afterpoint);
    }

    return res;
}

char* itoa_space(int number, char *ptr, int system, uint8_t space)
{
	itoa(number, ptr, system);
	uint8_t flag = 0;

	for(uint8_t i=0; i<space; i++)
	{
		if(ptr[i] == '\0')
		{
			flag = 1;
		}

		//if(tmp[i] < '0' || tmp[i] > '9')
		if(flag == 1)
		{
			ptr[i] = ' ';
		}
	}

	ptr[space] = '\0';

	return ptr;
}













