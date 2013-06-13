/*
 * ccparser.cpp
 *
 *  Created on: Jun 10, 2013
 *      Author: schernikov
 */

#include <iostream>
#include <sstream>
#include <cstdio>
#include <string>

#include "parser/sindaparser.h"
#include "parser/options.h"
#include "parser/node.h"
#include "parser/subroutine.h"
#include "parser/conductor.h"
#include "parser/variables.h"

using namespace std;

static const string headerline("HEADER");
static const string headernodeline("HEADER NODE DATA,");
static const string headercondline("HEADER CONDUCTOR DATA,");
static const string headervariableline("HEADER VARIABLES 0,");
static const string headersubline("HEADER SUBROUTINE");
static const string headeroptionsline("HEADER OPTIONS");

SindaParser::SindaParser(System& sys, std::string& nm) : Parser(nm), system(sys), header(0) {

}

void SindaParser::on_line(std::string& line) {
	if(startswith(line, headerline)) {
		if(header) {
			delete header;
			header = 0;
		}
		if(startswith(line, headernodeline)){
			string modname = selectafter(line, headernodeline);
			header = new NodeHeader(system, modname);
		} else if(startswith(line, headercondline)) {
			string modname = selectafter(line, headercondline);
			header = new ConductHeader(system, modname);
		} else if (startswith(line, headervariableline)){
			string modname = selectafter(line, headervariableline);
			header = new VariablesHeader(system, modname);
		} else if (startswith(line, headersubline)){
			string modname = selectafter(line, headersubline);
			header = new SubroutineHeader(system, modname);
		} else if (startswith(line, headeroptionsline)){
			string modname = selectafter(line, headeroptionsline);
			header = new OptionsHeader(system, modname);
		} else {
			cout << "  " << line << endl;
		}
	} else {
		if(line.at(0) == 'P'){
			/* ignore any 'P' records */
			return;
		}
		if(header) header->on_line(line);
	}
}

SindaParser::~SindaParser() {
	//cout << "destroying " << name() << endl;
}
