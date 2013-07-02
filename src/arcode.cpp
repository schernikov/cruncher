/*
 * arcode.cpp
 *
 *  Created on: Jun 25, 2013
 *      Author: schernikov
 */

#include <cstring>

#include "arcode.h"

Array::Array(std::string name, long count, const double* values){
	this->name = name;
	this->count = count;
	vals = new double[count];
	memcpy((void*)vals, values, sizeof(*values)*count);
}

Array::~Array(){
	if(vals) delete vals;
}
