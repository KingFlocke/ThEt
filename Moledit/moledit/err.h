/*
 * exception.h
 *
 *  Created on: 16.02.2012
 *      Author: eck
 */


#ifndef ERR_H_
#define ERR_H_

#include <string>

using namespace std;

class Err{
private:
	string message;
public:
	Err(string mes);
	string getMessage();
};

#endif /* ERR_H_ */
