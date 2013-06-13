/*
 * parser.h
 *
 *  Created on: Jun 10, 2013
 *      Author: schernikov
 */

#ifndef PARSER_H_
#define PARSER_H_

#include <string>

struct Parser {
private:
	std::string _name;
public:
	Parser(std::string& nm) : _name(nm) {};
	const std::string& name() { return _name; };
	virtual void on_line(std::string& line) = 0;
	virtual ~Parser() {};
};

#include "common.h"

#endif /* PARSER_H_ */
