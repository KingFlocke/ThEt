/*
 * system.cpp
 *
 *  Created on: 15.03.2011
 *      Author: eck
 */

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

#include <cstdlib> //für rand()
#include <ctime> //für time()
#include <cmath> //für pow() und ceil() und abs(float)
#include "err.h"

using namespace std;

#include "system.h"
#include "randomArray.h"
#include "component.h"
#include "quaternion.h"

#include "random1.cpp"

System::System(){
	System(0, 0, 0);
}

System::System(double lengthBoxX, double lengthBoxY, double lengthBoxZ){
	double lengthBox[3] = {lengthBoxX, lengthBoxY, lengthBoxZ}; //so geht das nur bei Iniitalisierung
	//refEnergy;
	//refLength;
	//refMass;
	header.sTime(0);
	header.sMoleculeFormat("ICRVQD");
	header.sLength(lengthBox);
	header.sComponentDef();
	header.sTemperature(0);
	header.sNumberMolecules(0);
	header.sComment("created with moledit");
}

System::System(char* filename){
	fileread(filename);
}

System::System(int numberMolecules, double densityNotReduced, double temperatureNotReduced, string component, double refEne, double refLen, double refMas){
	System temp(numberMolecules, densityNotReduced, temperatureNotReduced, component, refEne, refLen, refMas, 0, 0);
	*this = temp;
}

System::System(int numberMolecules, double densityNotReduced, double temperatureNotReduced, string component, double refEne, double refLen, double refMas, double lengthBoxYNotReduced, double lengthBoxZNotReduced){
	//numberMolecules [1], densityNotReduced [1/(Einheit von refLength, normalerweise A°)^3], temperatureNotReduced [Einheit von refEnergy, normalerweise K], lengthBoxYNotReduced [Einheit von refLength], lengthBoxZNotReduced [Einheit von refLength], component, refEne, refLen, refMas
	initVMolecules(numberMolecules);

	//Referenzwerte
	refEnergy = refEne; //in K
	refLength = refLen; //in A°
	refMass = refMas; //in u; Hinweis: 1kg/mol=1000u

	double refTime = refLength*sqrt(refMass/refEnergy);
	double refPressure = refEnergy/pow(refLength,3);
	double refDensity = 1/pow(refLength,3);

	//Einheiten berechnen für Kommentar in xdr-Datei
	double kBoltzmann = 1.3806504e-23; //J/K
	double kAvogadro = 6.02214179e23; //1/mol
	string units;
	units = "units:\n";

	ostringstream tmp;
	tmp << refEnergy;
	units += "#[T] = " + tmp.str() + " K\n";
	tmp.str(""); // leeren
	tmp << refLength;
	units += "#[l] = " + tmp.str() + " A°\n";
	tmp.str(""); // leeren
	tmp << (refMass);
	units += "#[m] = " + tmp.str() + " u\n";
	tmp.str(""); // leeren
	tmp << (refEnergy*kBoltzmann*kAvogadro);
	units += "#[u] = " + tmp.str() + " J/mol\n";
	tmp.str(""); // leeren
	tmp << (refPressure*kBoltzmann*1e30/1000000);//bezogen auf refLength^3
	units += "#[p] = " + tmp.str() + " MPa\n";
	tmp.str(""); // leeren
	tmp << (refDensity*1e30/kAvogadro);
	units += "#[rho] = " + tmp.str() + " mol/m^3\n";
	tmp.str(""); // leeren
	tmp << (refTime/(1e10*sqrt(kAvogadro*kBoltzmann*1000))*1e15);
	units += "#[t] = " + tmp.str() + " fs";
	tmp.str(""); // leeren

	//dt
	string dt;
	tmp << (2/(refTime/(1e10*sqrt(kAvogadro*kBoltzmann*1000))*1e15));
	dt = "dt = " + tmp.str();
	tmp.str(""); // leeren

	/*
	 *    double REFTIME = SIG_REF * sqrt(REFMASS / EPS_REF);
   double VEL_REF = SIG_REF / REFTIME;
   cout << "Velocity unit 1 = " << VEL_REF << " * 1620.34 m/s = "
        << 1620.34 * VEL_REF << " m/s.\n";
   double REFCARG = sqrt(EPS_REF * SIG_REF);
   cout << "Charge unit 1 = " << REFCARG << " e.\n";
   double DIP_REF = SIG_REF*REFCARG;
   double QDR_REF = SIG_REF*DIP_REF;
   double REFOMGA = 1.0 / REFTIME;
	 */

	//erstmal alles reduzieren, da man ja mit reduzierten Größen rechnen will
	double density = densityNotReduced*pow(refLength,3);
	double temperature = temperatureNotReduced/refEnergy;
	double lengthBoxY = lengthBoxYNotReduced/refLength;
	double lengthBoxZ = lengthBoxZNotReduced/refLength;

	double volume = (double)numberMolecules/density;
	//double lengthBoxY = (double)lengthBoxY;
	//double lengthBoxZ = (double)lengthBoxZ;
	double lengthBox[3];
	if (lengthBoxY != 0 && lengthBoxZ != 0){
		lengthBox[1] = lengthBoxY;
		lengthBox[2] = lengthBoxZ;
		lengthBox[0] = volume/lengthBoxY/lengthBoxZ;
	}
	else if (lengthBoxY == 0 && lengthBoxZ != 0){
		lengthBox[2] = lengthBoxZ;
		lengthBox[0] = sqrt(volume/lengthBoxZ);
		lengthBox[1] = lengthBox[0];
	}
	else if (lengthBoxY != 0 && lengthBoxZ == 0){
		lengthBox[1] = lengthBoxY;
		lengthBox[0] = sqrt(volume/lengthBoxY);
		lengthBox[2] = lengthBox[0];
	}
	else if (lengthBoxY == 0 && lengthBoxZ == 0){
		lengthBox[0] = pow(volume,(double)1/3);
		lengthBox[1] = lengthBox[0];
		lengthBox[2] = lengthBox[0];
	}

	Component comp(component, refEnergy, refLength, refMass);

	//Position kubisch-flächen-zentriertes Gitter
	double lengthSideDef = pow(volume,(double)1/3); //wenn das Volumen der Simulationsbox kubisch wäre
	double numberCellDef = pow((double)numberMolecules/4,(double)1/3); //Anzahl Zellen pro Seite des Würfels

	int numberCellsX = (int)ceil(numberCellDef/lengthSideDef*lengthBox[0]);
	int numberCellsY = (int)ceil(numberCellDef/lengthSideDef*lengthBox[1]);
	int numberCellsZ = (int)ceil(numberCellDef/lengthSideDef*lengthBox[2]);

	int numberCells = numberCellsX*numberCellsY*numberCellsZ;

	//Sicherheitsabstand lassen am Rand der Box lassen,auch wichtig hinterher fürs Drehen
	double lengthCellX = (lengthBox[0]-0.2)/numberCellsX;
	double lengthCellY = (lengthBox[1]-0.2)/numberCellsY;
	double lengthCellZ = (lengthBox[2]-0.2)/numberCellsZ;

	RandomArray randArray(numberCells*4,numberMolecules);

#ifdef DEBUG
	cout << "randArray = ";
	for (int i = 0; i <= 9; i++)
		cout << randArray[i];
	cout << endl;
#endif

	//Ausrichtung (eine feste; variable s.u.)
	//Quaternion orientation();

	//Winkelgeschwindigkeit vorbereiten
	double omega[3];
	//double I[3];
	//I[0] = comp.gIxx();
	//I[1] = comp.gIyy();
	//I[2] = comp.gIzz();

	double position[3];
	double posCell[3];
	int molId = 1;

	int p = 0;//id der möglichen Molekülposition
#ifdef DEBUG
	cout << "first (10 or less) random phi and chi: ";
#endif
	for (int c = 0; c <= numberCells-1; c++){
		//Position der Zelle
		int cz = (c%(numberCellsY*numberCellsZ))%numberCellsZ; //läuft erst über z; Wert von 0 bis numberCellsZ-1
		int cy = floor((c%(numberCellsY*numberCellsZ))/numberCellsZ); //dann über y
		int cx = floor(c/(numberCellsY*numberCellsZ)); //und dann über x
		posCell[0] = cx*lengthCellX+0.1; //verschieben um den Sicherheitsabstand 0.1
		posCell[1] = cy*lengthCellY+0.1;
		posCell[2] = cz*lengthCellZ+0.1;
		for (int i = 0; i < 4; i++){//4 mögliche Positionen pro Zelle
			if (randArray[p] == true){
				//Geschwindigkeit bestimmen
				double v = sqrt(3*temperature/comp.gMass());
				double phi, chi; //Winkel Verdrehung um y-, z-Achse
				phi = random1() * 6.2831853; //(0-1)*2*pi
				chi = random1() * 6.2831853;
#ifdef DEBUG
				if (c < 10){
					if (c*4+i <= 9)
						cout << phi << " " << chi << "; ";
				}
#endif
				double velocity[3];
				//erster Vektor velocity = (v, 0 , 0)
				//Drehung um x-Achse passiert nichts
				//Drehung um y-Achse
				velocity[0] = cos(phi)*cos(chi)*v;
				velocity[1] = cos(phi)*sin(chi)*v;
				velocity[2] = -sin(phi)*v;
				//Ausrichtung (variable; feste s.o.)
				double angle = random1()*360;
				double direction[3];
				for (int d=0; d<3; ++d){
					direction[d] = random1();
				}
				Quaternion orientation(angle, direction);
				//Drehimpuls bestimmen
				for(int d = 0; d < 3; ++d){
					//omega[d] = (I[d] == 0)? 0: ((random1() >= 0.5)? 1: -1)*sqrt(2*random1()*temperature/I[d]); //nicht 3*T/I ?
					//omega am Anfang einfach zu Null setzen, dann passt auch die Berechnung der kin. Energie oben
					omega[d] = 0;
				}
				//Position des Moleküls ermitteln
				if (i == 0){//links, unten, hinten
					position[0] = posCell[0];
					position[1] = posCell[1];
					position[2] = posCell[2];
				}
				else if (i == 1){//hinten
					position[0] = posCell[0]+0.5*lengthCellX;
					position[1] = posCell[1]+0.5*lengthCellY;
					position[2] = posCell[2];
				}
				else if (i == 2){//unten
					position[0] = posCell[0]+0.5*lengthCellX;
					position[1] = posCell[1];
					position[2] = posCell[2]+0.5*lengthCellZ;
				}
				else if (i == 3){//links
					position[0] = posCell[0];
					position[1] = posCell[1]+0.5*lengthCellY;
					position[2] = posCell[2]+0.5*lengthCellZ;
				}
				//Molekül eintragen
				Molecule mol;
				mol.sIdNumber(molId);
				mol.sComponentType(1);
				mol.sPosition(position);
				mol.sVelocity(velocity);
				mol.sOrientationQuaternion(orientation);
				mol.sAngularMom(omega);
				sVMolecules(molId-1, mol);
				molId++;
			}
			p++;
		}
	}
#ifdef DEBUG
	cout << endl;
#endif
	//Komponente
#ifdef DEBUG
	cout << comp << endl;
#endif
	header.sTime(0);
	header.sMoleculeFormat("ICRVQD");
	header.sLength(lengthBox);
	header.sComponentDef(comp);
	header.sTemperature(temperature);
	header.sNumberMolecules(numberMolecules);
	header.sComment("created with moledit");
	header.addComment(units);
	header.addComment(dt);
}

int System::fileread(char* filename){
	cout << "reading from xdr-file: " << filename << endl;

	ifstream input;
	input.open(filename);
	
	cout << "all right" << endl;

	string token;
	char c;
	bool isHeader = true;

	//den Kommentar des Headers zu "" setzen
	header.sComment("");

	//zuerst den filetype (mardynconfig-file) überprüfen
	input >> token;
	if (token != "mardyn") cerr << "input file 1 is no mardyn configuration file.\n";
	else input.ignore(1024,'\n');

	double length[3];
	//dann Header
	while (isHeader) {
		input >> c;
		//um die ersten Zeilen mit Kommentaren zu ignorieren
		if(c=='#') {
			input.putback(c);
			char comment[1024];
			input.getline(comment,1024);
			header.addComment((string)comment);
			header.addComment("\n");
			continue;
		}
		//findet das Ende des Headers
		if (isdigit(c)) {
			isHeader = false;
			input.putback(c);
			continue;
		}
		else {
			input.putback(c);
		}

		token.clear();

		input >> token;
		if (token == "t" || token == "currentTime") {
			double time;
			input >> time;
			header.sTime(time);
		}
		if (token == "L" || token == "Length") {
			//wird oben definiert, um unten die Position der Moleküle zu kontrollieren
			for (int i = 0; i <= 2; i++)
				input >> length[i];
			header.sLength(length);
		}
		//input z.B. 1	1 0 0 0 0	0 0 0 1 1 1	0 0 0 1e+10
		//oder auch anders, hängt von der Zahl der LJCenters, charges, quadrupoles, dipoles und tersoff ab
		if (token == "C" || token == "NumberOfComponents") {
			int numberComponents;
			input >> numberComponents;
			int size = 1;
			vector<double> componentDef(1);
			componentDef[0] = numberComponents;
			int j = 1;
			for (int i = 0; i < numberComponents; ++i){
				int componentDef1[5];
				for (int i = 0; i < 5; i++)
					input >> componentDef1[i];
				//berechnen wie groß componentDef sein muss
				size += (componentDef1[0]*8 + componentDef1[1]*5
						+ componentDef1[2]*7 + componentDef1[3]*7 + componentDef1[4]*15);
				//+Größe von componentDef1
				size += 5;
				//+ 3 dummys
				size += 3;
				//cout << size << "\n";
				componentDef.resize(size);
				for (int i = 0; i < 5; ++i){
					componentDef[j] = (double)componentDef1[i];
					++j;
				}
				while (j < size){
					input >> componentDef[j];
					++j;
				}
			}
			//+mixing coefficients; pro Komponentenpaar 2 Koeffizienten
			size += (numberComponents-1)*numberComponents;
			//+Permittivitätszahl
			size += 1;
			componentDef.resize(size);
			while (j < size){
				input >> componentDef[j];
				++j;
			}
			header.sComponentDef(componentDef);
		}
		if (token == "T" || token == "Temperature") {
			double temperature;
			input >> temperature;
			header.sTemperature(temperature);
		}
		if (token == "N" || token == "NumberOfMolecules") {
			int numberMolecules;
			input >> numberMolecules;
			header.sNumberMolecules(numberMolecules);
		}
		if (token == "M" || token == "MoleculeFormat") {
			string moleculeFormat;
			input >> moleculeFormat;
			header.sMoleculeFormat(moleculeFormat); //beeinflusst eigentlich auch componentDef
		}
		else {
			input.ignore(1024,'\n');
		}
	}

#ifdef DEBUG
	cout << "Header:" << endl;
	cout << header;
#endif


	//dann Body
	//1 1	5.0788374 16.316725 5.9219779	0.41884885 0.48231743 1.4006911	1 0 0 0 0 0 0
	//Anzahl der Moleküle aus dem Header
	int numberMol = header.gNumberMolecules();

#ifdef DEBUG
	cout << "statusVMolecules: ";
	statusVMolecules();
#endif

	//erstmal vMolecules auf richtige Größe bringen
	vMolecules.resize(numberMol);

#ifdef DEBUG
	cout << "statusVMolecules: ";
	statusVMolecules();
#endif

	for (int i=0; i<=numberMol-1; i++){
		Molecule molecule;

		int id;
		input >> id;
		molecule.sIdNumber(id);

		int component;
		input >> component;
		molecule.sComponentType(component);

		double pos[3];
		for (int j=0; j<=2; j++)
			input >> pos[j]; //teilweise geringe Ungenauigkeiten beim einlesen, Fehler: (1), bed. 1 Abweichung in der 8. Stelle
		molecule.sPosition(pos);

		//kontrollieren ob die eingelesenen Moleküle innerhalb der Box sind
		for (int j=0; j<3; j++){
			if (pos[j] > length[j]){
				ostringstream ost;
				ost << id;
				string error = "Error(readfile): molecule "+ost.str()+" is outside of simulation box. Aborting.";
				throw Err(error);
			}
		}

		double vel[3];
		for (int j=0; j<=2; j++)
			input >> vel[j];
		molecule.sVelocity(vel);

		double ori[4];
		for (int j=0; j<=3; j++)
			input >> ori[j];
		molecule.sOrientationQuaternion(ori);

		double ang[3];
		for (int j=0; j<=2; j++)
			input >> ang[j];
		molecule.sAngularMom(ang);

		//in vMolecules schreiben
		//vMolecules.push_back(molecule);
		vMolecules[i] = molecule;
	}

	//testen ob das gleiche Molekül eingelesen wurde, kann vorkommen wenn ls1 beim vorherigen Lauf einfach ein Molekül löscht
	if (vMolecules[numberMol-2].gIdNumber() == vMolecules[numberMol-1].gIdNumber()){
		throw eAtLeastTwoMoleculesEqual();
		return 1;
	}

#ifdef DEBUG
	cout << "statusVMolecules: ";
	statusVMolecules();
	cout.precision(8);
	cout << "vMolecules[0]: " << vMolecules[0];
#endif

	input.close();
	return 0;
}

int System::filewrite(const char* filename, const string flag){
	//flag == 'overwrite' || 'keep'
	//erstmal gucken ob Datei schon existiert, wenn flag == 'keep'
	if (flag == "keep") {
		ifstream outputtest;
		outputtest.open(filename);
		if (outputtest.good()){
			cout << "File " << filename << " already exists. Exit." << endl;
			return 1;
		}
		outputtest.close();
		cout << "writing to xdr-file: " << filename << endl;
	}
	else if (flag == "overwrite") {
		cout << "writing to xdr-file (possibly overwriting): " << filename << endl;
	}
	else{
		cout << "System::filewrite: do not know the flag \"" << flag << "\"" << endl;
		return 1;

	}
	//Datei zum Schreiben öffnen
	ofstream output;
	//int prec = 10;
	//output.precision(prec);
	output.open(filename);
	output << header << endl;
	for(int i=0; i<=header.gNumberMolecules()-1; i++)
		output << vMolecules[i];
	output.close();
	return 0;
}

int System::filewriteVTK(string strFilePrefix, const string flag, int nShape)
{
	if (strFilePrefix == ""){
		cout << "System::filewrite: strFilePrefix is not set" << endl;
		return 1;
	}



	// filename of .vtk-fileset
	string strFilename;
	strFilename = strFilePrefix + "_box.vtk";

	//flag == 'overwrite' || 'keep'
	//erstmal gucken ob Datei schon existiert, wenn flag == 'keep'
	if (flag == "keep") {
		ifstream outputtest;
		outputtest.open(strFilename.c_str());
		if (outputtest.good()){
			cout << "Fileset \"" << strFilePrefix.c_str() << "\" already exists. Exit." << endl;
			outputtest.close();
			return 1;
		}
		outputtest.close();
		cout << "writing to vtk-fileset: " << strFilename.c_str() << endl;
	}else if (flag == "overwrite") {
		cout << "writing to vtk-fileset (possibly overwriting): " << strFilePrefix.c_str() << endl;
	}else{
		cout << "System::filewrite: do not know the flag \"" << flag << "\"" << endl;
		return 1;
	}

	// ------------------- BOX -------------------------

	//Datei zum Schreiben öffnen (Box)
	ofstream outputBox;
	strFilename = strFilePrefix + "_box.vtk";
	//int prec = 10;
	//output.precision(prec);
	outputBox.open(strFilename.c_str() );

	// version
    outputBox << "# vtk DataFile Version 3.1 \n";

    // header
    outputBox << "vtk output \n";
    outputBox << "ASCII \n";
    outputBox << "DATASET UNSTRUCTURED_GRID \n";

    // Daten
    outputBox << "POINTS " << 8 << " FLOAT \n";

    // Ecken der Box
	Header head;
	head = this->gHeader();

    //hole Anzahl der Komponenten aus Header
    vector<double> componentDef = head.gComponentDef();
    int nNumComponents = componentDef[0];


    //hole sigma-werte aus dem header
    int size_header = 1;
    int size_header_pre = 0;
    vector<double> sigmaOfComponent(nNumComponents);
    for (int ix = 0; ix < nNumComponents; ++ix){
				//berechnen wie groß componentDef sein muss
				//cout << componentDef[1] << " " << componentDef[2] << " " << componentDef[3] << " " << componentDef[4] << " " << componentDef[5] << "\n"; //debug zeug
				size_header += (componentDef[1]*8 + componentDef[2]*5
						+ componentDef[3]*7 + componentDef[4]*7 + componentDef[5]*15);
				//+Größe von componentDef
				size_header += 5;
				//+ 3 dummys
				size_header += 3;
                //cout << size_header << "\n"; //debug zeug
                sigmaOfComponent[ix] = componentDef[size_header_pre+5+6]; //size_header_pre: offset der komponente, 5: componentDef, 6: sigma
                size_header_pre = size_header -1;
    }

	double* length;
	length = head.gLength();
	int edges[8][3] = {
			            { 0, 0, 0 },
			            { 0, 0, 1 },
			            { 0, 1, 0 },
			            { 0, 1, 1 },
			            { 1, 0, 0 },
			            { 1, 0, 1 },
			            { 1, 1, 0 },
			            { 1, 1, 1 },
	                  };
	for(int i=0; i<8; i++)
	{
		edges[i][0] = edges[i][0] * length[0];
		edges[i][1] = edges[i][1] * length[1];
		edges[i][2] = edges[i][2] * length[2];

		outputBox << edges[i][0] << " " << edges[i][1] << " " << edges[i][2] << " \n";
	}
	outputBox << "\n";

	// cell types
	outputBox << "CELLS 12 36 \n";
	outputBox << "2 0 1 \n";
	outputBox << "2 0 2 \n";
	outputBox << "2 0 4 \n";
	outputBox << "2 6 2 \n";
	outputBox << "2 6 4 \n";
	outputBox << "2 6 7 \n";
	outputBox << "2 3 1 \n";
	outputBox << "2 3 2 \n";
	outputBox << "2 3 7 \n";
	outputBox << "2 5 1 \n";
	outputBox << "2 5 4 \n";
	outputBox << "2 5 7 \n";
	outputBox << "\n";
	outputBox << "CELL_TYPES 12 \n";
	outputBox << "3 3 3 3 3 3 3 3 3 3 3 3 \n";
	outputBox << "\n";

	outputBox.close();

	// ------------------ MOLECULES ------------------------

	//Datei zum Schreiben öffnen (Component)
	int i;
	ostringstream convert;
	vector<Molecule>::iterator it;
    int nNumMols, nType;
	double* molPos;
	ofstream outputComp;

	for(i = 1; i <= nNumComponents; i++)
	{
        convert.str("");
        convert << i;
		strFilename = strFilePrefix + "_comp" + convert.str() + ".vtk";
		//int prec = 10;
		//output.precision(prec);
		outputComp.open(strFilename.c_str() );

		// version
		outputComp << "# vtk DataFile Version 3.1 \n";

		// header
		outputComp << "vtk output \n";
		outputComp << "ASCII \n";
		outputComp << "DATASET UNSTRUCTURED_GRID \n";

		// Daten
		nNumMols = 0;

		for (it=this->vMolecules.begin(); it != this->vMolecules.end(); ++it)
		{
			nType = (*it).gComponentType();
			if(nType == i)
			{
				nNumMols++;
			}
		}

		outputComp << "POINTS " << nNumMols << " FLOAT \n";

		for (it=this->vMolecules.begin(); it != this->vMolecules.end(); ++it)
		{
			nType = (*it).gComponentType();
			if(nType == i)
				{
					molPos = (*it).gPosition();

					for (unsigned short d = 0; d < 3; d++)
						outputComp << setw(11) << molPos[d];

					outputComp << "\n";
				}
		}
		outputComp << "\n";

		outputComp.close();
	}

	// ------------------- PYTHON-Script -------------------------

	// python script file *.py
	ofstream outputPython;

	//farben fuer die einzelnen komponenten
    vector<string> color(10);
    color[0] = "0.0, 0.0, 1.0";
    color[1] = "1.0, 0.0, 0.0";
    color[2] = "0.0, 1.0, 0.0";
    color[3] = "1.0, 1.0, 0.0";
    color[4] = "0.0, 1.0, 1.0";
    color[5] = "1.0, 0.0, 1.0";
    color[6] = "1.0, 0.5, 0.0";
    color[7] = "0.5, 1.0, 0.0";
    color[8] = "0.0, 1.0, 0.5";
    color[9] = "0.0, 0.5, 1.0";

	//bereite Dateiname fuer Python-Script vor
	strFilename = strFilePrefix + ".py";
	outputPython.open(strFilename.c_str());

	//extrahiere nur den Dateinamen ohne Pfadangabe und Dateiendung, funktioniert nur unter unix
	strFilename = strFilePrefix;
	const size_t last_slash_idx = strFilename.find_last_of("/");
	if (string::npos != last_slash_idx)
	{
		strFilename.erase(0, last_slash_idx + 1);
	}

    //python-header
	outputPython << "from paraview.simple import *\n";
	outputPython << "paraview.simple._DisableFirstRenderCameraReset()\n";
	outputPython << "\n";

	//kamera-einstellungen
	outputPython << "RenderView1 = GetRenderView()\n";
    outputPython << "RenderView1.CameraPosition = [9.0, 27.5, 126.08041089866609]\n";
	outputPython << "RenderView1.CameraFocalPoint = [9.0, 27.5, 9.0]\n";
	outputPython << "RenderView1.CameraClippingRange = [97.14598463925829, 141.80842813735666]\n";
	outputPython << "RenderView1.CameraParallelScale = 30.30264014900352\n";
    outputPython << "RenderView1.CenterOfRotation = [9.0, 27.5, 9.0]\n";
	outputPython << "\n";

	//laden der vtk-dateien

	//laden der box
	outputPython << strFilename <<"_box_vtk = LegacyVTKReader(FileNames=['"<< strFilename <<"_box.vtk'], guiName = \""<< strFilename <<"_box\")\n";
    outputPython << "DataRepresentation1 = Show()\n";
	outputPython << "\n";

	//laden der einzelnen komponenten

	for(i = 1; i <= nNumComponents; i++)
	{
        outputPython << strFilename <<"_comp" << i << "_vtk = LegacyVTKReader(FileNames=['"<< strFilename <<"_comp" << i << ".vtk'], guiName = \""<< strFilename <<"_comp" << i << "\")\n";
        outputPython << "DataRepresentation" << i+1 << " = Show()\n";
        outputPython << "\n";
    }

	//einstellen der darstellungsoptionen der einzelnen komponenten

	switch(nShape)
	{
	case 1:
		for(i = 1; i <= nNumComponents; i++)
		{
			outputPython << "SetActiveSource("<< strFilename <<"_comp" << i << "_vtk)\n";
			outputPython << "Glyph" << i << " = Glyph( GlyphType=\"2D Glyph\", GlyphTransform=\"Transform2\" )\n";
			outputPython << "\n";
			outputPython << "Glyph" << i << ".Scalars = ['POINTS', '']\n";
			outputPython << "Glyph" << i << ".Vectors = ['POINTS', '']\n";
			outputPython << "\n";
			outputPython << "Glyph" << i << ".SetScaleFactor = 1.0\n";
			outputPython << "\n";
			outputPython << "Glyph" << i << ".GlyphType.GlyphType = 'Vertex'\n";
			outputPython << "\n";
			outputPython << "DataRepresentation" << i+3 << " = Show()\n";
			outputPython << "\n";
			outputPython << "DataRepresentation" << i+3 << ".DiffuseColor = [" << color[i-1] << "]\n";
			outputPython << "\n";
		}
	break;
	case 2:
		for(i = 1; i <= nNumComponents; i++)
		{
			outputPython << "SetActiveSource("<< strFilename <<"_comp" << i << "_vtk)\n";
			outputPython << "Glyph" << i << " = Glyph( GlyphType=\"Sphere\", GlyphTransform=\"Transform2\" )\n";
			outputPython << "\n";
			outputPython << "Glyph" << i << ".Scalars = ['POINTS', '']\n";
			outputPython << "Glyph" << i << ".Vectors = ['POINTS', '']\n";
			outputPython << "\n";
			outputPython << "Glyph" << i << ".SetScaleFactor = 1.0\n";
			outputPython << "\n";
			outputPython << "Glyph" << i << ".GlyphType.Radius = "<< sigmaOfComponent[i-1]/2 <<"\n"; //radius einfuegen
			outputPython << "\n";
			outputPython << "DataRepresentation" << i+3 << " = Show()\n";
			outputPython << "\n";
			outputPython << "DataRepresentation" << i+3 << ".DiffuseColor = [" << color[i-1] << "]\n";
			outputPython << "\n";
		}
	break;
	}

	//darstellung rendern
	outputPython << "Render()\n";

	outputPython.close();
	return 0;
}

Header System::gHeader() const{
	return header;
}

int System::sHeader(Header head){
	header = head;
	return 0;
}

int System::initVMolecules(int numberMolecules){
	vMolecules.resize(numberMolecules);
	return 0;
}

int System::sVMolecules(unsigned index, Molecule input){
	vMolecules[index] = input;
	return 0;
}

int System::sVMolecules(vector<Molecule> input){
	vMolecules = input;
	return 0;
}

int System::sVMolecules(vector<vector<double> > vMolPos, vector<vector<double> > vMolVel, vector<vector<double> > vMolOrient, vector<vector<double> > vMolRotSpeed){
	unsigned int numberMolecules = vMolPos.size();
	initVMolecules(numberMolecules);
	Molecule mol;
	for (int i=0; i<int(numberMolecules); i++){
		mol.sIdNumber(i+1);
		mol.sComponentType(1);
		double pos[3] = {vMolPos[i][0],vMolPos[i][1],vMolPos[i][2]};
		mol.sPosition(pos);
		double vel[3] = {vMolVel[i][0],vMolVel[i][1],vMolVel[i][2]};
		mol.sVelocity(vel);
		double quat[4] = {vMolOrient[i][0],vMolOrient[i][1],vMolOrient[i][2],vMolOrient[i][3]};
		mol.sOrientationQuaternion(quat);
		double angMom[3] = {vMolRotSpeed[i][0],vMolRotSpeed[i][1],vMolRotSpeed[i][2]};
		mol.sAngularMom(angMom);
		sVMolecules(i, mol);
	}
	return 0;
}

int System::sVMolecules(vector<vector<double> > vMolPos, vector<vector<double> > vMolVel, int molOrient, int molRotSpeed){
	unsigned int size = vMolPos.size();
	vector<double> vRotSpeed3(3,0);
	vector<double> vQuat4(4,0);
	vQuat4[0] = 1;
	vector<vector<double> > vMolOrient(size,vQuat4);
	vector<vector<double> > vMolRotSpeed(size,vRotSpeed3);
	int ret = sVMolecules(vMolPos, vMolVel, vMolOrient, vMolRotSpeed);
	return ret;
}



vector<Molecule> System::gVMolecules() const{
	return vMolecules;
}

int System::statusVMolecules() const{
	cout << "max_size() = " << vMolecules.max_size() << ";";
	cout << "\tsize() = " << vMolecules.size() << ";";
	cout << "\tcapacity() = " << vMolecules.capacity() << ";" << endl;
	return 0;
}

int System::changeComponentType(int numberNewTypes, int possibilityPercentKeep, Component comp, double eta, double xi){ //funktioniert momentan nur mit numberNewTypes=2;
	if (possibilityPercentKeep > 100 || possibilityPercentKeep < 0){
		cerr << "Error: possibilityPercentKeep is not in percent. Exit.\n" << endl;
		return 1;
	}
	//srand(time(NULL)); //initialisiert den pseudo-random number generator
#ifdef DEBUG
	cout << "Random Type Numbers for the first 10 Molecules: ";
#endif
	for(unsigned int i=0; i<=vMolecules.size()-1; i++){
		int randomNumber = rand()%100; //zufällige Zahl [0:99]
		if (randomNumber>=possibilityPercentKeep)
			vMolecules[i].sComponentType(2); //else keep
#ifdef DEBUG
		if (i<=9)
			cout << vMolecules[i].gComponentType();
#endif
	}
#ifdef DEBUG
	cout << endl;
#endif

	header.addComponentDef(comp, eta, xi);
	return 0;
}

int System::rotateSystem(int rotateDirection){
	//rotateDirection:
	//0: drehen um x-Achse
	//1: drehen um y-Achse
	//2: drehen um z-Achse
	if (rotateDirection > 2 || rotateDirection < 0){
		cerr << "System::rotateSystem - rotateDirektion muss zwischen 0 und 2 liegen";
		exit(1);
	}
	double length[3];
	double newlength[3];
	for(int d=0; d<=2; d++) length[d] = header.gLength()[d];
	for(int d=0; d<=2; d++) *(newlength+d) = *(length+d);
	double position[3];
	double newposition[3];
	double velocity[3];
	double newvelocity[3];
	double angularMom[3];
	double newangularMom[3];
	Quaternion orientationQuat;
	for(int i=0; i<=2; i++){
		for(int j=0; j<=2; j++){
			if(i!=rotateDirection && j!=rotateDirection && i!=j){
				//Länge des Systems ändern
				newlength[i] = length[j];
			}
		}
	}
	header.sLength(newlength);

	//Schleife über alle Moleküle
	for(unsigned int m=0; m<=vMolecules.size()-1; m++){
		for(int d=0; d<=2; d++) position[d] = vMolecules[m].gPosition()[d];
		for(int d=0; d<=2; d++) newposition[d] = position[d];
		for(int d=0; d<=2; d++) velocity[d] = vMolecules[m].gVelocity()[d];
		for(int d=0; d<=2; d++) newvelocity[d] = velocity[d];
		for(int d=0; d<=2; d++) angularMom[d] = vMolecules[m].gAngularMom()[d];
		for(int d=0; d<=2; d++) newangularMom[d] = angularMom[d];
		orientationQuat = vMolecules[m].gOrientationQuaternion();
		//diese Richtung muss bezüglich des Ursprungs neu berechnet werden
		int neworigin = (rotateDirection+1)%3;
		for(int i=0; i<=2; i++){
			for(int j=0; j<=2; j++){
				if(i!=rotateDirection && j!=rotateDirection && i!=j){
					//Position, Geschwindigkeit und Drehimpuls der Moleküle anpassen
					if(i == neworigin){
						newposition[i] = length[j]-position[j];
						newvelocity[i] = -velocity[j];
						(angularMom[j] == 0)? newangularMom[i] = 0: newangularMom[i] = -angularMom[j];
					}
					else{
						newposition[i] = position[j];
						newvelocity[i] = velocity[j];
						newangularMom[i] = angularMom[j];
					}
				}
			}
		}
		vMolecules[m].sPosition(newposition);
		vMolecules[m].sVelocity(newvelocity);
		vMolecules[m].sAngularMom(newangularMom);
		//Ausrichtung der Moleküle anpassen
		//multiplizieren
		orientationQuat = vMolecules[m].gOrientationQuaternion();
		Quaternion quat1;
		double rotateQuaternion[4] = {0.7071,0,0,0};
		rotateQuaternion[rotateDirection+1] = 0.7071;
		quat1.sCoordinates(rotateQuaternion);
		orientationQuat*quat1; //memberfunction; ändert Quaternion orientationQuat: orientationQuat = quat1*orientationQuat;
		vMolecules[m].sOrientationQuaternion(orientationQuat);
	}
	return 0;
}

int System::trimTo(double start[3], double end[3]){
	mark.oblong(start, end, vMolecules);
	mark.others();
	mark.del(vMolecules);
	int numMol = vMolecules.size();
	double length[3];
	for (unsigned d = 0; d <= 2; d++)
		length[d] = abs(end[d]-start[d]);
	header.sLength(length);
	header.sNumberMolecules(numMol);
	return 0;
}

int System::shrink(double shrink){
	//System verkleinern und halos auf den Flächen einfügen mit der Tiefe von shrink
	double* lengthT = header.gLength();
	double length[3];
	for (unsigned d = 0; d <= 2; d++)
		length[d] = lengthT[d];
	double diff = 2*shrink;
	double shrinkFactor[3];
	for (unsigned d = 0; d <= 2; d++){
		double newLength = length[d]-diff;
		shrinkFactor[d] = newLength/length[d];
	}
	//newPos = (pos*shrinkFactor) + shrink
	unsigned numMol = vMolecules.size();
	for (unsigned i = 0; i <= numMol-1; i++){
		double* pos = vMolecules[i].gPosition();
		double newPos[3];
		for (unsigned d = 0; d <= 2; d++)
			newPos[d] = (pos[d]*shrinkFactor[d])+shrink;
		vMolecules[i].sPosition(newPos);
	}
	return 0;
}

int System::eliminateRelativeMovement(){
	double temperature1 = 0;
	double vel[3] = {0, 0, 0};
	unsigned numMol = vMolecules.size();
	for (unsigned i = 0; i <= numMol-1; ++i){
		for (unsigned j = 0; j < 3; ++j){
			vel[j] += vMolecules[i].gVelocity()[j];
		}
		temperature1 += vMolecules[i].gVelocity()[0]*vMolecules[i].gVelocity()[0]
			   + vMolecules[i].gVelocity()[1]*vMolecules[i].gVelocity()[1]
			   + vMolecules[i].gVelocity()[2]*vMolecules[i].gVelocity()[2];
	}
	for (unsigned i = 0; i < 3; ++i){
		vel[i] = vel[i]/numMol;
	}
	cout << "relative movement before elimination: " << vel[0] << " " << vel[1] << " " << vel[2] << endl;
	cout << "temperature*/mass_comp* before elimination : " << temperature1/3/numMol << endl;
	double molVel[3];
	for (unsigned i = 0; i <= numMol-1; ++i){
		for (unsigned j = 0; j < 3; ++j){
			molVel[j] = vMolecules[i].gVelocity()[j]-vel[j];
		}
		vMolecules[i].sVelocity(molVel);
	}
	for (unsigned i = 0; i < 3; ++i)
		vel[i] = 0;
	double temperature2 = 0;
	for (unsigned i = 0; i <= numMol-1; ++i){
		for (unsigned j = 0; j < 3; ++j){
			vel[j] += vMolecules[i].gVelocity()[j];
		}
		temperature2 += vMolecules[i].gVelocity()[0]*vMolecules[i].gVelocity()[0]
			   + vMolecules[i].gVelocity()[1]*vMolecules[i].gVelocity()[1]
			   + vMolecules[i].gVelocity()[2]*vMolecules[i].gVelocity()[2];
	}
	for (unsigned i = 0; i < 3; ++i){
		vel[i] = vel[i]/numMol;
	}
	cout << "relative movement after elimination: " << vel[0] << " " << vel[1] << " " << vel[2] << endl;
	cout << "temperature*/mass_comp* after elimination : " << temperature2/3/numMol << endl;
	double temperatureFactor = sqrt(temperature1/temperature2);
	for (unsigned i = 0; i <= numMol-1; ++i){
		for (unsigned j = 0; j < 3; ++j){
			molVel[j] = vMolecules[i].gVelocity()[j]*temperatureFactor;
		}
		vMolecules[i].sVelocity(molVel);
	}
	double temperature3 = 0;
	for (unsigned i = 0; i < 3; ++i)
		vel[i] = 0;
	for (unsigned i = 0; i <= numMol-1; ++i){
		for (unsigned j = 0; j < 3; ++j){
			vel[j] += vMolecules[i].gVelocity()[j];
		}
		temperature3 += vMolecules[i].gVelocity()[0]*vMolecules[i].gVelocity()[0]
			   + vMolecules[i].gVelocity()[1]*vMolecules[i].gVelocity()[1]
			   + vMolecules[i].gVelocity()[2]*vMolecules[i].gVelocity()[2];
	}
	for (unsigned i = 0; i < 3; ++i){
		vel[i] = vel[i]/numMol;
	}
	cout << "relative movement after T correction: " << vel[0] << " " << vel[1] << " " << vel[2] << endl;
	cout << "temperature after T correction : " << temperature3/3/numMol << endl;
	return 0;
}

vector<vector<double> > System::readMoleculesPos(char* filename){
	vector<vector<double> > vPos;
	cout << "reading position from file: " << filename << endl;
	ifstream input;
	input.open(filename);
	vector<double> pos;
	pos.resize(3);
	input >> pos[0]; //hier 1. Element des Arrays
	while (!input.eof()){
		for (int d=1; d<=2; d++){ //hier nur 2. und 3. Element des Arrays
			input >> pos[d];
		}
		vPos.push_back(pos);
		input >> pos[0]; //hier 1. Element des Arrays
	}
	return vPos;
}

vector<vector<double> > System::readMoleculesVel(char* filename){
	vector<vector<double> > vVel;
	cout << "reading position from file: " << filename << endl;
	ifstream input;
	input.open(filename);
	vector<double> vel;
	vel.resize(3);
	input >> vel[0]; //hier 1. Element des Arrays
	while (!input.eof()){
		for (int d=1; d<=2; d++){ //hier nur 2. und 3. Element des Arrays
			input >> vel[d];
		}
		vVel.push_back(vel);
		input >> vel[0]; //hier 1. Element des Arrays
	}
	return vVel;
}

int System::merge(double referenzPunkt[3], System system2, double referenzPunkt2[3]){
	/*
	 * merge nimmt ein weiteres System und erstellt für beide jeweils einen Referenzpunkt. Diese werden nun
	 * übereinandergelegt und das 2. System so mit dem ersten zusammengeführt.
	 * geht erstmal nur für gleiche Komponenten
	 */
	//Relation beider Systeme zum ihrem jeweiligen referenzPunkt (Abstand der Systemgrenzen von ihrem Referenzpunkt)
	double rel[3][2];
	double rel2[3][2];
	double* length = header.gLength();
	double* length2 = system2.gHeader().gLength();
	for(int d = 0; d < 3; d++){
		rel[d][0] = -referenzPunkt[d];
		rel[d][1] = length[d]-referenzPunkt[d];
	}
	for(int d = 0; d < 3; d++){
		rel2[d][0] = -referenzPunkt2[d];
		rel2[d][1] = length2[d]-referenzPunkt2[d];
	}
	//Relation des neuen Systems von seinem Referenzpunkt
	double relNeu[3][2];
	for(int d = 0; d < 3; d++){
		if(abs(rel[d][0]) > abs(rel2[d][0])){
			relNeu[d][0] = rel[d][0];
		}
		else{
			relNeu[d][0] = rel2[d][0];
		}
		if(abs(rel[d][1]) > abs(rel2[d][1])){
			relNeu[d][1] = rel[d][1];
		}
		else{
			relNeu[d][1] = rel2[d][1];
		}
	}
	//neue Länge der Simulationsbox
	double lengthNeu[3];
	for(int d = 0; d < 3; d++){
		lengthNeu[d] = relNeu[d][1]-relNeu[d][0];
	}
	//Offset für beide System
	double offset[3];
	double offset2[3];
	for(int d = 0; d < 3; d++){
		offset[d] = rel[d][0]-relNeu[d][0];
		offset2[d] = rel2[d][0]-relNeu[d][0];
	}
	//Länge und Anzahl der Moleküle des Systems aktualisieren
	double lengthNeuF[3];
	for(int d = 0; d < 3; d++)
		lengthNeuF[d] = lengthNeu[d];
	header.sLength(lengthNeuF);
	int numMol = header.gNumberMolecules();
	int numMol2 = system2.gHeader().gNumberMolecules();
	int numMolNeu = numMol+numMol2;
	header.sNumberMolecules(numMolNeu);
	//neuen Molekülvektor erstellen und alten ersetzen
	vector<Molecule> vMoleculesNeu;
	vector<Molecule> vMolecules2 = system2.gVMolecules();
	Molecule molecule;
	for (int n = 0; n < numMol; n++){
		molecule = vMolecules[n];
		double* pos = molecule.gPosition();
		for(int d = 0; d < 3; d++){
			pos[d] += offset[d];
		}
		molecule.sPosition(pos);
		vMoleculesNeu.push_back(molecule);
	}
	for (int n = 0; n < numMol2; n++){
		molecule = vMolecules2[n];
		molecule.sIdNumber(numMol+n+1);
		double* pos = molecule.gPosition();
		for(int d = 0; d < 3; d++){
			pos[d] += offset2[d];
		}
		molecule.sPosition(pos);
#ifdef DEBUG
		if (n == 0){
			cout << "merging..." << endl;
			cout << "last old and first new molecule id: " << numMol << " " << numMol+n+1 << endl;
			cout << "first new molecule position: " << pos[0] << " " << pos[1] << " " << pos[2] << endl;
		}
#endif //DEBUG
		vMoleculesNeu.push_back(molecule);
	}
	sVMolecules(vMoleculesNeu);
	//alles geklappt
	return 0;
}

