/*
 * vars_1.cpp
 *
 *  Created on: Jun 22, 2013
 *      Author: schernikov
 */

#include <cstdio>
#include <sstream>
#include <iostream>

#include "parser/vars_1.h"

Vars1Header::Vars1Header(System& sys, const std::string& nm) : Header(sys, nm) {
	if(nm.compare("SENSOR") != 0){
		std::stringstream ss;
		ss << "Variable 1 section: '" << nm << "'";
		throw CruncherException(ss.str());
	}
	sprintf(format, "%s%d\[^.].T%sld", "%",MAX_NAME_LENGTH,"%");
};

Vars1Header::~Vars1Header() {
	on_close();
};

void Vars1Header::on_line(std::string& line){
	std::string tr = trim(line);
	if(startswith(tr, "&")){
		std::string sub = trim(tr.substr(1, std::string::npos));
		on_add(sub);
		return;
	}
	on_close();
	on_open(tr);
}

void Vars1Header::on_open(std::string& line){
	equation = line;
}

void Vars1Header::on_add(std::string& line){
	equation += line;
}

void Vars1Header::on_close(){
	if(equation.empty()) return;
	size_t epos = equation.find("=");
	if(epos == std::string::npos){
		std::stringstream ss;
		ss << "Variable 1 section; Expected = sign, got: '" << equation << "'";
		throw CruncherException(ss.str());
	}
	std::string in_node = equation.substr(0, epos);

	char mname[MAX_NAME_LENGTH];
	long nodenum;
	if(sscanf(in_node.c_str(), format, &mname, &nodenum) != 2){
		throw CruncherException("equation node parse failed for "+equation);
	}

	std::stringstream ss(equation.substr(epos+1, std::string::npos));
	std::string arg;
	long idx = 0;
	std::string name;
	while (std::getline(ss, arg, '+')) {
		std::string mult = trim(arg);
		size_t pos = mult.find("*");
		if(pos == std::string::npos){
			std::stringstream ss;
			ss << "Variable 1 section; Expected multiplication, got: '" << mult << "'";
			throw CruncherException(ss.str());
		}
		std::string co = mult.substr(0, pos);
		double coef;
		if(sscanf(co.c_str(), "%lf", &coef) != 1){
			throw CruncherException("coefficient parse failed for "+mult);
		}
		std::string out_node = mult.substr(pos+1, std::string::npos);
		char nm[MAX_NAME_LENGTH];
		long num;
		if(sscanf(out_node.c_str(), format, &nm, &num) != 2){
			throw CruncherException("equation node parse failed for "+equation);
		}
		system.on_approx(mname, nodenum, idx, coef, nm, num);
		idx += 1;
	}
	equation.clear();
}
