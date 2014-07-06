/*
 * molecule.cpp
 *
 *  Created on: 10.11.2010
 *      Author: eck
 */

using namespace std;

#include "molecule.h"

int Molecule::gIdNumber() const{
	return idNumber;
}

int Molecule::sIdNumber(int input){
	idNumber = input;
	return 0;
}

int Molecule::gComponentType() const{
	return componentType;
}

int Molecule::sComponentType(int input){
	componentType = input;
	return 0;
}

double* Molecule::gPosition(){
	return position;
}

int Molecule::sPosition(double input[3]){
	for (int i=0; i<=2; i++)
		position[i] = input[i];
	return 0;
}

double* Molecule::gVelocity(){
	return velocity;
}

int Molecule::sVelocity(double input[3]){
	for (int i=0; i<=2; i++)
		velocity[i] = input[i];
	return 0;
}

Quaternion Molecule::gOrientationQuaternion() const{
	return orientationQuaternion;
}

int Molecule::sOrientationQuaternion(Quaternion quat){
	orientationQuaternion = quat;
	return 0;
}

int Molecule::sOrientationQuaternion(double input[4]){
	orientationQuaternion.sCoordinates(input);
	return 0;
}

double* Molecule::gAngularMom(){
	return angularMom;
}

int Molecule::sAngularMom(double input[3]){
	for (int i=0; i<=2; i++)
		angularMom[i] = input[i];
	return 0;
}

//überladener Ausgabeoperator
ostream& operator<< ( ostream& os, Molecule& rhs){
	os.precision(8);
	//Quaternion orientationQuaternion = rhs.gOrientationQuaternion();
	os << rhs.gIdNumber() << " " << rhs.gComponentType() << "\t" << rhs.gPosition()[0] << " " <<
			rhs.gPosition()[1] << " " << rhs.gPosition()[2] << "\t" << rhs.gVelocity()[0] << " " <<
			rhs.gVelocity()[1] << " " << rhs.gVelocity()[2] << "\t" <<
			rhs.gOrientationQuaternion().gCoordinates()[0] << " " <<
			rhs.gOrientationQuaternion().gCoordinates()[1] << " " <<
			rhs.gOrientationQuaternion().gCoordinates()[2] << " " <<
			rhs.gOrientationQuaternion().gCoordinates()[3] << " " << rhs.gAngularMom()[0] << " " <<
			rhs.gAngularMom()[1] << " " << rhs.gAngularMom()[2] << endl;
	return os;
}
