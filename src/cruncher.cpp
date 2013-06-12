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
#include "ccparser.h"

using namespace std;

static void parsefile(const char* fname);

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

	parsefile(fname);

	return 0;
}

static void fend(Parser* parser, int& fcount, int flines){
	if(!parser) return;
	/* old file ends here */
	fcount += 1;
	cout << " ["<< fcount << "] name: " << parser->name() <<  " " << flines << " lines" << endl;
	delete parser;
}

void parsefile(const char* fname){
	std::ifstream infile(fname);
	int count = 0, flines = 0, fcount=0;
	std::string prefix("C   INSERT ");
	std::string emptycomm("C\r");
	std::string empty("\r");
	std::string comment("C ");
	std::string ccsuff(".cc");
	std::string arraysuff(".array");

	Parser* parser = 0;

	std::string line;
	std::string curname, cn;

	try {
		while (std::getline(infile, line)) {
			count += 1;
			if (startswith(line, prefix)){
				cn = selectafter(line, prefix);
				if(endswith(cn, arraysuff)){
					cn.clear();
				}
			} else {
				if(curname.compare(cn) != 0 && !cn.empty()){
					if (startswith(line, emptycomm)){
						fend(parser, fcount, flines);
						/* new file starts here */
						flines = 0;
						curname = cn;
						if(endswith(curname, ccsuff)){
							parser = new SindaParser(curname);
						} else {
							parser = new UndefParser(curname);
						}
					} else {
						/* that was not new file; ignore this line */
						cn.clear();
					}
				} else {
					flines += 1;
					if (line.empty() || startswith(line, comment) || line.compare(empty) == 0){
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
		cerr << curname << " line " << flines << endl << "  " << e.what() << endl;
		return;
	}
	cout << "  number of lines: " << count << endl;
}
