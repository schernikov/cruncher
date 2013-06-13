/*
 * system.cpp
 *
 *  Created on: Jun 12, 2013
 *      Author: schernikov
 */

#include "system.h"

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
