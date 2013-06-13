/*
 * node.h
 *
 *  Created on: Jun 12, 2013
 *      Author: schernikov
 */

#ifndef NODE_H_
#define NODE_H_

#include "header.h"

struct NodeHeader : public Header {
	NodeHeader(System& sys, const std::string& nm) : Header(sys, nm) {};
	~NodeHeader() {};

	void on_line(std::string& line);
};

#endif /* NODE_H_ */
