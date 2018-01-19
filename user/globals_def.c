/*
 * globals_def.c
 *
 *  Created on: 9 Jan 2018
 *      Author: gaudat
 */


#include "globals.h"

bool is_autonomous;
bool change_channels_automatically;
u32 sniff_types_mask;

void initialize_globals() {
	is_autonomous = false;
	change_channels_automatically = true;
	sniff_types_mask = ~0;
}
