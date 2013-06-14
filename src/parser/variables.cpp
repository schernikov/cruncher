/*
 * variables.cpp
 *
 *  Created on: Jun 12, 2013
 *      Author: schernikov
 */

#include <iostream>
#include <sstream>
#include <cstdio>

#include "parser/variables.h"

static std::string varcond("IF( NSOL .GT. 1 .OR. DTIMES .GT. 0. ) THEN");
static std::string qcall("CALL DA11MC(");
static std::string tcall("CALL D1DEG1 (");
static std::string lcall("CALL LOADQ(");
static std::string varelse("ELSE");
static std::string varend("ENDIF");
static std::string timem("TIMEM");

void ArraySpec::set(const std::string& mod, long tm, long val, const std::string& var){
	arrayname = mod;
	time = tm;
	vals = val;
	varname = var;
}

bool ArraySpec::empty(){
	return varname.empty();
}

void ArraySpec::clear(){
	varname.clear();
}

std::string ArraySpec::str(){
	std::stringstream ss;
	ss << arrayname << "["<< time << "," << vals << "]->" << varname;
	return ss.str();
}

VariablesHeader::VariablesHeader(System& sys, const std::string& nm) : Header(sys, nm) {
	sprintf(format, "%s%d\[^.].Q%sld", "%",MAX_NAME_LENGTH,"%");
	had_else = false;
};

VariablesHeader::~VariablesHeader() {
	checkvars();
};

static long getarray(std::string& arg){
	long num;
	std::string var = trim(arg);
	if(sscanf(var.c_str(), "A%ld", &num) != 1){
		throw CruncherException("Expected A<num> format "+arg);
	}
	return num;
}

void VariablesHeader::checkvars(){
	if(!current_array.empty()){
		current_array.clear();
	}
}

std::string get_args(std::string& subline, const std::string& call){
	size_t pos = subline.find(')', call.size());
	if(pos == std::string::npos){
		throw CruncherException("Expected closing bracket: "+subline);
	}
	return subline.substr(call.size(), pos-call.size());
}

void VariablesHeader::on_callargs(std::string& subline, const std::string& call, long tmidx, long valsidx, long varidx){
	checkvars();

	std::stringstream ss(get_args(subline, call));
	std::string arg;
	size_t pos = 0;
	long time, vals;
	std::string name;
	while (std::getline(ss, arg, ',')) {
		pos += 1;
		if(pos == (size_t)tmidx){
			time = getarray(arg);
		}
		if (pos == (size_t)valsidx) {
			vals = getarray(arg);
		}
		if (pos == (size_t)varidx){
			name = trim(arg);
		}
	}
	if(name.empty()){
		throw CruncherException("Can not find array and variable spec: "+subline);
	}
	current_array.set(modname, time, vals, name);
	VarSpec vspec = on_varname(name);
	if(vspec.node == 0){
		/* this is temporary variable; ignore */
		return;
	}
	set_from_array(vspec, current_array);
}

void VariablesHeader::on_sunargs(std::string& subline, const std::string& call){
	std::stringstream ss(get_args(subline, call));
	std::string arg;
	size_t pos = 0;
	long fnum, tnum;
	long arrays[5];
	std::string name;
	while (std::getline(ss, arg, ',')) {
		pos += 1;
		if(pos == 1){
			name = trim(arg);
			if(name.at(0) == '\'' && name.at(name.size()-1) == '\''){
				name = name.substr(1, name.size()-2);
			}
		} else if (pos == 2) {
			if(sscanf(arg.c_str(), "%ld", &fnum) != 1){
				throw CruncherException("Expected number as argument 2 in '"+subline+"'");
			}
		} else if (pos == 3) {
			if(sscanf(arg.c_str(), "%ld", &tnum) != 1){
				throw CruncherException("Expected number as argument 3 in '"+subline+"'");
			}
		} else if (pos >= 4 && pos <= 8) {
			arrays[pos-4] = getarray(arg);
		} else {
			throw CruncherException("Too many arguments in "+subline);
		}
	}
	system.on_sun(sun_mode.offset, name.c_str(), fnum, tnum, sizeof(arrays)/sizeof(arrays[0]), arrays);
}

void VariablesHeader::set_from_array(const VarSpec& vspec, const ArraySpec& array){
	switch(vspec.type){
	case VS_Energy:
		if(array.time == array.vals){
			std::stringstream ss;
			ss << "Energy array expected to be a single one, got: " << array.time;
			throw CruncherException(ss.str());
		}
		system.on_energy(vspec.mod.c_str(), vspec.node, array.arrayname.c_str(), array.time, array.vals);
		break;
	case VS_Temperature:
		if(array.time != array.vals){
			std::stringstream ss;
			ss << "Temperature array expected to be a double one, got: " << array.time << "," << array.vals;
			throw CruncherException(ss.str());
		}
		system.on_temp_array(vspec.mod.c_str(), vspec.node, array.arrayname.c_str(), array.time);
		break;
	default:
		std::stringstream ss;
		ss << "Unexpected variable type: " << vspec.type << " "<< vspec.mod << "." << vspec.node;
		throw CruncherException(ss.str());
	}
}

void VariablesHeader::on_line(std::string& line){
	size_t epos = line.find("=");
	if(epos == std::string::npos){
		/* ignore certain statements */
		std::string subline = trim(line);
		if(startswith(subline, varcond)){
			had_else = false;
			return;
		}
		if(startswith(subline, qcall)){
			on_callargs(subline, qcall, 3, 4, 6);
			return;
		}
		if(startswith(subline, tcall)){
			on_callargs(subline, tcall, 2, 2, 3);
			return;
		}
		if(startswith(subline, lcall)){
			if(!sun_mode.is_on()){
				throw CruncherException("Got '"+line+"'. Sun offset was expected.");
			}
			on_sunargs(subline, lcall);
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
	on_equation(line, epos);
}

VarSpec VariablesHeader::on_varname(const std::string& vn){
	/* possible name options:
	 *   Q<temp_name>
	 *   Q<num>
	 *   T<num>
	 *   <mod>.Q<num>
	 *   <mod>.T<num>
	 **/
	std::string mod, nd;
	size_t pos = vn.find(".");
	if(pos == std::string::npos){
		mod = modname;
		nd = vn;
	} else {
		mod = vn.substr(0, pos);
		nd = vn.substr(pos+1, vn.size()-pos-1);
	}
	mod = trim(mod);
	nd = trim(nd);
	bool energy;
	if(nd.at(0) == 'Q'){
		energy = true;
	} else if (nd.at(0) == 'T') {
		energy = false;
	} else {
		throw CruncherException("Unexpected varname: "+vn);
	}
	nd = nd.substr(1, nd.size()-1);
	long num;
	if(sscanf(nd.c_str(), "%ld", &num) != 1){
		num = 0;
	}
	return VarSpec(energy?VS_Energy:VS_Temperature, mod, num);
}

void VariablesHeader::assign(const std::string& line, size_t epos){
	std::string in_node = line.substr(0, epos);
	in_node = trim(in_node);
	VarSpec tspec = on_varname(in_node);
	std::string out_node = line.substr(epos+1, std::string::npos);
	out_node = trim(out_node);
	VarSpec fspec = on_varname(out_node);
	if(fspec.type != tspec.type){
		throw CruncherException("Wrong variable types: "+line);
	}
	if(!current_array.empty() && current_array.varname.compare(out_node) == 0){
		/* that's essentially array assignment */
		set_from_array(tspec, current_array);
	} else {
		switch(tspec.type){
		case VS_Energy:
			system.on_energy_assign(fspec.mod.c_str(), fspec.node, tspec.mod.c_str(), tspec.node);
			break;
		case VS_Temperature:
			system.on_temp_assign(fspec.mod.c_str(), fspec.node, tspec.mod.c_str(), tspec.node);
			break;
		default:
			std::stringstream ss;
			ss << "Unexpected variable type: " << tspec.type << " "<< tspec.mod << "." << tspec.node;
			throw CruncherException(ss.str());
		}
	}
}

void VariablesHeader::on_equation(const std::string& line, size_t epos){
	size_t spos = line.find("+", epos+1);
	if(spos == std::string::npos){
		spos = line.find("-", epos+1);
		if(spos == std::string::npos){
			assign(line, epos);
		} else {
			on_sum(line, epos, spos-1);  // need to pickup '-' (minus) sign as part of addition
		}
	} else {
		on_sum(line, epos, spos);
	}
}
void VariablesHeader::on_sum(const std::string& line, size_t epos, size_t spos){
	std::string in_node = line.substr(0, epos);
	in_node = trim(in_node);
	std::string out_node = line.substr(epos+1, spos-epos-1);
	out_node = trim(out_node);
	if(in_node.compare(out_node) != 0){
		throw CruncherException("increment is expected "+line);
	}
	std::string sum = line.substr(spos+1, std::string::npos);
	sum = trim(sum);

	char mname[MAX_NAME_LENGTH];
	long nodenum;
	if(sscanf(in_node.c_str(), format, &mname, &nodenum) != 2){
		if(in_node.compare(timem) == 0){
			sun_mode.on_offset(sum);
			return;
		}
		throw CruncherException("equation node parse failed for "+line);
	}
	size_t starpos = sum.find("*");
	if(starpos != std::string::npos){
		std::stringstream ss(sum);
		std::string item;
		std::string vn;
		double mult = 1;
		while (std::getline(ss, item, '*')) {
			double num;
			if(sscanf(item.c_str(), "%lf", &num) != 1){
				if(!vn.empty()){
					throw CruncherException("Too many variables "+line);
				}
				vn = trim(item);
				if(vn.compare(current_array.varname) != 0){
					throw CruncherException("Unexpected variable name '"+vn+"', need '"+current_array.varname+"'");
				}
			} else {
				mult *= num;
			}
		}
		system.on_energy(mname, nodenum, mult, current_array.arrayname.c_str(), current_array.time, current_array.vals);
	} else {
		double num;
		if(sscanf(sum.c_str(), "%lf", &num) != 1){
			throw CruncherException("Expected float number: "+line);
		}
		system.on_energy(mname, nodenum, num);
	}
}

bool SunMode::is_on(){
	return open && !closed;
}

void SunMode::on_offset(const std::string& sum){
	if(!open){
		double off;
		if(sscanf(sum.c_str(), "%lf", &off) != 1){
			throw CruncherException("Expected start offset instead of: "+sum);
		}
		offset = off;
		open = true;
		return;
	}
	if(closed){
		throw CruncherException("Unexpected offset. Already had complete sun block.");
	}
	double off;
	if(sscanf(sum.c_str(), "%lf", &off) != 1){
		throw CruncherException("Expected end offset instead of: "+sum);
	}
	if(off != (-offset)){
		throw CruncherException("Start and end offset expected to be equal: "+sum);
	}
	closed = true;
}
