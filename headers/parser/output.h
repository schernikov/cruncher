/*
 * output.h
 *
 *  Created on: Jun 13, 2013
 *      Author: schernikov
 */

#ifndef OUTPUT_H_
#define OUTPUT_H_

#include "header.h"

struct OutputHeader : public Header {
	OutputHeader(System& sys, const std::string& nm) : Header(sys, nm) {};
	~OutputHeader() {};

	void on_line(std::string& line);
};


#endif /* OUTPUT_H_ */
