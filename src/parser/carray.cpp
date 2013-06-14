/*
 * carray.cpp
 *
 *  Created on: Jun 13, 2013
 *      Author: schernikov
 */

#include <iostream>
#include <cstdio>
#include <sstream>
#include <cstring>

#include "parser/carray.h"

#define INIT_VALUES_COUNT 1024

CarrayHeader::CarrayHeader(System& sys, const std::string& nm) : BaseArrayHeader(sys, nm) {
	sequence = 0;
	clean();
	maxcount = INIT_VALUES_COUNT;
	values = new long[maxcount];
}

void CarrayHeader::on_line(std::string& line){
	std::string subline = proc_line(line);

	if(subline.empty()){
		return;
	}
	std::stringstream ss(subline);
	std::string item;
	while (std::getline(ss, item, ',')) {
		long value;
		if(sscanf(item.c_str(), "%ld", &value) != 1){
			stop();
			std::string nm = trim(item);
			start(nm);
		} else {
			add_value(value);
		}
	}
}

void CarrayHeader::add_value(long val){
	if(name.empty()){
		throw CruncherException("Got value but no mod name for "+modname);
	}
	if(vcount >= maxcount){
		/* increase storage size twice than before */
		long newcount = maxcount*2;
		long* newvalues = new long[newcount];
		memcpy(newvalues, values, maxcount*sizeof(values[0]));
		delete values;
		values = newvalues;
		maxcount = newcount;
	}
	sequence++;
	values[vcount] = val;
	vcount++;
}

void CarrayHeader::clean(){
	name.clear();
	vcount = 0;
}

void CarrayHeader::start(std::string& nm){
	name = nm;
}

void CarrayHeader::stop(){
	if(!name.empty()){
		system.on_node_list(sequence, name.c_str(), vcount, values);
		clean();
	}
}

void CarrayHeader::open(long num){
	/* don't care about new array index */
}

void CarrayHeader::close(){
	/* don't care about array index */
}

CarrayHeader::~CarrayHeader() {
	std::cout << "   " << modname << " " << sequence << " nodes found" << std::endl;
	stop();
	delete values;
}
