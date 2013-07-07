/*
 * elements.cpp
 *
 *  Created on: Jun 28, 2013
 *      Author: schernikov
 */

#include "elements.h"
#include <cstdio>
#include <boost/foreach.hpp>

Node::Node(Module* mod, long nm) : num(nm){
	parent = mod;
	this->temperature = NAN_TEMP;
	this->capacitance = -1.0;
}

void Node::init(double temp, double cap){
	this->temperature = temp;
	this->capacitance = cap;
}

double Node::connect(bool lin, Node& nd, double cond){
	ConductMap* cm = lin ? (&lins) : (&rads);
	ConductMap::iterator it = cm->find(&nd);
	if(it == cm->end()){
		(*cm)[&nd] = cond;
		return cond;
	}
	double old = it->second;
	it->second = cond;
	return old;
}

Node& NodeCollect::get(const char* name, long num){
	Module* mod;
	ModuleMap::iterator mit = modules.find(name);
	if (mit == modules.end()){
		mod = &modules[name];
		mod->name = name;
	} else {
		mod = &mit->second;
	}
	NodeMap::iterator nit = mod->nodes.find(num);
	if (nit == mod->nodes.end()){
		Node* newnode = new Node(mod, num);
		mod->nodes[num] = newnode;
		return *newnode;
	}
	return *nit->second;
}

NodeCollect::~NodeCollect() {
	BOOST_FOREACH(ModuleMap::value_type mi, modules) {
		Module& mod = mi.second;
		BOOST_FOREACH(NodeMap::value_type ni, mod.nodes) {
			delete ni.second;
		}
	}
}

void NodeSet::add(Node& node){
	add(node, 0);
}

void NodeSet::add(Node& node, double val){
	/*std::pair<NSet::iterator,bool> ret = */
	set.insert(node.self());

	ModInfo::iterator it = info.find(node.parent->name);
	if (it == info.end()){
		StatInfo& stat = info[node.parent->name];
		stat.count = 1;
		stat.sum = val;
	} else {
		it->second.count += 1;
		it->second.sum += val;
	}

}

void NodeSet::show(const char* pref){
	BOOST_FOREACH(ModInfo::value_type i, info) {
		const std::string& name = i.first;
		StatInfo& info = i.second;
		if(info.sum == 0){
			printf("%s%s[%ld]\n", pref, name.c_str(), info.count);
		} else {
			printf("%s%s[%ld] %.3lf\n", pref, name.c_str(), info.count, info.sum);
		}
	}
}
