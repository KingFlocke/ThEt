/*
 * quaternion.cpp
 *
 *  Created on: 04.04.2011
 *      Author: eck
 */

using namespace std;

#include "math.h"
#include "quaternion.h"
#include "constants.h"

Quaternion::Quaternion(){
	double q[4] = {1,0,0,0};
	sCoordinates(q);
}

Quaternion::Quaternion(double angle, double direction[3]){
	coordinates[0] = cos(angle/360*pi);
	double length = sqrt(direction[0]*direction[0]+direction[1]*direction[1]+direction[2]*direction[2]);
	for (int i=0; i<3; ++i){
		coordinates[i+1] = direction[i]/length*sin(angle/360*pi);
	}
}

Quaternion::Quaternion(double q[4]){
	sCoordinates(q);
}

double* Quaternion::gCoordinates(){
	return coordinates;
}

int Quaternion::sCoordinates(double input[4]){
	for (int i=0; i<4; ++i)
		coordinates[i] = input[i];
	return 0;
}

//Elementfunktion: überladener *Operator
Quaternion Quaternion::operator*(Quaternion quat1){
	double* c1 = quat1.gCoordinates();
	double* c2 = coordinates;
	double k[4];
	k[0] = c1[0]*c2[0]-c1[1]*c2[1]-c1[2]*c2[2]-c1[3]*c2[3];
	k[1] = c1[0]*c2[1]+c1[1]*c2[0]+c1[2]*c2[3]-c1[3]*c2[2];
	k[2] = c1[0]*c2[2]+c1[2]*c2[0]-c1[1]*c2[3]+c1[3]*c2[1];
	k[3] = c1[0]*c2[3]+c1[3]*c2[0]+c1[1]*c2[2]-c1[2]*c2[1];
	sCoordinates(k);
	return *this;
}
