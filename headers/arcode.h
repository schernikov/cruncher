/*
 * arcode.h
 *
 *  Created on: Jun 25, 2013
 *      Author: schernikov
 */

#ifndef ARCODE_H_
#define ARCODE_H_

#include <string>
#include <cstring>

struct Array {
	std::string name;
	size_t count;
	const double* vals;

	Array(std::string name, long count, const double* values);
	~Array();
};


#endif /* ARCODE_H_ */
