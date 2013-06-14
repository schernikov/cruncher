/*
 * array.cpp
 *
 *  Created on: Jun 13, 2013
 *      Author: schernikov
 */

#include <cstdio>
#include <sstream>
#include <cstring>

#include "parser/array.h"

#define INIT_VALUES_COUNT 1024

ArrayHeader::ArrayHeader(System& sys, const std::string& nm) : BaseArrayHeader(sys, nm){
	maxcount = INIT_VALUES_COUNT;
	values = new double[maxcount];
	clean();
}

void ArrayHeader::on_line(std::string& line){
	std::string subline = proc_line(line);

	if(subline.empty()){
		return;
	}
	std::stringstream ss(subline);
	std::string item;
	while (std::getline(ss, item, ',')) {
		double value;
		if(sscanf(item.c_str(), "%lf", &value) != 1){
			std::string var = trim(item);
			if(var.compare("SPACE") == 0){
				clean();  // forget about this array
				return;
			}
			throw CruncherException("Expected float value got: "+item);
		}
		add_value(value);
	}
}

void ArrayHeader::add_value(double val){
	if(curr_array == 0){
		throw CruncherException("Got value but no array index for "+modname);
	}
	if(vcount >= maxcount){
		/* increase storage size twice than before */
		long newcount = maxcount*2;
		double* newvalues = new double[newcount];
		memcpy(newvalues, values, maxcount*sizeof(values[0]));
		delete values;
		values = newvalues;
		maxcount = newcount;
	}
	values[vcount] = val;
	vcount++;
}

void ArrayHeader::clean(){
	curr_array = 0;
	vcount = 0;
}

void ArrayHeader::open(long num){
	curr_array = num;
}

void ArrayHeader::close(){
	if(curr_array != 0){
		system.on_array(modname.c_str(), curr_array, vcount, values);
		clean();
	}
}

ArrayHeader::~ArrayHeader(){
	close();
	delete values;
}
