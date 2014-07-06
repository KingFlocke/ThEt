/*
 * system.h
 *
 *  Created on: 15.03.2011
 *      Author: eck
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include "header.h"
#include "mark.h"
#include "molecule.h"
//#include "config.h"
#include <vector>

class eNumberMoleculesNotInteger{};
class eAtLeastTwoMoleculesEqual{};

class System{
private:
	Header header;
	vector<Molecule> vMolecules;
	//Mark mark(vector<Molecule> vMolecules);
	Mark mark;
//	Config config;
	double refEnergy;
	double refLength;
	double refMass;
public:
	//constructor
	System(); //erstellt ein leeres System
	System(double lengthBoxX, double lengthBoxY, double lengthBoxZ); //erstellt ein leeres System der Größe X, Y, Z
	System(char* filename); //liest das System aus einer Datei ein
	System(int numberMolecules, double densityNotReduced, double temperatureNotReduced, string component,
			double refEne, double refLen, double refMas);
	System(int numberMolecules, double densityNotReduced, double temperatureNotReduced, string component,
			double refEne, double refLen, double refMas,
			double lengthBoxYNotReduced, double lengthBoxZNotReduced); //erstellt das System vollständig neu

	Header gHeader() const;
	int sHeader(Header head);
	Mark gMark() const;
	int sMark(Mark mar);
	int fileread(char* filename);
	int filewrite(const char* filename, const string flag);
	int filewriteVTK(string strFilePrefix, const string flag, int nShape);
	//getVector suchen nach vector oop c++
	int initVMolecules(int numberMolecules);
	int sVMolecules(unsigned index, Molecule input);
	int sVMolecules(vector<Molecule> input);
	int sVMolecules(vector<vector<double> > vMolPos, vector<vector<double> > vMolVel, vector<vector<double> > vMolOrient, vector<vector<double> > vMolRotSpeed);
	int sVMolecules(vector<vector<double> > vMolPos, vector<vector<double> > vMolVel, int molOrient, int molRotSpeed);
	vector<Molecule> gVMolecules() const;
	int statusVMolecules() const;
	//überladener Ausgabeoperator
	//	friend ostream& operator<< (ostream& os, vector<Molecule>& rhs);
	int changeComponentType(int numberNewTypes, int possibilityPercentKeep, Component comp, double eta, double xi);
	int rotateSystem(int rotateDirection);
	int trimTo(double start[3], double end[3]);
	int shrink(double shrink);
	int eliminateRelativeMovement();

	static vector<vector<double> > readMoleculesPos(char* filename);
	static vector<vector<double> > readMoleculesVel(char* filename);
	int merge(double referenzPunkt[3], System system2, double referenzPunkt2[3]);
};

#endif /* SYSTEM_H_ */
