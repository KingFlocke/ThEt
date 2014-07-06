/*
 * mark.h
 *
 *  Created on: 23.08.2011
 *      Author: eck
 */

#ifndef MARK_H_
#define MARK_H_

#include <vector>
#include "molecule.h"

class Mark{
private:
	vector<bool> marklist;
	unsigned int numberMarked();
public:
	Mark(vector<Molecule> vMolecules);
	Mark();
	int oblong(double start[3], double end[3], vector<Molecule> vMolecules);
	int ball(double center[3], double radius, vector<Molecule> vMolecules);
	int others();
	int del(vector<Molecule> &vMolecules);
};

#endif /* MARK_H_ */
