/*
 * basearray.cpp
 *
 *  Created on: Jun 13, 2013
 *      Author: schernikov
 */

#include <cstdio>

#include "parser/basearray.h"

std::string BaseArrayHeader::proc_line(const std::string& line){
	std::string subline;
	size_t epos = line.find("$");
	std::string ln;
	if(epos != std::string::npos){
		ln = line.substr(0, epos);
	} else {
		ln = line;
	}
	size_t pos = ln.find("=");
	if(pos == std::string::npos){
		/* continue with previous array */
		subline = trim(ln);
	} else {
		/* this is new array start */
		close();

		std::string aname = ln.substr(0, pos);
		long num;
		if(sscanf(aname.c_str(), "%ld", &num) != 1){
			throw CruncherException("Expected array index: "+aname);
		}
		open(num);

		subline = ln.substr(pos+1, std::string::npos);
		subline = trim(subline);
	}
	return subline;
}
