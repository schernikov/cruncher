/*
 * arcode.cpp
 *
 *  Created on: Jun 25, 2013
 *      Author: schernikov
 */

#include <cstring>

#include "common.h"
#include "elements.h"
#include "arcode.h"

Array::Array(std::string& name, const char* mod, long idx, long count, const double* values){
	this->name = name;
	this->mod = mod;
	this->idx = idx;
	this->count = count;
	vals = new double[count];
	memcpy((void*)vals, values, sizeof(*values)*count);
}

Array::~Array(){
	if(vals) delete vals;
}

void Array::replace(long count, const double* values){
	delete this->vals;
	this->count = count;
	this->vals = values;
}

static void checkarsizes(const Array* timearr, const Array* valsarr){
	if(valsarr->count != timearr->count){
		throw CruncherException("Array sizes don't match: %s.%ld[%ld] != %s.%ld[%ld]",
				timearr->mod.c_str(), timearr->idx, timearr->count,
				valsarr->mod.c_str(), valsarr->idx, valsarr->count);
	}
}

void ArraySet::add(Node& node, double mult, const Array* times, const Array* vals) {
	checkarsizes(times, vals);

	NodeValues* nv = &values[&node];
	nv->times = times;
	nv->vals = vals;
	nv->mult = mult;
}
