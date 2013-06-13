/*
 * conductor.cpp
 *
 *  Created on: Jun 12, 2013
 *      Author: schernikov
 */

#include <cstdio>

#include "parser/conductor.h"

ConductHeader::ConductHeader(System& sys, const std::string& nm) : Header(sys, nm) {
	sprintf(format, "%sld,%s%d\[^.].%sld,%s%d\[^.].%sld,%sf", "%","%",MAX_NAME_LENGTH,"%","%",MAX_NAME_LENGTH, "%", "%");
}

void ConductHeader::on_line(std::string& line){
	long idx;
	char fromname[MAX_NAME_LENGTH], toname[MAX_NAME_LENGTH];
	long fromnode, tonode;
	double cap;

	/* 1,    ADCE.1,    NPANEL.13,    14.025         $ Contact */
	if(sscanf(line.c_str(), format, &idx, &fromname, &fromnode, &toname, &tonode, &cap) != 6){
		throw CruncherException("node parse failed for "+line);
	}
}
