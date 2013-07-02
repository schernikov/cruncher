/*
 * vars_1.h
 *
 *  Created on: Jun 22, 2013
 *      Author: schernikov
 */

#ifndef VARS_1_H_
#define VARS_1_H_

#include "header.h"

struct Vars1Header : public Header {
	std::string equation;
	char format[MAX_NAME_LENGTH];

	Vars1Header(System& sys, const std::string& nm);
	~Vars1Header();

	void on_line(std::string& line);
	void on_open(std::string& line);
	void on_add(std::string& line);
	void on_close();
};


#endif /* VARS_1_H_ */
