/*
 * ccparser.h
 *
 *  Created on: Jun 10, 2013
 *      Author: schernikov
 */

#include "parser.h"

#ifndef CCPARSER_H_
#define CCPARSER_H_

struct Header {
	virtual void on_line(std::string& line) = 0;
	virtual ~Header() {};
};

struct SindaParser : public Parser {
	SindaParser(std::string& nm);
	void on_line(std::string& line);
	~SindaParser();
private:
	Header* header;
};


#endif /* CCPARSER_H_ */
