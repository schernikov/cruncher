//============================================================================
// Name        : cruncher.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <exception>

#include "system.h"

static void usage(const char* name){
    std::cerr << "Usage:  " << name << " <sinfile>" << std::endl;
}

int main(int argc, const char* argv[]) {
	const char* fname = argv[1];

	if(fname == 0){
		usage(argv[0]);
		return -1;
	}
	std::cout << "parsing file " << fname << std::endl;

	System system;

	system.parsefile(fname);

	system.process();

	system.report();

	return 0;
}

