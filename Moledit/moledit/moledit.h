/*
 * moledit.h
 *
 *  Created on: 15.03.2011
 *      Author: eck
 */

#ifndef MOLEDIT_H_
#define MOLEDIT_H_

#include "system.h"
#include "component.h"

int changeComponentType(System &system, int numberNewTypes, int possibilityPercentKeep, Component comp, double eta, double xi);
int rotateSystem(System &system, int rotateDirection);
System mergeSystems(System system1, System system2, int flag);
System readRawSystem(double sizeBox[3], char* posInFilename, char* velInFilename, double refEnergy, double refLength, double refMass, double temperature);


#endif /* MOLEDIT_H_ */
