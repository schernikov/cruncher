/*
 * system.cpp
 *
 *  Created on: Jun 12, 2013
 *      Author: schernikov
 */

#include <cstdio>
#include <sstream>

#include "common.h"
#include "system.h"

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
	negates = 0;
}

void System::on_title(const char* ttl){
	title = ttl;
}

/* array energy value assigned directly to node */
void System::on_energy(const char* modname, long nodenum, const char* arraymod, long timesarray, long valsarray){
	if(show) printf("  Q[%s.%ld/%ld] => %s.%ld\n", arraymod, timesarray, valsarray, modname, nodenum);
}

/* increment node energy from array with multiplier */
void System::on_energy(const char* modname, long nodenum, double mult, const char* arraymod, long timesarray, long valsarray){
	if(show) printf("  Q[%s.%ld/%ld] => %s.%ld + %.4lf*dt\n", arraymod, timesarray, valsarray, modname, nodenum, mult);
}

/* increment node energy with constant value */
void System::on_energy(const char* modname, long nodenum, double sum){
	if(show) printf("  Q => %s.%ld + %.4lf\n", modname, nodenum, sum);
}

/* assign one node energy to another */
void System::on_energy_assign(const char* frommod, long fromnode, const char* tomod, long tonode){
	if(show) printf("  Q => %s.%ld => %s.%ld\n", frommod, fromnode, tomod, tonode);
}

/* assign array temperature to node */
void System::on_temp_array(const char* modname, long nodenum, const char* arraymod, long arraynum){
	if(show) printf("  T[%s.%ld] => %s.%ld\n", arraymod, arraynum, modname, nodenum);
}

/* assign one node temperature to another */
void System::on_temp_assign(const char* frommod, long fromnode, const char* tomod, long tonode){
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
		/* unknown meaning */
		negates += 1;
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

void System::on_node(const char* mod, long node, double temp, double cap){
	total_nodes += 1;
	if(node > 0 && cap > 0){
		/* diffusion node */
		diffus += 1;
		return;
	}
	if(node > 0 && cap < 0){
		/* arithmetic node; cap = 0 */
		if(show) printf("arithm: %s.%ld T:%.2lf C:%.2lf\n", mod, node, temp, cap);
		arithms += 1;
		return;
	}
	if(node < 0 && cap >= 0){
		/* boundary node; cap = 0 */
		if(show) printf("bounds: %s.%ld T:%.2lf C:%.2lf\n", mod, node, temp, cap);
		bounds += 1;
		node = -node;
		return;
	}
	if(node < 0 && cap < 0){
		/* heater node */
		if(show) printf("heater: %s.%ld T:%.2lf C:%.2lf\n", mod, node, temp, cap);
		heats += 1;
		node = -node;
		cap = -cap;
		return;
	}
	std::stringstream ss;
	ss << "Unexpected node: " << mod << "."<< node << " temp:"<< temp << " cap:"<< cap;
	throw CruncherException(ss.str());
}

void System::on_variable(const char* name, double val){

}

void System::on_array(const char* name, long idx, long count, const double* values){

}

void System::on_node_list(long idx, const char* name, long count, long* nodes){

}

void System::process(){
	printf("\n\n");
	printf("===== %s =====\n", title.c_str());
	printf("Total nodes: %ld\n", total_nodes);
	printf("    diffuse: %ld\n", diffus);
	printf("    arigthm: %ld\n", arithms);
	printf("      bound: %ld\n", bounds);
	printf("      heats: %ld\n", heats);
	printf("Total conducts: %ld\n", total_conducts);
	printf("   linear: %ld\n", total_lins);
	printf("     rads: %ld\n", total_rads);
	printf("  one-way: %ld\n", oneways);
	printf("  negates: %ld\n", negates);
}
