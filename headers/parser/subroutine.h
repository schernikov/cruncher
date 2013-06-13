/*
 * subroutine.h
 *
 *  Created on: Jun 12, 2013
 *      Author: schernikov
 */

#ifndef SUBROUTINE_H_
#define SUBROUTINE_H_

#include "header.h"

struct SubroutineHeader : public Header {
	SubroutineHeader(System& sys, const std::string& nm) : Header(sys, nm) {};
	~SubroutineHeader() {};

	void on_line(std::string& line);
};


#endif /* SUBROUTINE_H_ */
