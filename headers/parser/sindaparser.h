/*
 * ccparser.h
 *
 *  Created on: Jun 10, 2013
 *      Author: schernikov
 */

#include "parser.h"
#include "header.h"
#include "system.h"

#ifndef SINDAPARSER_H_
#define SINDAPARSER_H_

struct SindaParser : public Parser {
	System& system;

	SindaParser(System& system, std::string& nm);
	void close_header();
	void on_line(std::string& line);
	~SindaParser();
private:
	Header* header;
};


#endif /* SINDAPARSER_H_ */
