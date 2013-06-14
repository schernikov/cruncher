/*
 * control.h
 *
 *  Created on: Jun 13, 2013
 *      Author: schernikov
 */

#ifndef CONTROL_H_
#define CONTROL_H_

#include "header.h"

struct ControlHeader : public Header {
	ControlHeader(System& sys, const std::string& nm) : Header(sys, nm) {};
	~ControlHeader() {};

	void on_line(std::string& line);
};

#endif /* CONTROL_H_ */
