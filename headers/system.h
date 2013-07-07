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
#include <utility>
#include "elements.h"
#include "arcode.h"

typedef boost::unordered_map<std::string, Array*> ArrayMap;
typedef std::vector<std::string> StringVector;
typedef std::pair<std::string, std::string> NamesPair;
typedef std::pair<long, long> IndexPair;
typedef std::pair<std::string, long> NodeDesc;
typedef std::pair<Module*, Module*> ModPair;
typedef std::vector<NodeDesc> NodesVector;
typedef boost::unordered_map<Module*, IndexPair> ModStatsMap;
typedef boost::unordered_map<long, Node*> SunNodesMap;
typedef boost::unordered_map<std::string, NodeSet*> NodeSetMap;

typedef std::pair<Node*, Node*> NodePair;
typedef boost::unordered_map<NodePair, double> ConnectionMap;
typedef boost::unordered_map<NodePair, double> ConflictMap;

struct ModConnects {
	std::string fr;
	std::string to;
	long lins;
	long rads;
};

typedef std::vector<ModConnects> ModsVector;

typedef double* (*DAllocator)(size_t size, void* f);
typedef long* (*LAllocator)(size_t size, void* f);

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

	double start;
	double stop;

	NodeSet hset;
	NodeSet qset;
	NodeSet tset;
	NodeSet aset;
	NodeSet bset;
	NodeSet dset;
	NodeSet sset;
	NodeSetMap nodesets;

	ArrayMap amap;

	ArraySet tarrs;
	ArraySet qarrs;
	ArraySet sarrs;

	NodeCollect collection;

	ConflictMap confcons;
	long conflicts;

	const Array* sun_time;
	SunNodesMap sun_nodes;
	double sun_offset;

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
	const Array* on_array(const char* name, long idx, long count, const double* values);
	void on_node_list(long idx, const char* name, long count, long* nodes);
	void on_approx(const char* mod, long node, long pos, double mult, const char* name, long num);
	void on_warning(const char * format, ... );

	void modnames(StringVector& nms);
	void getnodes(std::string& mod, LAllocator alloc, void* f);
	void pulltemps(std::string& mod, LAllocator la, void* l, DAllocator da, void* d);
	void pullcaps(std::string& mod, LAllocator la, void* l, DAllocator da, void* d);
	ArraySet& getaset(std::string& type);
	void arraynodes(std::string& type, NodesVector& nodes);
	double pullarray(std::string& type, std::string& mod, long num, DAllocator alloc, void* f);
	void setnodes(std::string& tp, NodesVector& nodes);

	void modconns(ModsVector& vect);
	void consvals(std::string& tp, std::string& fmod, std::string& tmod, size_t size, LAllocator la, DAllocator da, void* f);

	void process();
	void report();
};


#endif /* SYSTEM_H_ */
