/*
 * options.cpp
 *
 *  Created on: Jun 12, 2013
 *      Author: schernikov
 */

#include "parser/options.h"

static std::string outputline("OUTPUT = ");
static std::string saveline("SAVE = ");
static std::string user1line("USER1 = ");
static std::string user2line("USER2 = ");
static std::string doubleline("DOUBLEPRECISION");
static std::string mline("MLINE = ");
static std::string mixarrayline("MIXARRAY");
static std::string titleline("TITLE");

void OptionsHeader::on_line(std::string& line){
	std::string subline = trim(line);
	if(startswith(subline, outputline)){
		return;
	}
	if(startswith(subline, saveline)){
		return;
	}
	if(startswith(subline, user1line) || startswith(subline, user2line)){
		return;
	}
	if(startswith(subline, doubleline)){
		return;
	}
	if(startswith(subline, mline)){
		return;
	}
	if(startswith(subline, mixarrayline)){
		return;
	}
	if(startswith(subline, titleline)){
		return;
	}
	throw CruncherException("Unexpected options line: "+line);
}
