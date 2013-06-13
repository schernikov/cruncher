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
	void on_sun(double offset, const char* aname, long fnum, long tnum, size_t count, const long* anums);
	void on_conduct(long idx, const char* fmod, long fnode, const char* tmod, long tnode, double cond);
	void on_node(const char* mod, long node, double temp, double cap);
	void on_title(const char* title);
	void on_variable(const char* name, double val);
};


#endif /* SYSTEM_H_ */
