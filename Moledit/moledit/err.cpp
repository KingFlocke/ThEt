/*
 * error.cpp
 *
 *  Created on: 16.02.2012
 *      Author: eck
 */

#include "err.h"

Err::Err(string mes){
	message = mes;
}

string Err::getMessage(){
	return message;
}

