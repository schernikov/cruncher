/*
 * conductor.cpp
 *
 *  Created on: Jun 12, 2013
 *      Author: schernikov
 */

#include <cstring>
#include <cstdio>

#include "parser/conductor.h"

ConductHeader::ConductHeader(System& sys, const std::string& nm) : Header(sys, nm) {
	sprintf(format, "%sld,%s%d\[^.].%sld,%s%d\[^.].%sld,%slf", "%","%",MAX_NAME_LENGTH,"%","%",MAX_NAME_LENGTH, "%", "%");
}

char* trim_name(char* name){
	int len = strlen(name);
	int pos = len-1;
	while(name[pos] <= 0x20){
		if(pos == 0){
			throw CruncherException("Empty node name");
		}
		pos -= 1;
	}
	name[pos+1] = 0;
	pos = 0;
	while(name[pos] <= 0x20){
		pos += 1;
	}
	return name+pos;
}

void ConductHeader::on_line(std::string& line){
	long idx;
	char fromname[MAX_NAME_LENGTH], toname[MAX_NAME_LENGTH];
	long fromnode, tonode;
	double cond;

	/* 1,    ADCE.1,    NPANEL.13,    14.025         $ Contact */
	if(sscanf(line.c_str(), format, &idx, &fromname, &fromnode, &toname, &tonode, &cond) != 6){
		throw CruncherException("node parse failed for "+line);
	}
	system.on_conduct(idx, trim_name(fromname), fromnode, trim_name(toname), tonode, cond);
}
