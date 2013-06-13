/*
 * node.cpp
 *
 *  Created on: Jun 12, 2013
 *      Author: schernikov
 */

#include <cstdio>

#include "parser/node.h"

void NodeHeader::on_line(std::string& line){
	long node;
	double temp, cap;

	/* 1,    20.,    128.2271 */
	if(sscanf(line.c_str(), "%ld,%lf,%lf\r", &node, &temp, &cap) != 3){
		throw CruncherException("node parse failed for "+line);
	}
	system.on_node(modname.c_str(), node, temp, cap);
}
