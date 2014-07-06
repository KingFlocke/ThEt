//============================================================================
// Name        : merge.cpp
// Author      : ich
// Version     :
// Copyright   : blacopyright
// Description : bla
//============================================================================

#include "moledit.h"

#include <iostream>
#include <string>
#include <cmath>
#include <stdlib.h>

#include "err.h"

class eLengthsNotEqual{};
class eNumberMoleculesNotEqual{};
class eFlagNotOneTwo{};
class eSizeSmallerNumbertrue{};

using namespace std;

int main(int argc, char** argv) {
	const char* usage = "usage: moledit <filename> <outfilenamePrefix> <nShape (1:dots, 2:spheres)> \n\n";
	if(argc != 4)
	{
		cout << usage;
		return 1;
	}

	//srand(time(NULL)); //initialisiert den pseudo-random number generator

	//int numberMolecules = atof(argv[1]);
	//double density = atof(argv[2])*6.02214129e-4;//1/A°^3
	//double temperature = atof(argv[3]);//K
	char* infilename = argv[1];
	char* outfilename = argv[2];
	int nShape = atoi(argv[3]);

	////Bezugsgrößen (normalerweise epsilon, sigma, Masse einer Komponente)
	////Standard vom mader-Programm
	//double refEnergy = 1;//K
	//double refLength = 1;//A°
	////double refMass = 14.007; //u
	//double refMass = 1;//u

	////System erstellen
	System sys;
	//try{
		//System temp(numberMolecules, density, 0, "tip4p2005", refEnergy, refLength, refMass);
		//Header head = temp.gHeader();
		//head.sTemperature(temperature/refEnergy);
		//temp.sHeader(head);
		//sys = temp;
	//}
	//catch (const eNumberMoleculesNotInteger &Exception){
		//cerr << "Error (System::System): numberMolecules is not an integer. Aborting.";
		//return 1;
	//}
	//catch (const eSizeSmallerNumbertrue &Exception){
		//cerr << "Error (System::System RandomArray): number of true elements is larger than size of array. Aborting.";
		//return 1;
	//}
	//catch (exception &Exception){
		//cerr << Exception.what();
		//return 1;
	//}

	////Kommentar (Aufruf) setzen
	//Header head = sys.gHeader();
	//head.addExecComment(VER, REV, argc, argv);
	//sys.sHeader(head);

	////Moleküle austauschen
	//Component comp2("azeton", refEnergy, refLength, refMass);
	//changeComponentType(sys, 2, 80, comp2, 1, 1);

	//drehen, damit Dichteprofile in y-Richtung aufgezeichnet werden können
	//rotateSystem(sysGFG, 2);

	//Relativbewegung eliminieren
	//sysGFG.eliminateRelativeMovement();

	//rausschreiben
	sys.fileread(infilename);
	sys.filewriteVTK(outfilename,"overwrite", nShape);

	//Ende
	cout << "finished!" << endl;
	return 0;
}

int changeComponentType(System &system,int numberNewTypes,int possibilityPercentKeep, Component comp, double eta, double xi){
	system.changeComponentType(numberNewTypes,possibilityPercentKeep,comp,eta,xi);
	return 0;
}

int rotateSystem(System &system, int rotateDirection){
	system.rotateSystem(rotateDirection);
	return 0;
}

System mergeSystems(System system1, System system2, int flag){
	//system1 und system2 in x-Richtung verbinden
	//flag = 1: header von system1 wird übernommen; flag = 2: header von system2 wird übernommen
	//Boxlängen ermitteln
	Header header1 = system1.gHeader();
	Header header2 = system2.gHeader();
	double* length1;
	double* length2;
	length1 = header1.gLength();
	length2 = header2.gLength();
	//testen ob die Längen in y- und z-Richtung gleich sind
	if ((length1[1] != length2[1]) || (length1[2] != length2[2])){
		throw eLengthsNotEqual();
	}
	//neues System erstellen
	System system3 = system1;
	//und Parameter ändern
	Header header3 = system3.gHeader();
	if (flag == 1){
		header3 = header1;
	}
	else if (flag == 2){
		header3 = header2;
	}
	else{
		throw eFlagNotOneTwo();
	}
	double length3[3];
	length3[0] = length1[0] + length2[0];
	length3[1] = length1[1];
	length3[2] = length1[2];
	header3.sLength(length3);
	int numberOfMolecules1 = header1.gNumberMolecules();
	int numberOfMolecules2 = header2.gNumberMolecules();
	int numberOfMolecules3 = numberOfMolecules1+numberOfMolecules2;
	header3.sNumberMolecules(numberOfMolecules3);
	system3.sHeader(header3);
	vector<Molecule> vMolecules2 = system2.gVMolecules();
	system3.initVMolecules(numberOfMolecules3);
	for (int n = 0; n <= numberOfMolecules2-1; n++){
		Molecule molecule = vMolecules2[n];
		molecule.sIdNumber(numberOfMolecules1+n+1);
		double* pos = molecule.gPosition();
		pos[0] = pos[0]+length1[0];
		molecule.sPosition(pos);
#ifdef DEBUG
		if (n == 0){
			cout << "merging..." << endl;
			cout << "last old and first new molecule id: " << numberOfMolecules1 << " " << numberOfMolecules1+n+1 << endl;
			cout << "first new molecule position: " << pos[0] << " " << pos[1] << " " << pos[2] << endl;
		}
#endif
		system3.sVMolecules(numberOfMolecules1+n, molecule);
	}

	return system3;
}

System readRawSystem(double sizeBox[3], char* posInFilename, char* velInFilename, double refEnergy, double refLength, double refMass, double temperature){
	System sys1(sizeBox[0],sizeBox[1],sizeBox[2]);
	vector<vector<double> > vMolPos = System::readMoleculesPos(posInFilename);
#ifdef DEBUG
	cout << "DEBUG" << endl;
	unsigned int size = vMolPos.size();
	cout << "the size of vMolPos is " << size << "." << endl;
	cout << "the first element of vMolPos is " << vMolPos[0][0] << " " << vMolPos[0][1] << " " <<
			vMolPos[0][2] << "." << endl;
	cout << "the second element of vMolPos is " << vMolPos[1][0] << " " << vMolPos[1][1] << " " <<
			vMolPos[1][2] << "." << endl;
	cout << "the next-to-last element of vMolPos is " << vMolPos[size-2][0] << " " << vMolPos[size-2][1] << " " <<
					vMolPos[size-2][2] << "." << endl;
	cout << "the last element of vMolPos is " << vMolPos[size-1][0] << " " << vMolPos[size-1][1] << " " <<
				vMolPos[size-1][2] << "." << endl;
	cout << "end DEBUG" << endl;
#endif
	vector<vector<double> > vMolVel = System::readMoleculesVel(velInFilename);
	unsigned int numberMolecules = vMolPos.size();
	if (numberMolecules != vMolVel.size()){
		throw eNumberMoleculesNotEqual();
	}

	sys1.sVMolecules(vMolPos, vMolVel, 0, 0);
	Header head = sys1.gHeader();
	Component comp = Component("ar",refEnergy,refLength,refMass);
	head.sComponentDef(comp);
	double temp = temperature/refEnergy;
	head.sTemperature(temp);
	head.sNumberMolecules(numberMolecules);
	sys1.sHeader(head);
	return sys1;
}
