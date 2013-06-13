/*
 * system.cpp
 *
 *  Created on: Jun 12, 2013
 *      Author: schernikov
 */

#include "system.h"

void System::on_title(const char* title){

}

/* array energy value assigned directly to node */
void System::on_energy(const char* modname, long nodenum, const char* arraymod, long timesarray, long valsarray){

}

/* increment node energy from array with multiplier */
void System::on_energy(const char* modname, long nodenum, double mult, const char* arraymod, long timesarray, long valsarray){

}

/* increment node energy with constant value */
void System::on_energy(const char* modname, long nodenum, double sum){

}

/* assign one node energy to another */
void System::on_energy_assign(const char* frommod, long fromnode, const char* tomod, long tonode){

}

/* assign array temperature to node */
void System::on_temp_array(const char* modname, long nodenum, const char* arraymod, long arraynum){

}

/* assign one node temperature to another */
void System::on_temp_assign(const char* frommod, long fromnode, const char* tomod, long tonode){

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
}

void System::on_conduct(long idx, const char* fmod, long fnode, const char* tmod, long tnode, double cond){

}

void System::on_node(const char* mod, long node, double temp, double cap){

}

void System::on_variable(const char* name, double val){

}
