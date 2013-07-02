/*
 * system.cpp
 *
 *  Created on: Jun 12, 2013
 *      Author: schernikov
 */

#include <cstdio>
#include <sstream>
#include <limits>
#include <iostream>
#include <fstream>
#include <string>
#include <exception>

#include <boost/foreach.hpp>

#include "common.h"
#include "system.h"
#include "arcode.h"

#include "parser/sindaparser.h"
#include "elements.h"

static bool show = false;

System::System(){
	total_nodes = 0;
	total_conducts = 0;
	total_rads = 0;
	total_lins = 0;
	diffus = 0;
	arithms = 0;
	bounds = 0;
	heats = 0;
	oneways = 0;
}

void System::on_title(const char* ttl){
	title = ttl;
}

static std::string mkarname(const char* name, long idx){
	char buf[MAX_NAME_LENGTH];
	int n = snprintf(buf, sizeof(buf), "%s.%ld", name, idx);
	if(n <= 0 || n >= (int)sizeof(buf)){
		throw CruncherException("Can not create array name: %s.%ld", name, idx);
	}
	return buf;
}

static const Array* findarray(const ArrayMap& mp, const char* arraymod, long idx){
	std::string arnm = mkarname(arraymod, idx);
	ArrayMap::const_iterator it = mp.find(arnm);
	if (it == mp.end()){
		throw CruncherException("Can not find array: %s", arnm.c_str());
	}
	return it->second;
}

static void checkarsizes(const Array* timearr, const Array* valsarr){
	if(valsarr->count != timearr->count){
		throw CruncherException("Array sizes don't match: %s[%ld] != %s[%ld]",
				timearr->name.c_str(), timearr->count, valsarr->name.c_str(), valsarr->count);
	}
}

/* array energy value assigned directly to node */
void System::on_energy(const char* modname, long nodenum, const char* arraymod, long timesarray, long valsarray){
	Node& nd = collection.get(modname, nodenum);
	qset.add(nd);
	const Array* timearr = findarray(amap, arraymod, timesarray);
	const Array* valsarr = findarray(amap, arraymod, valsarray);

	checkarsizes(timearr, valsarr);

	if(show) printf("  Q[%ld] => %s.%ld\n", timearr->count, modname, nodenum);
}

/* increment node energy from array with multiplier */
void System::on_energy(const char* modname, long nodenum, double mult, const char* arraymod, long timesarray, long valsarray){
	Node& nd = collection.get(modname, nodenum);
	qset.add(nd, mult);

	const Array* timearr = findarray(amap, arraymod, timesarray);
	const Array* valsarr = findarray(amap, arraymod, valsarray);

	checkarsizes(timearr, valsarr);

	if(show) printf("  Q[%ld] => %s.%ld + %.4lf*dt\n", timearr->count, modname, nodenum, mult);
}

/* increment node energy with constant value */
void System::on_energy(const char* modname, long nodenum, double sum){
	Node& nd = collection.get(modname, nodenum);
	qset.add(nd, sum);
	if(show) printf("  Q => %s.%ld + %.4lf\n", modname, nodenum, sum);
}

/* assign one node energy to another */
void System::on_energy_assign(const char* frommod, long fromnode, const char* tomod, long tonode){
	Node& nd = collection.get(tomod, tonode);
	qset.add(nd);
	if(show) printf("  Q => %s.%ld => %s.%ld\n", frommod, fromnode, tomod, tonode);
}

/* assign array temperature to node */
void System::on_temp_array(const char* modname, long nodenum, const char* arraymod, long arraynum){
	Node& nd = collection.get(modname, nodenum);
	tset.add(nd);
	const Array* arr = findarray(amap, arraymod, arraynum);
	if(show) printf("  T[%ld] => %s.%ld\n", arr->count, modname, nodenum);
}

/* assign one node temperature to another */
void System::on_temp_assign(const char* frommod, long fromnode, const char* tomod, long tonode){
	Node& nd = collection.get(tomod, tonode);
	tset.add(nd);
	if(show) printf("  T => %s.%ld => %s.%ld\n", frommod, fromnode, tomod, tonode);
}

void System::on_sun(double offset, const char* aname, long fnum, long tnum, size_t count, const long* anums){
	/*  aname    - Submodel name that contains the carray data
		fnum     - First carray number
		tnum     - Total number of carrays used
		count    - number of anums[]
		anums[0] - Storage array internally used for relative node numbers
		anums[1] - Area array if data output as flux, set to zero if total absorbed
		anums[2] - Array of steady state values
		anums[3] - Time array
		anums[4] - Scale array where heating rates can be scaled
	*/
	//TODO: process
}

void System::on_conduct(long idx, const char* fmod, long fnode, const char* tmod, long tnode, double cond){
	total_conducts += 1;
	if(tnode <= 0 || idx == 0){
		std::stringstream ss;
		ss << "Unexpected conductor: [" << idx<< "]"<< fmod << "."<< fnode << " -> "<< tmod << "." << tnode << " = "<< cond;
		throw CruncherException(ss.str());
	}
	if(fnode < 0){
		/* one-way conductor */
		oneways += 1;
		if(show) printf("one-way: [%ld] %s.%ld -> %s.%ld = %.2lf\n", idx, fmod, fnode, tmod, tnode, cond);
	}
	if(cond < 0){
		cond = -cond;
	}
	if(idx > 0){
		total_lins += 1;
		/* linear conductor */
		return;
	}
	/* radiative conductor */
	idx = -idx;
	total_rads += 1;
}

static const int NBUCKETS = 30;
static const double MAX_CAP = 100.0;
static const double buckstep = MAX_CAP/(NBUCKETS-1);
static long histcounts[NBUCKETS] = { 0 };
static double histtotals[NBUCKETS] = { 0.0 };
static double minval = std::numeric_limits<double>::max();
static double maxval = std::numeric_limits<double>::min();

static int histbucket(double val){
	if(MAX_CAP <= val){
		return NBUCKETS-1;
	}
	return (long)val/buckstep;
}

static void histogramm(double val){
	if(val < minval){
		minval = val;
	}
	if(val > maxval){
		maxval = val;
	}
	int idx = histbucket(val);
	histcounts[idx] += 1;
	histtotals[idx] += val;
}

void System::on_node(const char* mod, long node, double temp, double cap){
	total_nodes += 1;
	if(node > 0 && cap > 0){
		/* diffusion node */
		if(cap >= MAX_CAP){
			if(show) printf("high diffuse: %s.%ld T:%.2lf C:%.2lf\n", mod, node, temp, cap);
		}
		histogramm(cap);
		diffus += 1;
		Node& nd = collection.get(mod, node);
		nd.init(temp, cap);
		dset.add(nd);
		return;
	}
	if(node > 0 && cap < 0){
		/* arithmetic node; cap = 0 */
		if(show) printf("arithm: %s.%ld T:%.2lf C:%.2lf\n", mod, node, temp, cap);
		arithms += 1;
		Node& nd = collection.get(mod, node);
		nd.init(temp, 0);
		aset.add(nd);
		return;
	}
	if(node < 0 && cap >= 0){
		/* boundary node; cap = inf */
		if(show) printf("bounds: %s.%ld T:%.2lf C:%.2lf\n", mod, node, temp, cap);
		bounds += 1;
		Node& nd = collection.get(mod, -node);
		nd.init(temp, 0);
		bset.add(nd);
		return;
	}
	if(node < 0 && cap < 0){
		/* heater node; cap=inf */
		if(show) printf("heater: %s.%ld T:%.2lf C:%.2lf\n", mod, node, temp, cap);
		heats += 1;
		Node& nd = collection.get(mod, -node);
		nd.init(temp, 0);
		hset.add(nd);
		return;
	}
	throw CruncherException("Unexpected node: %s.%ld temp:%lf cap:%lf", mod, node, temp, cap);
}

void System::on_variable(const char* name, double val){

}

void System::on_array(const char* name, long idx, long count, const double* values){
	std::string arname = mkarname(name, idx);
	if (amap.find(arname) != amap.end()){
		throw CruncherException("Duplicate array name: %s", arname.c_str());
	}
	amap[arname] = new Array(arname, count, values);
}

void System::on_node_list(long idx, const char* name, long count, long* nodes){

}

/* called on sensor temperature approximation equation */
void System::on_approx(const char* mod, long node, long pos, double mult, const char* name, long num){
	if(show) printf("  %s.%ld[%ld]= + %f*%s.%ld\n", mod, node, pos, mult, name, num);
}

void System::process(){

}

void System::modnames(StringVector& nms){
	BOOST_FOREACH(ModuleMap::value_type i, collection.modules) {
		nms.push_back(i.first);
	}
}

static const NodeMap& getnodemap(ModuleMap& mmap, std::string& mod){
	ModuleMap::const_iterator it = mmap.find(mod);
	if (it == mmap.end()){
		throw CruncherException("module %s does not exist", mod.c_str());
	}
	return it->second.nodes;
}

long System::nodescount(std::string& mod){
	const NodeMap& nmap = getnodemap(collection.modules, mod);
	return nmap.size();
}

const NodeMap& getsafenodes(ModuleMap& mmap, std::string& mod, long size){
	const NodeMap& nmap = getnodemap(mmap, mod);
	if((long)nmap.size() != size){
		throw CruncherException("%ld is different from %ld in %s", size, nmap.size(), mod.c_str());
	}
	return nmap;
}

void System::getnodes(std::string& mod, long size, long arr[]){
	const NodeMap& nmap = getsafenodes(collection.modules, mod, size);
	int pos = 0;
	BOOST_FOREACH(NodeMap::value_type i, nmap) {
		arr[pos++] = i.first;
	}
}

void System::pulltemps(std::string& mod, long size, long* idxs, double* vals){
	const NodeMap& nmap = getsafenodes(collection.modules, mod, size);
	int pos = 0;
	BOOST_FOREACH(NodeMap::value_type i, nmap) {
		idxs[pos] = i.first;
		vals[pos] = i.second->temperature;
		pos += 1;
	}
}

void System::pullcaps(std::string& mod, long size, long* idxs, double* vals){
	const NodeMap& nmap = getsafenodes(collection.modules, mod, size);
	int pos = 0;
	BOOST_FOREACH(NodeMap::value_type i, nmap) {
		idxs[pos] = i.first;
		vals[pos] = i.second->capacitance;
		pos += 1;
	}
}

void System::report(){
	printf("\n\n");
	printf("===== %s =====\n", title.c_str());
	printf("Total nodes: %ld\n", total_nodes);
	printf("    diffuse: %ld\n", diffus);
	dset.show("        ");
	printf("     arithm: %ld\n", arithms);
	aset.show("        ");
	printf("      bound: %ld\n", bounds);
	bset.show("        ");
	printf("      heats: %ld\n", heats);
	hset.show("        ");
	printf("external Qs:\n");
	qset.show("        ");
	printf("external Ts:\n");
	tset.show("        ");
	printf("Total conducts: %ld\n", total_conducts);
	printf("   linear: %ld\n", total_lins);
	printf("     rads: %ld\n", total_rads);
	printf("  one-way: %ld\n", oneways);
	printf("\n");
	printf("==========\n");
	printf("  %ld arrays\n", amap.size());
	printf("\n");
	double bucklevel = 0;
	printf("min:%.4f max:%.4f\n", minval, maxval);
	for (int i = 0; i < NBUCKETS; ++i) {
		printf("%s[%d] %5ld %10.4lf %10.4lf\n",(i<10)?" ":"", i, histcounts[i], bucklevel, histtotals[i]);
		bucklevel += buckstep;
	}
}

System::~System(){
	BOOST_FOREACH(ArrayMap::value_type i, amap) {
		Array* parray = i.second;
		delete parray;
	}
}

static void fend(Parser* parser, int& fcount, int flines){
	if(!parser) return;
	/* old file ends here */
	fcount += 1;
	std::cout << " ["<< fcount << "] name: " << parser->name() <<  " " << flines << " lines" << std::endl;
	delete parser;
}

void System::parsefile(const char* fname){
	std::ifstream infile(fname);
	int count = 0, flines = 0, fcount=0;
	std::string prefix("   INSERT ");
	std::string empty("\r");
	std::string arraysuff(".array");

	Parser* parser = 0;

	std::string line;
	std::string curname, cn;

	try {
		while (std::getline(infile, line)) {
			count += 1;
			bool comment = (line.at(0) == 'C' || line.at(0) == 'c');
			if (comment && startswith(1, line, prefix)){
				cn = selectafter(line, prefix, 1);
				if(endswith(cn, arraysuff)){
					cn.clear();
				}
			} else {
				std::string trimmed = trim(line);
				if(curname.compare(cn) != 0 && !cn.empty()){
					if (comment && trimmed.size() == 1){ /* empty comment line */
						fend(parser, fcount, flines);
						/* new file starts here */
						flines = 0;
						curname = cn;
						parser = new SindaParser(*this, curname);
					} else {
						/* that was not new file; ignore this line */
						cn.clear();
					}
				} else {
					flines += 1;
					if (comment || trimmed.size() == 0 || (trimmed.size() == 1 && trimmed.at(0) == '`')){
						/* ignore comments and empty lines */
					} else {
						/* file content */
						if(parser) parser->on_line(line);
					}
				}
			}
		}
		fend(parser, fcount, flines);
	} catch (std::exception& e) {
		if(parser) delete parser;
		std::cerr << curname << " line " << flines << std::endl << "  " << e.what() << std::endl;
		return;
	}
	std::cout << "  number of lines: " << count << std::endl;
}
