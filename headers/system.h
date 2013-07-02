/*
 * system.h
 *
 *  Created on: Jun 12, 2013
 *      Author: schernikov
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <string>
#include <vector>
#include "elements.h"
#include "arcode.h"

typedef boost::unordered_map<std::string, Array*> ArrayMap;
typedef std::vector<std::string> StringVector;

struct System {
	std::string title;
	long total_nodes;
	long total_conducts;
	long total_rads;
	long total_lins;
	long diffus;
	long arithms;
	long bounds;
	long heats;
	long oneways;

	NodeSet hset;
	NodeSet qset;
	NodeSet tset;
	NodeSet aset;
	NodeSet bset;
	NodeSet dset;

	ArrayMap amap;

	NodeCollect collection;

	System();
	~System();

	void parsefile(const char* fname);

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
	void on_array(const char* name, long idx, long count, const double* values);
	void on_node_list(long idx, const char* name, long count, long* nodes);
	void on_approx(const char* mod, long node, long pos, double mult, const char* name, long num);

	void modnames(StringVector& nms);
	void getnodes(std::string& mod, long size, long arr[]);
	void pulltemps(std::string& mod, long size, long* idxs, double* vals);
	void pullcaps(std::string& mod, long size, long* idxs, double* vals);
	long nodescount(std::string& mod);
	void process();
	void report();
};


#endif /* SYSTEM_H_ */
