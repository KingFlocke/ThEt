/*
 * molecule.h
 *
 *  Created on: 10.11.2010
 *      Author: eck
 */

#ifndef MOLECULE_H_
#define MOLECULE_H_

#include <ostream>
#include "quaternion.h"

class Molecule{
private:
	int idNumber;
	int componentType;
	double position[3];
	double velocity[3];
	Quaternion orientationQuaternion; //Orientierung in Quaternionen (q0, q1, q2, q3) mit q = q0+q1*i+q2*j+q3*k
	double angularMom[3]; //Drehimpuls (Dx, Dy, Dz)
public:
	int gIdNumber() const;
	int sIdNumber(int input);
	int gComponentType() const;
	int sComponentType(int input);
	double* gPosition();
	int sPosition(double input[3]);
	double* gVelocity();
	int sVelocity(double input[3]);
	Quaternion gOrientationQuaternion() const;
	int sOrientationQuaternion(Quaternion quat);
	int sOrientationQuaternion(double input[4]);
	double* gAngularMom();
	int sAngularMom(double input[3]);
	//überladener Ausgabeoperator
	friend ostream& operator<< (ostream& os, Molecule& rhs);
};

#endif /* MOLECULE_H_ */
