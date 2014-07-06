/*
 * mark.cpp
 *
 *  Created on: 23.08.2011
 *      Author: eck
 */

#include <cmath>

using namespace std;

#include "mark.h"

Mark::Mark(vector<Molecule> vMolecules){
	//marklist erstellen, alle Werte auf false
	unsigned siz = vMolecules.size();
	marklist.resize(siz);
	for (unsigned i = 0; i <= siz-1; i++)
		marklist[i] = false;
}

Mark::Mark(){
}

int Mark::oblong(double start[3], double end[3], vector<Molecule> vMolecules){
	unsigned siz = vMolecules.size();
	//marklist auf die richtige Größe setzen wichtig!!
	marklist.resize(siz);
	for (unsigned i = 0; i <= siz-1; i++)
		marklist[i] = false;
	double* pos;
	for (unsigned i = 0; i <= siz-1; i++){
		pos = vMolecules[i].gPosition();
		//wenn die Molekülposition innerhalb der Box ist
		if (pos[0] >= start[0] && pos[0] <= end[0] && pos[1] >= start[1] && pos[1] <= end[1] && pos[2] >= start[2] && pos[2] <= end[2])
			marklist[i] = true;
	}
	return 0;
}

int Mark::ball(double center[3], double radius, vector<Molecule> vMolecules){
	unsigned siz = vMolecules.size();
	//marklist auf die richtige Größe setzen wichtig!!
	marklist.resize(siz);
	for (unsigned i = 0; i <= siz-1; i++)
		marklist[i] = false;
	//Abstand zweier Punkte (Position von Molekül und center)
	for (unsigned i = 0; i <= siz-1; i++){
		double* pos = vMolecules[i].gPosition();
		double d = sqrt(pow(pos[0]-center[0],2.0)+pow(pos[1]-center[1],2.0)+pow(pos[2]-center[2],2.0));
		if (d < radius){
			marklist[i] = true;
		}
	}
	return 0;
}

int Mark::others(){
	unsigned siz = marklist.size();
	for (unsigned i = 0; i <= siz-1; i++){
		if (marklist[i] == false)
			marklist[i] = true;
		else
			marklist[i] = false;
	}
	return 0;
}

int Mark::del(vector<Molecule> &vMolecules){
	unsigned siz = vMolecules.size();
	vector<Molecule> vNewMolecules;
	unsigned numMarked = numberMarked();
	vNewMolecules.resize(numMarked);
	Molecule newMolecule;
	int newID = 1;
	for (unsigned i = 0; i <= siz-1; i++){
		if (marklist[i] == false){
			newMolecule = vMolecules[i];
			newMolecule.sIdNumber(newID);
			vNewMolecules[newID-1] = newMolecule;
			newID++;
		}
	}
	vMolecules = vNewMolecules;
	return 0;
}

unsigned int Mark::numberMarked(){
	unsigned numberMarked = 0;
	unsigned siz = marklist.size();
	for (unsigned i = 0; i <= siz-1; i++){
		if (marklist[i] == true){
			numberMarked++;
		}
	}
	return numberMarked;
}
