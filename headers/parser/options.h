/*
 * options.h
 *
 *  Created on: Jun 12, 2013
 *      Author: schernikov
 */

#ifndef OPTIONS_H_
#define OPTIONS_H_

#include "header.h"

struct OptionsHeader : public Header {
	OptionsHeader(System& sys, const std::string& nm) : Header(sys, nm) {};
	~OptionsHeader() {};

	void on_line(std::string& line);
};


#endif /* OPTIONS_H_ */
