/*
 * underparser.h
 *
 *  Created on: Jun 10, 2013
 *      Author: schernikov
 */

#include "parser/parser.h"

#ifndef UNDEFPARSER_H_
#define UNDEFPARSER_H_

struct UndefParser : public Parser {
	UndefParser(std::string& nm);
	void on_line(std::string& line);
};


#endif /* UNDEFPARSER_H_ */
