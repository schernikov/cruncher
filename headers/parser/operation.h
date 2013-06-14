/*
 * operation.h
 *
 *  Created on: Jun 13, 2013
 *      Author: schernikov
 */

#ifndef OPERATION_H_
#define OPERATION_H_

#include "header.h"

struct OperationHeader : public Header {
	OperationHeader(System& sys, const std::string& nm) : Header(sys, nm) {};
	~OperationHeader() {};

	void on_line(std::string& line);
};


#endif /* OPERATION_H_ */
