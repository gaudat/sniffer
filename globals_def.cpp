/*
 * globals_def.c
 *
 *  Created on: 9 Jan 2018
 *      Author: gaudat
 */


#include "globals.h"

bool is_autonomous;
bool change_channels_automatically;

bool is_capturing;
uint32_t sniff_types_mask;

void initialize_globals() {
	is_autonomous = false;
	change_channels_automatically = true;
	is_capturing = true;
	sniff_types_mask = ~0;
}
