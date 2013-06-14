/*
 * basearray.h
 *
 *  Created on: Jun 13, 2013
 *      Author: schernikov
 */

#ifndef BASEARRAY_H_
#define BASEARRAY_H_

#include "header.h"

struct BaseArrayHeader : public Header {
	std::string proc_line(const std::string& line);

	BaseArrayHeader(System& sys, const std::string& nm) : Header(sys, nm) {};
	virtual void open(long num) = 0;
	virtual void close() = 0;
};

#endif /* BASEARRAY_H_ */
