/*
 * array.h
 *
 *  Created on: Jun 13, 2013
 *      Author: schernikov
 */

#ifndef ARRAY_H_
#define ARRAY_H_

#include "basearray.h"

struct ArrayHeader : public BaseArrayHeader {
	long curr_array;
	long vcount;
	double* values;
	long maxcount;

	ArrayHeader(System& sys, const std::string& nm);
	~ArrayHeader();

	void on_line(std::string& line);
	void add_value(double val);
	void clean();
	void open(long num);
	void close();
};

#endif /* ARRAY_H_ */
