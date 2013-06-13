/*
 * system.h
 *
 *  Created on: Jun 12, 2013
 *      Author: schernikov
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <string>

struct System {
	void on_energy(const char* modname, long nodenum, const char* arraymod, long timesarray, long valsarray);
	void on_energy(const char* modname, long nodenum, double mult, const char* arraymod, long timesarray, long valsarray);
	void on_energy(const char* modname, long nodenum, double sum);
	void on_energy_assign(const char* frommod, long fromnode, const char* tomod, long tonode);
	void on_temp_array(const char* modname, long nodenum, const char* arraymod, long arraynum);
	void on_temp_assign(const char* frommod, long fromnode, const char* tomod, long tonode);
};


#endif /* SYSTEM_H_ */
