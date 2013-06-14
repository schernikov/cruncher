/*
 * control.cpp
 *
 *  Created on: Jun 13, 2013
 *      Author: schernikov
 */

#include <cstdio>

#include "parser/control.h"

void ControlHeader::on_line(std::string& line){
	size_t epos = line.find("=");
	if(epos == std::string::npos){
		throw CruncherException("Equation is expected "+line);
	}
	std::string name = line.substr(0, epos);
	name = trim(name);

	std::string val = line.substr(epos+1, std::string::npos);
	size_t cpos = val.find("$");
	if(cpos != std::string::npos){
		val = val.substr(0, cpos);
	}
	val = trim(val);
	double num;
	if(sscanf(val.c_str(), "%lf", &num) != 1){
		if(name.compare("UID") == 0 && val.compare("SI") == 0){
			return;
		}
		throw CruncherException("Expected floating point value: "+line);
	}
	system.on_variable(name.c_str(), num);
}
