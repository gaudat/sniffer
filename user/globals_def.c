/*
 * globals_def.c
 *
 *  Created on: 9 Jan 2018
 *      Author: gaudat
 */


#include "globals.h"

bool is_autonomous;
bool change_channels_automatically;

void initialize_globals() {
	is_autonomous = false;
	change_channels_automatically = true;
}
