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

#include "undefparser.h"
#include "parser/sindaparser.h"
#include "system.h"

using namespace std;

static void parsefile(System& system, const char* fname);

static void usage(const char* name){
    cerr << "Usage:  " << name << " <sinfile>" << endl;
}

int main(int argc, const char* argv[]) {
	const char* fname = argv[1];

	if(fname == 0){
		usage(argv[0]);
		return -1;
	}
	cout << "parsing file " << fname << endl;

	System system;

	parsefile(system, fname);

	system.process();

	return 0;
}

static void fend(Parser* parser, int& fcount, int flines){
	if(!parser) return;
	/* old file ends here */
	fcount += 1;
	cout << " ["<< fcount << "] name: " << parser->name() <<  " " << flines << " lines" << endl;
	delete parser;
}

void parsefile(System& system, const char* fname){
	std::ifstream infile(fname);
	int count = 0, flines = 0, fcount=0;
	std::string prefix("   INSERT ");
	std::string empty("\r");
	std::string arraysuff(".array");

	Parser* parser = 0;

	std::string line;
	std::string curname, cn;

	try {
		while (std::getline(infile, line)) {
			count += 1;
			bool comment = (line.at(0) == 'C' || line.at(0) == 'c');
			if (comment && startswith(1, line, prefix)){
				cn = selectafter(line, prefix, 1);
				if(endswith(cn, arraysuff)){
					cn.clear();
				}
			} else {
				std::string trimmed = trim(line);
				if(curname.compare(cn) != 0 && !cn.empty()){
					if (comment && trimmed.size() == 1){ /* empty comment line */
						fend(parser, fcount, flines);
						/* new file starts here */
						flines = 0;
						curname = cn;
						parser = new SindaParser(system, curname);
					} else {
						/* that was not new file; ignore this line */
						cn.clear();
					}
				} else {
					flines += 1;
					if (comment || trimmed.size() == 0 || (trimmed.size() == 1 && trimmed.at(0) == '`')){
						/* ignore comments and empty lines */
					} else {
						/* file content */
						if(parser) parser->on_line(line);
					}
				}
			}
		}
		fend(parser, fcount, flines);
	} catch (exception& e) {
		if(parser) delete parser;
		cerr << curname << " line " << flines << endl << "  " << e.what() << endl;
		return;
	}
	cout << "  number of lines: " << count << endl;
}
