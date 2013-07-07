/*
 * elements.h
 *
 *  Created on: Jun 28, 2013
 *      Author: schernikov
 */

#ifndef ELEMENTS_H_
#define ELEMENTS_H_

#include <string>
#include <set>
#include <boost/unordered_map.hpp>

#define NAN_TEMP   -1000000.0

struct Node;

typedef boost::unordered_map<long, Node*> NodeMap;
typedef boost::unordered_map<Node*, double> ConductMap;


struct Module {
	std::string name;
	NodeMap nodes;
};

struct Node {
	Module* parent;
	const long num;
	double temperature;
	double capacitance;
	ConductMap lins;
	ConductMap rads;

	Node(Module* mod, long num);
	void init(double temp, double cap);
	Node* self() { return this;};
	double connect(bool lin, Node& nd, double cond);
};

struct StatInfo {
	long count;
	double sum;
};

typedef std::set<Node*> NSet;
typedef boost::unordered_map<std::string, StatInfo> ModInfo;

struct NodeSet {
	NSet set;
	ModInfo info;
	void add(Node& node);
	void add(Node& node, double val);
	void show(const char* pref);
};

typedef boost::unordered_map<std::string, Module> ModuleMap;

struct NodeCollect {
	ModuleMap modules;

	Node& get(const char* name, long num);
	~NodeCollect();
};

#endif /* ELEMENTS_H_ */
