/*
 * conductor.h
 *
 *  Created on: Jun 12, 2013
 *      Author: schernikov
 */

#ifndef CONDUCTOR_H_
#define CONDUCTOR_H_

#include "header.h"

struct ConductHeader : public Header {
	char format[MAX_NAME_LENGTH];

	ConductHeader(System& sys, const std::string& nm);
	~ConductHeader() {};

	void on_line(std::string& line);
};

#endif /* CONDUCTOR_H_ */
