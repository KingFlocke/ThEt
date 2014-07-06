/*
 * component.h
 *
 *  Created on: 27.01.11
 *      Author: eck
 */

#ifndef COMPONENT_H_
#define COMPONENT_H_

#include <string>
#include <vector>
#include <cmath> //für pow() und sqrt()


using namespace std;

class Component{
private:
	int _numberLJCenters;
	int _numberCharges;
	int _numberQuadrupoles;
	//wenn COMPLEX_POTENTIAL_SET braucht man auch
	int _numberDipoles;
	int _numberTersoff; //Modell um Feststoffe zu simulieren, fokussiert die Verbindungen zwischen Atomen
	//LJ center position
	vector<double> _vLJx;
	vector<double> _vLJy;
	vector<double> _vLJz;
	//LJ center Masse
	vector<double> _vLJm;
	//LJ center Energie
	vector<double> _vLJeps;
	//LJ center Länge
	vector<double> _vLJsigma;
	//Abschneideradius; shifted ja oder nein
	vector<double> _vLJrcTruncated;
	vector<bool> _vLJdoShift;
	//Ladung Position
	vector<double> _vchargeX;
	vector<double> _vchargeY;
	vector<double> _vchargeZ;
	//Ladung Masse
	vector<double> _vchargeM;
	//Ladung Ladung
	vector<double> _vchargeQ;
	//Dipol Position
	vector<double> _vdipoleX;
	vector<double> _vdipoleY;
	vector<double> _vdipoleZ;
	//Dipolmoment
	vector<double> _vdipoleMyx;
	vector<double> _vdipoleMyy;
	vector<double> _vdipoleMyz;
	vector<double> _vdipoleMyabs;
	//Quadrupol Position
	vector<double> _vquadrupoleX;
	vector<double> _vquadrupoleY;
	vector<double> _vquadrupoleZ;
	//Quadrupolmoment
	vector<double> _vquadrupoleQx;
	vector<double> _vquadrupoleQy;
	vector<double> _vquadrupoleQz;
	vector<double> _vquadrupoleQabs;
	//Trägheitsmoment
	double ixx; //in Richtung der x-Achse
	double iyy;
	double izz;
	//allgemeine Parameter
	double eps;
	double sigma;
	double mass;
	double length;
	double qdr;
	double cutoff;
	int resize();
public:
	Component(string substance, double refEnergy, double refLength, double refMass);
	int gnumberComponents() const;
	int snumberComponents(int ersetz);
	int gnumberLJCenters() const;
	int snumberLJCenters(int ersetz);
	int gnumberCharges() const;
	int snumberCharges(int ersetz);
	int gnumberQuadrupoles() const;
	int snumberQuadrupoles(int ersetz);
	int gnumberdipoles() const;
	int snumberdipoles(int ersetz);
	int gnumberTersoff() const;
	int snumberTersoff(int ersetz);
	double gMass() const;
	double gIxx() const;
	double gIyy() const;
	double gIzz() const;
	vector<double> gComponent() const;
	unsigned size();
	double operator[](int index);
	friend ostream& operator<< (ostream& os, Component& comp);
};

#endif /* COMPONENT_H_ */
