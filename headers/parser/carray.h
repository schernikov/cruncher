/*
 * carray.h
 *
 *  Created on: Jun 13, 2013
 *      Author: schernikov
 */

#ifndef CARRAY_H_
#define CARRAY_H_

#include "basearray.h"

struct CarrayHeader : public BaseArrayHeader {
	long sequence;
	std::string name;
	long vcount;
	long* values;
	long maxcount;

	CarrayHeader(System& sys, const std::string& nm);
	~CarrayHeader();

	void on_line(std::string& line);
	void add_value(long val);
	void start(std::string& nm);
	void stop();
	void clean();
	void open(long num);
	void close();
};

#endif /* CARRAY_H_ */
