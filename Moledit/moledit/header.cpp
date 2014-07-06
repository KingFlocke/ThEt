/*
 * header.cpp
 *
 *  Created on: 22.10.2010
 *      Author: eck
 */

#include <iostream>
#include <sstream>
#include <time.h>

using namespace std;

#include "header.h"

int Header::addExecComment(string version, string revision, int argc, char** argv){
	//Kommentar (Aufruf) setzen
	string comm, tmp;
	comm = "created: ";
	time_t rawtime;
	//struct tm * timeinfo;
	time(&rawtime);
	tmp = asctime(localtime(&rawtime));
	comm += tmp;
	comm.erase(comm.size()-1,1);
	addComment(comm);
	comm = "Version: ";
	comm += version;
	comm += " ";
	comm += revision;
	addComment(comm);
	comm = "moledit invocation: ";
	for (int i = 0; i < argc; ++i){
		tmp = argv[i];
		comm += tmp+" ";
	}
	addComment(comm);
	return 0;
}

//get functions
double Header::gTime() const{
	return actTime;
}

string Header::gMoleculeFormat() const{
	return moleculeFormat;
}

//set functions
int Header::sTime(double input){
	actTime = input;
	return 0;
}

int Header::sMoleculeFormat(string input){
	moleculeFormat = input;
	return 0;
}

double* Header::gLength(){
	return length;
}

int Header::sLength(double input[3]){
	for (int i=0; i<=2; i++)
		length[i] = input[i];
	return 0;
}

int Header::gNumberMolecules() const{
	return numberMolecules;
}

int Header::sNumberMolecules(int input){
	numberMolecules = input;
	return 0;
}

double Header::gTemperature() const{
	return temperature;
}

int Header::sTemperature(double input){
	temperature = input;
	return 0;
}

vector<double> Header::gComponentDef(){
	return componentDef;
}

string Header::outComponentDef(){
	string out;
	ostringstream ost;
	ost.precision(15);
	//Anzahl an Komponenten
	ost << componentDef[0];
	out = ost.str()+"\n";
	ost.str("");
	//Anzahl an LJ-Zentren, Ladungen, Dipolen, Quadrupolen, Tersoff
	int i = 1;
	for (int n = 0; n <= componentDef[0]-1; n++){ //Ausgabe für die jeweilige Komponente
		int indNumberLJ = i;
		int indNumberCharges = i+1;
		int indNumberDipoles = i+2;
		int indNumberQuadrupoles = i+3;
		int b = i+4;
		while (i <= b){
			ost << componentDef[i];
			out += ost.str()+" ";
			ost.str("");
			i++;
		}
		out += "\n";
		//LJ-Center
		for (int a = 0; a <= componentDef[indNumberLJ]-1; a++){
			ost << componentDef[i];
			out += ost.str()+" ";
			ost.str("");
			i++;
			ost << componentDef[i];
			out += ost.str()+" ";
			ost.str("");
			i++;
			ost << componentDef[i];
			out += ost.str()+"\t";//Tabulator
			ost.str("");
			i++;
			ost << componentDef[i];
			out += ost.str()+" ";
			ost.str("");
			i++;
			ost << componentDef[i];
			out += ost.str()+" ";
			ost.str("");
			i++;
			ost << componentDef[i];
			out += ost.str()+"\t";//Tabulator
			ost.str("");
			i++;
			//rc_truncated und do_shift
			ost << componentDef[i];
			out += ost.str()+" ";
			ost.str("");
			i++;
			ost << componentDef[i];
			out += ost.str()+"\n";//Zeilenumbruch
			ost.str("");
			i++;
		}
		//Ladungen
		for (int a = 0; a <= componentDef[indNumberCharges]-1; a++){
			ost << componentDef[i];
			out += ost.str()+" ";
			ost.str("");
			i++;
			ost << componentDef[i];
			out += ost.str()+" ";
			ost.str("");
			i++;
			ost << componentDef[i];
			out += ost.str()+"\t";//Tabulator
			ost.str("");
			i++;
			ost << componentDef[i];
			out += ost.str()+" ";
			ost.str("");
			i++;
			ost << componentDef[i];
			out += ost.str()+"\n";//Zeilenumbruch;
			ost.str("");
			i++;
		}
		//Dipole
		for (int a = 0; a <= componentDef[indNumberDipoles]-1; a++){
			ost << componentDef[i];
			out += ost.str()+" ";
			ost.str("");
			i++;
			ost << componentDef[i];
			out += ost.str()+" ";
			ost.str("");
			i++;
			ost << componentDef[i];
			out += ost.str()+"\t";//Tabulator
			ost.str("");
			i++;
			ost << componentDef[i];
			out += ost.str()+" ";
			ost.str("");
			i++;
			ost << componentDef[i];
			out += ost.str()+" ";
			ost.str("");
			i++;
			ost << componentDef[i];
			out += ost.str()+"\t";//Tabulator
			ost.str("");
			i++;
			ost << componentDef[i];
			out += ost.str()+"\n";//Zeilenumbruch
			ost.str("");
			i++;
		}
		//Quadrupole
		for (int a = 0; a <= componentDef[indNumberQuadrupoles]-1; a++){
			ost << componentDef[i];
			out += ost.str()+" ";
			ost.str("");
			i++;
			ost << componentDef[i];
			out += ost.str()+" ";
			ost.str("");
			i++;
			ost << componentDef[i];
			out += ost.str()+"\t";//Tabulator
			ost.str("");
			i++;
			ost << componentDef[i];
			out += ost.str()+" ";
			ost.str("");
			i++;
			ost << componentDef[i];
			out += ost.str()+" ";
			ost.str("");
			i++;
			ost << componentDef[i];
			out += ost.str()+"\t";//Tabulator
			ost.str("");
			i++;
			ost << componentDef[i];
			out += ost.str()+"\n";//Zeilenumbruch
			ost.str("");
			i++;
		}
		//Dummys
		for (int a = 0; a <= 2; a++){
			ost << componentDef[i];
			out += ost.str()+" ";
			ost.str("");
			i++;
		}
		out += "\n";
	}
	//eta und xi; wenn 2 Komponenten vorhanden sind
	if (componentDef[0]-1){
		ost << componentDef[i];
		out += ost.str()+" ";
		ost.str("");
		i++;
		ost << componentDef[i];
		out += ost.str()+" ";
		ost.str("");
		i++;
		out += "\n";
	}
	//1e10
	ost << componentDef[i];
	out += ost.str()+" ";
	ost.str("");
	i++;

	return out;
}

int Header::sComponentDef(const vector<double> input){
	unsigned int size = input.size();
	componentDef.resize(size);
	for (unsigned int i=0; i < size; i++)
		componentDef[i] = input[i];
	return 0;
}

int Header::sComponentDef(){
	componentDef.resize(1);
	componentDef[0] = 0;
	return 0;
}

int Header::sComponentDef(Component comp){
	//erstellt den Header mit EINER spezifischen Komponente component
	vector<double> component;
	component = comp.gComponent();

	componentDef.resize(component.size()+2);
	componentDef[0] = 1;
	unsigned i = 0;
	while (i <= component.size()-1){
		componentDef[i+1] = component[i];
		i++;
	}
	//noch _epsilonRF
	componentDef[i+1] = 1e+10;
	return 0;
}

int Header::addComponentDef(Component comp, double eta, double xi){
	//fügt dem Header eine weitere Komponente hinzu

	int n = 6;//richtige Stelle finden
	for (int a = 0; a <= componentDef[1]-1; a++){//LJ-Center
		n = n+8;
	}
	for (int a = 0; a <= componentDef[2]-1; a++){//Ladungen
		n = n+5;
	}
	for (int b = 0; b <= componentDef[3]-1; b++){//Dipole
		n = n+7;
	}
	for (int c = 0; c <= componentDef[4]-1; c++){//Quadrupole
		n = n+7;
	}
	n = n+3;//dummys
	componentDef[0]++; //eine Komponente mehr
	int nNeu = comp.size()+3;//3 für dummy
	componentDef.resize(n+nNeu+2+1);
	//neue Komponente eintragen
	unsigned i = 0;
	while (i <= comp.size()-1){
		componentDef[n] = comp[i];
		n++;
		i++;
	}
	//dummys
	for (int i = 0; i <= 2; i++){
		componentDef[n] = 0;
		n++;
	}
	//eta und xi für Kombinationsregel
	componentDef[n] = eta;
	componentDef[n+1] = xi;
	n += 2;
	componentDef[n] = 1.0e+10;//

	return 0;
}

string Header::gComment() const{
	return comment;
}

int Header::sComment(string input){
	comment = "#"+input+"\n";
	return 0;
}

int Header::addComment(string input){
	comment += "#"+input+"\n";
	return 0;
}

ostream& operator<< (ostream& os, Header& head){
	os << "mardyn trunk 20100210" << endl;
	os << head.comment; //gibt alle Kommentare gesammelt ab der zweiten Zeile aus
	os.precision(8);
	os << "t\t" << head.gTime() << endl;
	os << "L\t" << head.gLength()[0] << " " << head.gLength()[1] << " " << head.gLength()[2] << endl;
	os << "T\t" << head.gTemperature() << endl;
	os << "C\t" << head.outComponentDef() << endl;
	os << "N\t" << head.gNumberMolecules() << endl;
	os << "M\t" << head.gMoleculeFormat() << endl;
	return os;
}

