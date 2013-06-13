/*
 * variables.h
 *
 *  Created on: Jun 12, 2013
 *      Author: schernikov
 */

#ifndef VARIABLES_H_
#define VARIABLES_H_

#include "header.h"

enum VS_Types {
	VS_Unknown,
	VS_Temperature,
	VS_Energy,
};

struct VarSpec {
	VS_Types type;
	std::string mod;
	long node;
	VarSpec (VS_Types tp, std::string md, long nd) : type(tp), mod(md), node(nd) {};
};

struct ArraySpec {
	std::string arrayname;
	long time;
	long vals;
	std::string varname;

	void set(const std::string& mod, long tm, long val, const std::string& var);
	bool empty();
	void clear();
	std::string str();
};

struct SunMode {
	bool open;
	bool closed;
	double offset;

	SunMode() : open(false), closed(false), offset(0) {};
	void on_offset(const std::string& nm);
	bool is_on();
};

struct VariablesHeader : public Header {
	char format[MAX_NAME_LENGTH];
	bool had_else;
	ArraySpec current_array;

	SunMode sun_mode;

	VariablesHeader(System& sys, const std::string& nm);
	~VariablesHeader();

	void on_line(std::string& line);
	void set_from_array(const VarSpec& vspec, const ArraySpec& array);
	VarSpec on_varname(const std::string& vn);
	void on_callargs(std::string& subline, const std::string& call, long tmidx, long valsidx, long varidx);
	void on_sunargs(std::string& subline, const std::string& call);
	void assign(const std::string& line, size_t epos);
	void on_equation(const std::string& line, size_t epos);
	void on_sum(const std::string& line, size_t epos, size_t spos);
	void checkvars();
};


#endif /* VARIABLES_H_ */
