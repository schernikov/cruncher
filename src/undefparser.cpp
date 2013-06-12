/*
 * undefparser.cpp
 *
 *  Created on: Jun 10, 2013
 *      Author: schernikov
 */

#include <iostream>

#include "undefparser.h"

using namespace std;

UndefParser::UndefParser(string& nm) : Parser(nm) {

}

void UndefParser::on_line(string& line) {
	//cout << "parsing file " << fname << endl;
}
