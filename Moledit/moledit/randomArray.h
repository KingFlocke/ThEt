/*
 * randomArray.h
 *
 *  Created on: 04.05.2011
 *      Author: eck
 */

#ifndef RANDOMARRAY_H_
#define RANDOMARRAY_H_

#include <vector>

class eSizeSmallerNumbertrue{};

class RandomArray{
private:
	vector<bool> vBool;
public:
	RandomArray(int size, int numberTrue); //constructor
	bool gArray(unsigned i) const;
	bool operator[](int index);
};


#endif /* RANDOMARRAY_H_ */
