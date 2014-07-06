/*
 * randomArray.cpp
 *
 *  Created on: 04.05.2011
 *      Author: eck
 */

#include <cstdlib> //für rand()
#include <ctime> //für time()
#include <iostream> //für cerr und endl
#include <list>

using namespace std;

#include "randomArray.h"

RandomArray::RandomArray(int size, int numberTrue){
	//size muss mindestens so groß sein wie numberTrue
	if (size < numberTrue){
		throw eSizeSmallerNumbertrue();
	}

	//Größe von vBool anpassen und initialisieren
	vBool.resize(size);
	for (int i=0; i<=size-1; i++)
		vBool[i] = false;

	int possibilityPerMillTrue = (int)((double)numberTrue/size*1000); //Reihenfolge wichtig wg. int*1000/int, dabei Pufferüberlauf bei großen int*1000: Umwandlung

	//srand(time(NULL)); //initialisiert den pseudo-random number generator
	for(unsigned int i=0; i<=vBool.size()-1; i++){
		int randomNumber = rand()%1000; //zufällige Zahl [0:999]
		if (randomNumber<possibilityPerMillTrue)
			vBool[i] = true;
	}
	//zählen wieviele Stellen des Arrays true sind und benötigte Liste mit nur true oder false erstellen
	int numberVBoolTrue = 0;
	for (int i = 0; i <= size-1; i++){
		if (vBool[i] == true)
			numberVBoolTrue++;
	}
	list<int> lTrue, lFalse;
	for (int i = 0; i <= size-1; i++){
		if (vBool[i] == true)
			lTrue.push_front(i);
		else
			lFalse.push_front(i);
	}
	//über-/unterzählige anpassen
	int diffTrue = numberVBoolTrue-numberTrue;
	if (diffTrue > 0){
		while (diffTrue > 0){ //überzählig: zu viele true
			int randomNumber = rand()%(size-lTrue.size()); //zufällige Zahl [0:lTrue.size()-1]
			list<int>::iterator iter = lTrue.begin();
			for (int i = 0; i < randomNumber; ++i)
				++iter;
			lTrue.erase(iter);
			diffTrue--;
		}
		list<int>::reverse_iterator riter;
		int iV = 0;
		for(riter=lTrue.rbegin(); riter!=lTrue.rend(); ++riter){
			while(iV < *riter){
				vBool[iV] = false;
				++iV;
			}
			vBool[iV] = true;
			++iV;
		}
	}else if (diffTrue < 0){
		while (diffTrue < 0){ //unterzählig
			int randomNumber = rand()%(size-lFalse.size()); //zufällige Zahl [0:lFalse.size()-1]
			list<int>::iterator iter = lFalse.begin();
			for (int i = 0; i < randomNumber; ++i)
				++iter;
			lFalse.erase(iter);
			diffTrue++;
		}
		list<int>::reverse_iterator riter;
		int iV = 0;
		for(riter=lFalse.rbegin(); riter!=lFalse.rend(); ++riter){
			while(iV < *riter){
				vBool[iV] = true;
				++iV;
			}
			vBool[iV] = false;
			++iV;
		}
	}

#ifdef DEBUG
	//true/false zählen
	int n = 0;
	for (int i = 0; i <= size-1; i++){
		if (vBool[i] == true)
			n++;
	}
	cout << "number of true elements in array: " << n << "\n";
	cout << "number of false elements in array: " << size-n << "\n";

	cout << "Random Type Booleans for the first 10 Indices: ";
	for (int i = 0; i <= 9; i++)
		cout << vBool[i];
	cout << "\n";
#endif
}

bool RandomArray::gArray(unsigned i) const{
	if (i>vBool.size()-1){
		cerr << "Error: asked dimension is not part of the array. Exit.\n" << endl;
			return false;
	}
	return vBool[i];
}

bool RandomArray::operator[](int index){
	return vBool[index];
}
