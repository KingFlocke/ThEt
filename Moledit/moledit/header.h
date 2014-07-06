/*
 * header.h
 *
 *  Created on: 22.10.2010
 *      Author: eck
 */

#ifndef HEADER_H_
#define HEADER_H_

#include <string>
#include <vector>

#include "component.h"

class Header{
private:
	double actTime;
	double length[3];
	vector<double> componentDef;
	double temperature;
	int numberMolecules;
	string moleculeFormat;
	string comment;
public:
	int addExecComment(string version, string revision, int argc, char** argv);
	double gTime() const;
	int sTime(double input);
	string gMoleculeFormat() const;
	int sMoleculeFormat(string input);
	double* gLength();
	int sLength(double input[3]);
	vector<double> gComponentDef();
	string outComponentDef();
	int sComponentDef(const vector<double> input);
	int sComponentDef();
	int sComponentDef(Component comp);
	int addComponentDef(Component comp, double eta, double xi);
	double gTemperature() const;
	int sTemperature(double input);
	int gNumberMolecules() const;
	int sNumberMolecules(int input);
	string gComment() const;
	int sComment(string input);
	int addComment(string input);
	friend ostream& operator<< (ostream& os, Header& head);
	int read(char* filename);
};

#endif /* HEADER_H_ */
