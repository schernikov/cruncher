/*
 * header.h
 *
 *  Created on: Jun 12, 2013
 *      Author: schernikov
 */

#ifndef HEADER_H_
#define HEADER_H_

#include <string>

#include "common.h"
#include "system.h"

struct Header {
	std::string modname;
	System& system;

	Header(System& sys, const std::string& mod) : modname(mod), system(sys) {};
	virtual void on_line(std::string& line) = 0;
	virtual ~Header() {};
};

#endif /* HEADER_H_ */
