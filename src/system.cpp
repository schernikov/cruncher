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
#include <memory>
#include <stdarg.h>

#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

#include "common.h"
#include "system.h"
#include "arcode.h"

#include "parser/sindaparser.h"
#include "elements.h"

extern std::string cr_vformat (const char *fmt, va_list ap);

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
	start = -1.0;
	stop = -1.0;
	sun_time = 0;
	sun_offset = 0;
	nodesets["heat"] = &hset;
	nodesets["energy"] = &qset;
	nodesets["temperature"] = &tset;
	nodesets["arithmetic"] = &aset;
	nodesets["boundary"] = &bset;
	nodesets["diffuse"] = &dset;
	nodesets["sun"] = &sset;
	nodesets["one"] = &oset;
	nodesets["approximated"] = &kset;
	nodesets["undefined"] = &uset;
	conflicts = 0;
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

static std::string mkaltname(const char* pref, const char* name, long idx){
	char buf[MAX_NAME_LENGTH];
	int n = snprintf(buf, sizeof(buf), "%s%s.%ld", pref, name, idx);
	if(n <= 0 || n >= (int)sizeof(buf)){
		throw CruncherException("Can not create array name: %s.%ld", name, idx);
	}
	return buf;
}

static const Array* findarray(const ArrayMap& mp, const char* arraymod, long idx, bool safe=true){
	std::string arnm = mkarname(arraymod, idx);
	ArrayMap::const_iterator it = mp.find(arnm);
	if (it == mp.end()){
		if(safe){
			throw CruncherException("Can not find array: %s", arnm.c_str());
		}
		return 0;
	}
	return it->second;
}

/* array energy value assigned directly to node */
void System::on_energy(const char* modname, long nodenum, const char* arraymod, long timesarray, long valsarray){
	Node& nd = collection.get(modname, nodenum);
	qset.add(nd);
	const Array* timearr = findarray(amap, arraymod, timesarray);
	const Array* valsarr = findarray(amap, arraymod, valsarray);

	qarrs.add(nd, 1.0, timearr, valsarr);

	if(show) printf("  Q[%ld] => %s.%ld\n", timearr->count, modname, nodenum);
}

/* increment node energy from array with multiplier */
void System::on_energy(const char* modname, long nodenum, double mult, const char* arraymod, long timesarray, long valsarray){
	Node& nd = collection.get(modname, nodenum);
	qset.add(nd, mult);

	const Array* timearr = findarray(amap, arraymod, timesarray);
	const Array* valsarr = findarray(amap, arraymod, valsarray);
	qarrs.add(nd, mult, timearr, valsarr);

	if(show) printf("  Q[%ld] => %s.%ld + %.4lf*dt\n", timearr->count, modname, nodenum, mult);
}

/* increment node energy with constant value */
void System::on_energy(const char* modname, long nodenum, double sum){
	Node& nd = collection.get(modname, nodenum);
	qset.add(nd, sum);
	double tms[] = {start, stop};
	double vals[] = {sum, sum};
	std::string aname = mkaltname("array_", modname, nodenum);
	const Array* tarr = findarray(amap, aname.c_str(), 1, false);
	const Array* varr = findarray(amap, aname.c_str(), 2, false);
	if(tarr || varr){
		on_warning("%s.%ld had a duplicate energy array", modname, nodenum);
		amap.erase(tarr->name);
		amap.erase(varr->name);
		delete tarr;
		delete varr;
	}
	tarr = on_array(aname.c_str(), 1, sizeof(tms)/sizeof(tms[0]), tms);
	varr = on_array(aname.c_str(), 2, sizeof(vals)/sizeof(vals[0]), vals);
	qarrs.add(nd, 1.0, tarr, varr);
	if(show) printf("  Q => %s.%ld + %.4lf\n", modname, nodenum, sum);
}

void System::on_warning(const char * format, ... ){
	  va_list args;
	  va_start (args, format);
	  std::cerr << cr_vformat (format, args) << std::endl;
	  va_end (args);
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
	std::string aname = mkaltname("split_", arraymod, arraynum);
	const Array* tarr = findarray(amap, aname.c_str(), 1, false);
	const Array* varr = findarray(amap, aname.c_str(), 2, false);
	if(!tarr || !varr){
		const Array* arr = findarray(amap, arraymod, arraynum);
		if(arr->count == 0 || arr->count%1 != 0){
			throw CruncherException("Expected '%s' array with even size, got %d instead", arr->name.c_str(), arr->count);
		}

		size_t size = arr->count/2;
		{
			std::auto_ptr<double> tms(new double[size]);
			std::auto_ptr<double> vals(new double[size]);
			double* ptms = tms.get();
			double* pvals = vals.get();
			for (int i = 0; i < (int)size; ++i) {
				ptms[i] = arr->vals[2*i];
				pvals[i] = arr->vals[2*i+1];
			}
			amap.erase(arr->name);
			delete arr;
			tarr = on_array(aname.c_str(), 1, size, ptms);
			varr = on_array(aname.c_str(), 2, size, pvals);
		}
	}
	tarrs.add(nd, 1.0, tarr, varr);
	if(show) printf("  T[%ld] => %s.%ld\n", tarr->count, modname, nodenum);
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
	if(count < 5){
		throw CruncherException("Unexpected sun arrays");
	}
	long scaleidx = anums[4];
	const Array* scalearr = findarray(amap, aname, scaleidx);
	if(scalearr->count != 1 || scalearr->vals[0] != 1.0){
		throw CruncherException("Unexpected scale array for sun data: %s.%ld[%ld]=%lf",
								aname, scaleidx, scalearr->count, scalearr->vals[0]);
	}
	sun_time = findarray(amap, aname, anums[3]);
	sun_offset = offset;
}

void System::on_conduct(long idx, const char* fmod, long fnode, const char* tmod, long tnode, double cond){
	total_conducts += 1;
	if(tnode <= 0 || idx == 0){
		std::stringstream ss;
		ss << "Unexpected conductor: [" << idx<< "]"<< fmod << "."<< fnode << " -> "<< tmod << "." << tnode << " = "<< cond;
		throw CruncherException(ss.str());
	}
	bool one = false;
	if(fnode < 0){
		/* one-way conductor */
		fnode = -fnode;
		one = true;
		if(show) printf("one-way: [%ld] %s.%ld -> %s.%ld = %.2lf\n", idx, fmod, fnode, tmod, tnode, cond);
	}
	if(cond < 0){
		cond = -cond;
	}
	Node& fnd = collection.get(fmod, fnode);
	Node& tnd = collection.get(tmod, tnode);
	if(one){
		oset.add(tnd);
	}

	double old;
	if(idx > 0){
		total_lins += 1;
		/* linear conductor */
		old = fnd.connect(true, tnd, cond);
	} else {
		/* radiative conductor */
		total_rads += 1;
		old = fnd.connect(false, tnd, cond);
	}
	if(old != cond){
		/* conflicting conductor definitions */
		NodePair np = NodePair(&fnd, &tnd);
		conflicts += 1;
		confcons[np] = old;
	}
}

static const double MAX_CAP = 100.0;

void System::on_node(const char* mod, long node, double temp, double cap){
	total_nodes += 1;
	if(node > 0 && cap > 0){
		/* diffusion node */
		if(cap >= MAX_CAP){
			if(show) printf("high diffuse: %s.%ld T:%.2lf C:%.2lf\n", mod, node, temp, cap);
		}
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
	if (boost::iequals(name, "timeo")){
		start = val;
	}
	if (boost::iequals(name, "timend")){
		stop = val;
	}
}

const Array* System::on_array(const char* name, long idx, long count, const double* values){
	std::string arname = mkarname(name, idx);
	if (amap.find(arname) != amap.end()){
		throw CruncherException("Duplicate array name: %s", arname.c_str());
	}
	Array* arr = new Array(arname, name, idx, count, values);
	amap[arname] = arr;
	return arr;
}

void System::on_node_list(long idx, const char* name, long count, long* nodes){
	long first = idx-count+1;
	for (int i = 0; i < count; ++i) {
		Node& nd = collection.get(name, nodes[i]);
		sun_nodes[first+i] = &nd;
		sset.add(nd);
	}
}

/* called on sensor temperature approximation equation */
void System::on_approx(const char* mod, long node, long pos, double mult, const char* name, long num){
	Node& xnd = collection.get(mod, node);
	Node& nd = collection.get(name, num);

	ApproxVector& vec = approxes[&xnd];
	if(vec.size() == 0){
		kset.add(xnd); /* need to do it only once for given destination node */
	}
	vec.push_back(WeightPair(&nd, mult));

	if(show) printf("  %s.%ld[%ld]= + %f*%s.%ld\n", mod, node, pos, mult, name, num);
}

void System::process(){
	if(!sun_time){
		throw CruncherException("Sun time information is missing");
	}
	long lastidx = sun_time->count-1;
	double first = sun_time->vals[0];
	double last = sun_time->vals[lastidx];
	const static double day = 86400; /* seconds */
	if(first != 0){
		throw CruncherException("Expected sun time to start from 0; got %lf indstead", first);
	}
	if(last > (stop-start)){
		throw CruncherException("Not implemented for spans shorter than 1 day");
	}
	while(last >= day){
		lastidx -= 1;
		last = sun_time->vals[lastidx];
	}
	int start_day = ((int)((start+sun_offset)/day));
	int stop_day = ((int)((stop+sun_offset+day-1)/day));
	int daysnum = stop_day-start_day;
	int daypoints = lastidx+1;
	int allpoints = daysnum*daypoints+1;
	double* newtimes = new double[allpoints];
	double off = start_day*day-sun_offset-first;
	for (int pos=0, dnum=0, idx = 0; idx < allpoints; ++idx) {
		newtimes[idx] = sun_time->vals[pos++]+off+dnum*day;
		if(pos > lastidx){
			pos = 0;
			dnum += 1;
		}
	}

	((Array*)sun_time)->replace(allpoints, newtimes);

	BOOST_FOREACH(SunNodesMap::value_type si, sun_nodes) {
		long idx = si.first;
		Node* nodes = si.second;
		const Array* varr = findarray(amap, sun_time->mod.c_str(), sun_time->idx+idx);
		double* newvals = new double[allpoints];

		for (int pos=0, dnum=0, idx = 0; idx < allpoints; ++idx) {
			newvals[idx] = varr->vals[pos++];
			if(pos > lastidx){
				pos = 0;
				dnum += 1;
			}
		}
		((Array*)varr)->replace(allpoints, newvals);

		sarrs.add(*nodes, 1.0, sun_time, varr);
	}

	/* let's try to find undefined nodes */
	BOOST_FOREACH(ModuleMap::value_type i, collection.modules) {
		BOOST_FOREACH(NodeMap::value_type nid, i.second.nodes) {
			Node& nd = *nid.second;
			if(nd.capacitance < 0){
				uset.add(nd);
			}
		}
	}
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

const NodeMap& getsafenodes(ModuleMap& mmap, std::string& mod, long size){
	const NodeMap& nmap = getnodemap(mmap, mod);
	if((long)nmap.size() != size){
		throw CruncherException("%ld is different from %ld in %s", size, nmap.size(), mod.c_str());
	}
	return nmap;
}

void System::getnodes(std::string& mod, LAllocator alloc, void* f){
	const NodeMap& nmap = getnodemap(collection.modules, mod);
	size_t size = nmap.size();
	if(!size) return;
	long* arr = alloc(size, f);
	int pos = 0;
	BOOST_FOREACH(NodeMap::value_type i, nmap) {
		arr[pos++] = i.first;
	}
}

typedef double (*ValCall)(Node* nd);

static void pullvals(const NodeMap& nmap, LAllocator la, void* l, DAllocator da, void* d, ValCall valcall){
	size_t size = nmap.size();
	if(!size) return;
	long* idxs = la(size, l);
	double* vals = da(size, d);
	int pos = 0;
	BOOST_FOREACH(NodeMap::value_type i, nmap) {
		idxs[pos] = i.first;
		vals[pos] = valcall(i.second);
		pos += 1;
	}
}

static double tempcall(Node* nd){
	return nd->temperature;
}

void System::pulltemps(std::string& mod, LAllocator la, void* l, DAllocator da, void* d){
	pullvals(getnodemap(collection.modules, mod), la, l, da, d, tempcall);
}

static double capcall(Node* nd){
	return nd->capacitance;
}

void System::pullcaps(std::string& mod, LAllocator la, void* l, DAllocator da, void* d){
	pullvals(getnodemap(collection.modules, mod), la, l, da, d, capcall);
}

ArraySet& System::getaset(std::string& type){
	if (boost::iequals(type, "energy")){
		return qarrs;
	}
	if (boost::iequals(type, "temperature")){
		return tarrs;
	}
	if (boost::iequals(type, "sun")){
		return sarrs;
	}
	throw CruncherException("Don't know what to do with type %s", type.c_str());
}

void System::arraynodes(std::string& type, NodesVector& nodes){
	ArraySet& as = getaset(type);
	BOOST_FOREACH(ValuesMap::value_type vi, as.values) {
		Node* nd = vi.first;
		nodes.push_back(NodeDesc(nd->parent->name, nd->num));
	}
}

void System::setnodes(std::string& tp, NodesVector& nodes){
	NodeSetMap::const_iterator nit = nodesets.find(tp);
	if(nit == nodesets.end()){
		throw CruncherException("Don't know what to do with type %s", tp.c_str());
	}
	NodeSet* ns = nit->second;
	BOOST_FOREACH(NSet::value_type nd, ns->set) {
		nodes.push_back(NodeDesc(nd->parent->name, nd->num));
	}
}

void System::modconns(ModsVector& vect){
	BOOST_FOREACH(ModuleMap::value_type mm, collection.modules) {
		Module& mod = mm.second;
		ModStatsMap msm = ModStatsMap();
		BOOST_FOREACH(NodeMap::value_type nm, mod.nodes) {
			Node* fnd = nm.second;
			BOOST_FOREACH(ConductMap::value_type cm, fnd->lins) {
				Node* tnd = cm.first;
				IndexPair& counts = msm[tnd->parent];
				counts.first += 1;
			}
			BOOST_FOREACH(ConductMap::value_type cm, fnd->rads) {
				Node* tnd = cm.first;
				IndexPair& counts = msm[tnd->parent];
				counts.second += 1;
			}
		}
		BOOST_FOREACH(ModStatsMap::value_type st, msm) {
			ModConnects mc = ModConnects();
			mc.fr = mod.name;
			mc.to = st.first->name;
			mc.lins = st.second.first;
			mc.rads = st.second.second;
			vect.push_back(mc);
		}
	}
}

void System::consvals(std::string& tp, std::string& fname, std::string& tname, size_t size, LAllocator la, DAllocator da, void* f){
	ModuleMap::const_iterator it = collection.modules.find(fname);
	if(it == collection.modules.end()){
		throw CruncherException("module '%s' does not exist", fname.c_str());
	}
	const Module& fmod = it->second;
	it = collection.modules.find(tname);
	if(it == collection.modules.end()){
		throw CruncherException("module '%s' does not exist", tname.c_str());
	}
	bool linear;
	if (boost::iequals(tp, "lins")){
		linear = true;
	} else if (boost::iequals(tp, "rads")){
		linear = false;
	} else {
		throw CruncherException("Expected 'lins' or 'rads', got '%s'", tp.c_str());
	}
	const Module& tmod = it->second;
	if(size <= 0) return;
	long pos = 0;
	long* froms = la(size, f);
	long* tos = la(size, f);
	double* cons = da(size, f);
	BOOST_FOREACH(NodeMap::value_type nit, fmod.nodes) {
		Node& fnd = *nit.second;
		ConductMap* cm = linear? &fnd.lins : &fnd.rads;
		BOOST_FOREACH(ConductMap::value_type cit, *cm) {
			Node& tnd = *cit.first;
			if(tnd.parent != &tmod) continue;
			froms[pos] = fnd.num;
			tos[pos] = tnd.num;
			cons[pos] = cit.second;
			pos += 1;
		}
	}
}

void System::appox(std::string& mod, long num, WeightedNodesVector& dvec){
	BOOST_FOREACH(ApproxMap::value_type aid, approxes) {
		Node* xnd = aid.first;
		if(xnd->parent->name.compare(mod) != 0) continue;
		if(xnd->num != num) continue;
		ApproxVector& avec = aid.second;
		BOOST_FOREACH(ApproxVector::value_type aid, avec) {
			Node* nd = aid.first;
			WeightedNode wnd = {nd->parent->name, nd->num, aid.second};
			dvec.push_back(wnd);
		}
		return;
	}
	/* not found */
}

static void duparray(const Array* array, DAllocator allocator, void* f){
	double* times = allocator(array->count, f);
	if(!times){
		throw CruncherException("failed to allocate %ld doubles", array->count);
	}
	memcpy(times, array->vals, sizeof(times[0])*array->count);
}

static double pullvalues(ModuleMap& mmap, ValuesMap& vmap, std::string& mod, long num, DAllocator allocator, void* f){
	const NodeMap& nmap = getnodemap(mmap, mod);
	NodeMap::const_iterator nit = nmap.find(num);
	if(nit == nmap.end()){
		throw CruncherException("node %ld does not exist in %s", num, mod.c_str());
	}
	Node* node = nit->second;
	ValuesMap::iterator vit = vmap.find(node);
	if(vit == vmap.end()){
		throw CruncherException("no values for %s.%ld", node->parent->name.c_str(), node->num);
	}
	NodeValues& nodevals = vit->second;
	duparray(nodevals.times, allocator, f);
	duparray(nodevals.vals, allocator, f);
	return nodevals.mult;
}

double System::pullarray(std::string& type, std::string& mod, long num, DAllocator allocator, void* f){
	ArraySet& as = getaset(type);
	return pullvalues(collection.modules, as.values, mod, num, allocator, f);
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
	if(uset.set.size()){
		printf("  undefined:\n");
		uset.show("        ");
	}
	printf("Total conducts: %ld\n", total_conducts);
	printf("     linear: %ld\n", total_lins);
	printf("       rads: %ld\n", total_rads);
	printf("    one-way: %ld\n", oset.set.size());
	printf("    approx.: %ld\n", kset.set.size());
	printf("  undefined: %ld\n", uset.set.size());
	std::stringstream ss;
	if((long)confcons.size() != conflicts){
		ss << " (" << conflicts-confcons.size() << " more than once)";
	}
	printf("  conflicts: %ld%s\n", conflicts, ss.str().c_str());
	printf("\n");
	printf("==========\n");
	printf("  %ld arrays\n", amap.size());
	printf("\n");
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
		throw CruncherException("%s line %d\n %s", curname.c_str(), flines, e.what());
	}
	std::cout << "  number of lines: " << count << std::endl;
}
