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
#include <boost/unordered_map.hpp>

struct Array {
	std::string name;
	std::string mod;
	long idx;
	size_t count;
	const double* vals;

	Array(std::string& name, const char* mod, long idx, long count, const double* values);
	~Array();

	void replace(long count, const double* values);
};

struct NodeValues {
	const Array* times;
	const Array* vals;
	double mult;
};

typedef boost::unordered_map<Node*, NodeValues> ValuesMap;

struct ArraySet {
	ValuesMap values;

	void add(Node& node, double mult, const Array* times, const Array* vals);
};

#endif /* ARCODE_H_ */
