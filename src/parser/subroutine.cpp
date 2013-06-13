/*
 * subroutine.cpp
 *
 *  Created on: Jun 12, 2013
 *      Author: schernikov
 */


#include "parser/subroutine.h"

static const std::string subsubroutine="SUBROUTINE";
static const std::string subreturn="RETURN";
static const std::string subend="END";

void SubroutineHeader::on_line(std::string& line){
	std::string subline;
	if(line.at(0) == 'F'){
		subline = line.substr(1, std::string::npos);
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
