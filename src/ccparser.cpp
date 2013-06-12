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

#include "ccparser.h"

#define MAX_NAME_LENGTH 256

using namespace std;

static const string headerline("HEADER");
static const string headernodeline("HEADER NODE DATA,");
static const string headercondline("HEADER CONDUCTOR DATA,");
static const string headervariableline("HEADER VARIABLES 0,");
static const string headersubline("HEADER SUBROUTINE");
static const string headeroptionsline("HEADER OPTIONS");

struct OptionsHeader : public Header {
	string modname;

	OptionsHeader(const string& nm) : modname(nm) {};
	~OptionsHeader() {};

	void on_line(string& line);
};

void OptionsHeader::on_line(string& line){
	throw CruncherException("Unexpected options line: "+line);
}

struct NodeHeader : public Header {
	string modname;

	NodeHeader(const string& nm) : modname(nm) {};
	~NodeHeader() {};

	void on_line(string& line);
};

void NodeHeader::on_line(string& line){
	long node;
	double temp, cap;

	/* 1,    20.,    128.2271 */
	if(sscanf(line.c_str(), "%ld,%lf,%lf\r", &node, &temp, &cap) != 3){
		throw CruncherException("node parse failed for "+line);
	}
}

struct SubroutineHeader : public Header {
	string modname;

	SubroutineHeader(const string& nm) : modname(nm) {};
	~SubroutineHeader() {};

	void on_line(string& line);
};

static const string subsubroutine="SUBROUTINE";
static const string subreturn="RETURN";
static const string subend="END";

void SubroutineHeader::on_line(string& line){
	string subline;
	if(line.at(0) == 'F'){
		subline = line.substr(1, string::npos);
		subline = trim(subline);
	} else {
		subline = trim(line);
	}
	if(startswith(subline, subsubroutine)){
		return;
	}
	if(startswith(subline, subreturn)){
		return;
	}
	if(startswith(subline, subend)){
		return;
	}
	throw CruncherException("Unexpected subroutine line: '"+line+"'");
}

struct ConductHeader : public Header {
	string modname;
	char format[MAX_NAME_LENGTH];

	ConductHeader(const string& nm);
	~ConductHeader() {};

	void on_line(string& line);
};

ConductHeader::ConductHeader(const string& nm) : modname(nm) {
	sprintf(format, "%sld,%s%d\[^.].%sld,%s%d\[^.].%sld,%sf", "%","%",MAX_NAME_LENGTH,"%","%",MAX_NAME_LENGTH, "%", "%");
}

void ConductHeader::on_line(string& line){
	long idx;
	char fromname[MAX_NAME_LENGTH], toname[MAX_NAME_LENGTH];
	long fromnode, tonode;
	double cap;

	/* 1,    ADCE.1,    NPANEL.13,    14.025         $ Contact */
	if(sscanf(line.c_str(), format, &idx, &fromname, &fromnode, &toname, &tonode, &cap) != 6){
		throw CruncherException("node parse failed for "+line);
	}
}


struct VariablesHeader : public Header {
	string modname;
	char format[MAX_NAME_LENGTH];
	bool had_else;
	long time_array;
	long vals_array;
	string varname;

	VariablesHeader(const string& nm);
	~VariablesHeader() {
		checkvars();
	};

	void on_line(string& line);
	void checkvars();
};

VariablesHeader::VariablesHeader(const string& nm) : modname(nm) {
	sprintf(format, "%s%d\[^.].Q%sld", "%",MAX_NAME_LENGTH,"%");
	had_else = false;
	time_array = 0;
	vals_array = 0;
	//cout << nm << " variables" << endl;
};

static string varcond("IF( NSOL .GT. 1 .OR. DTIMES .GT. 0. ) THEN");
static string varcall("CALL DA11MC(");
static string varelse("ELSE");
static string varend("ENDIF");

static long getarray(string& arg){
	long num;
	if(sscanf(arg.c_str(), "A%ld", &num) != 1){
		throw CruncherException("Expected A<num> format "+arg);
	}
	return num;
}

void VariablesHeader::checkvars(){
	if(!varname.empty()){
		cout << "   " << modname << ": time call " << varname << "["<< time_array << "," << vals_array << "]" << endl;
		varname.clear();
	}
}

void VariablesHeader::on_line(string& line){
	size_t epos = line.find("=");
	if(epos == string::npos){
		/* ignore certain statements */
		string subline = trim(line);
		if(startswith(subline, varcond)){
			had_else = false;
			return;
		}
		if(startswith(subline, varcall)){
			checkvars();

			size_t pos = subline.find(')', varcall.size());
			if(pos == string::npos){
				throw CruncherException("Expected closing bracket"+line);
			}
			string args =subline.substr(varcall.size(), pos-varcall.size());
			stringstream ss(args);
			string arg;
			pos = 0;
			while (std::getline(ss, arg, ',')) {
				pos += 1;
				if(pos == 3){
					time_array = getarray(arg);
				} else if (pos == 4) {
					vals_array = getarray(arg);
				} else if (pos == 6){
					varname = arg;
				}
			}
			return;
		}
		if(startswith(subline, varelse)){
			had_else = true;
			return;
		}
		if(startswith(subline, varend)){
			had_else = false;
			return;
		}
		throw CruncherException("Equation is expected "+line);
	}
	if(had_else){
		/* ignore equations after ELSE */
		return;
	}
	string in_node = line.substr(0, epos);
	in_node = trim(in_node);
	size_t spos = line.find("+", epos+1);
	if(spos == string::npos || spos <= epos){
		throw CruncherException("Sum is expected "+line);
	}
	string out_node = line.substr(epos+1, spos-epos-1);
	out_node = trim(out_node);
	if(in_node.compare(out_node) != 0){
		throw CruncherException("increment is expected "+line);
	}
	char modname[MAX_NAME_LENGTH];
	long nodenum;
	if(sscanf(line.c_str(), format, &modname, &nodenum) != 2){
		throw CruncherException("equation node parse failed for "+line);
	}
	string sum = line.substr(spos+1, string::npos);
	sum = trim(sum);
	stringstream ss(sum);
	string item;
	string vn;
	while (std::getline(ss, item, '*')) {
		double num;
		if(sscanf(item.c_str(), "%lf", &num) != 1){
			if(!vn.empty()){
				throw CruncherException("Too many variables "+line);
			}
			vn = trim(item);
			if(vn.compare(varname) != 0){
				throw CruncherException("Unexpected variable name '"+vn+"', need '"+varname+"'");
			}
		}
	}
}

SindaParser::SindaParser(std::string& nm) : Parser(nm), header(0){

}

void SindaParser::on_line(std::string& line) {
	if(startswith(line, headerline)) {
		if(header) {
			delete header;
			header = 0;
		}
		if(startswith(line, headernodeline)){
			string modname = selectafter(line, headernodeline);
			header = new NodeHeader(modname);
		} else if(startswith(line, headercondline)) {
			string modname = selectafter(line, headercondline);
			header = new ConductHeader(modname);
		} else if (startswith(line, headervariableline)){
			string modname = selectafter(line, headervariableline);
			header = new VariablesHeader(modname);
		} else if (startswith(line, headersubline)){
			string modname = selectafter(line, headersubline);
			header = new SubroutineHeader(modname);
		} else if (startswith(line, headeroptionsline)){
			string modname = selectafter(line, headeroptionsline);
			header = new OptionsHeader(modname);
		} else {
			cout << "  " << line << endl;
		}
	} else {
		if(header) header->on_line(line);
	}
}

SindaParser::~SindaParser() {
	//cout << "destroying " << name() << endl;
}
