/*
 * quaternion.h
 *
 *  Created on: 04.04.2011
 *      Author: eck
 */

#ifndef QUATERNION_H_
#define QUATERNION_H_

class Quaternion{
private:
	double coordinates[4];
public:
	Quaternion();
	Quaternion(double angle, double direction[3]);
	Quaternion(double q[4]);
	double* gCoordinates();
	int sCoordinates(double input[4]);
	//Elementfunktion: Multiplizieren mit einem anderen Quaternion
	Quaternion operator*(Quaternion quat1); //quat1 * *this; drehen
};

#endif /* QUATERNION_H_ */
