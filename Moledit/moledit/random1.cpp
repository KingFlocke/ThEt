/*
 * random.cpp
 *
 *  Created on: 09.05.2011
 *      Author: eck
 */

using namespace std;

#include <cstdlib> //für rand()

double random1(){ //erstellt eine zufällige Zahl von 0 bis kleiner als 1
	int precision = 1000000;
	int randomNumber = rand()%precision; //zufällige Zahl [0:999999]
	double random = (double)randomNumber/precision;
	return random;
}
