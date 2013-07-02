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
#include "parser/operation.h"
#include "parser/output.h"
#include "parser/control.h"
#include "parser/array.h"
#include "parser/carray.h"
#include "parser/vars_1.h"

using namespace std;

static const string headerline("HEADER");
static const string headernodeline("HEADER NODE DATA,");
static const string headercondline("HEADER CONDUCTOR DATA,");
static const string headervariableline("HEADER VARIABLES 0,");
static const string headerarrayline("HEADER ARRAY DATA,");
static const string headerchararrayline("HEADER CARRAY DATA,");
static const string headersubline("HEADER SUBROUTINE");
static const string headeroptionsline("HEADER OPTIONS");
static const string headeroperline("HEADER OPERATION DATA");
static const string headeroutputline("HEADER OUTPUT CALLS");
static const string headercontrolline("HEADER CONTROL DATA");
static const string headervars1line("HEADER VARIABLES 1,");

SindaParser::SindaParser(System& sys, std::string& nm) : Parser(nm), system(sys), header(0) {

}
void SindaParser::close_header(){
	if(header) {
		delete header;
		header = 0;
	}
}

void SindaParser::on_line(std::string& line) {
	if(startswith(line, headerline)) {
		close_header();

		if(startswith(line, headernodeline)){
			string modname = selectafter(line, headernodeline);
			header = new NodeHeader(system, modname);
		} else if(startswith(line, headercondline)) {
			string modname = selectafter(line, headercondline);
			header = new ConductHeader(system, modname);
		} else if (startswith(line, headervariableline)){
			string modname = selectafter(line, headervariableline);
			header = new VariablesHeader(system, modname);
		} else if (startswith(line, headervars1line)){
			string modname = selectafter(line, headervars1line);
			header = new Vars1Header(system, modname);
		} else if (startswith(line, headersubline)){
			string modname = selectafter(line, headersubline);
			header = new SubroutineHeader(system, modname);
		} else if (startswith(line, headeroptionsline)){
			string modname = selectafter(line, headeroptionsline);
			header = new OptionsHeader(system, modname);
		} else if (startswith(line, headeroperline)){
			header = new OperationHeader(system, "");
		} else if (startswith(line, headeroutputline)){
			header = new OutputHeader(system, "");
		} else if (startswith(line, headercontrolline)){
			header = new ControlHeader(system, "");
		} else if (startswith(line, headerarrayline)){
			string modname = selectafter(line, headerarrayline);
			header = new ArrayHeader(system, modname);
		} else if (startswith(line, headerchararrayline)){
			string modname = selectafter(line, headerchararrayline);
			header = new CarrayHeader(system, modname);
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
	close_header();
}
