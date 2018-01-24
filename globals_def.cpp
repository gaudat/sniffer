/*
 * globals_def.c
 *
 *  Created on: 9 Jan 2018
 *      Author: gaudat
 */


#include "globals.h"

bool is_autonomous;
bool is_capturing;
uint32_t sniff_types_mask_32;
uint32_t sniff_types_mask_10;

bool sniffer_write_to_sd;
File sniffer_log;

unsigned int sniffer_flush_interval;
bool sniffer_drop_more;
int channel_counted_frames;
int channel_hop_delay[14];
os_timer_t channel_hopper_timer;

void initialize_globals() {
	is_autonomous = false;
	is_capturing = true;
	sniff_types_mask_32 = ~0;
	sniff_types_mask_10 = ~0;
	sniffer_write_to_sd = false;
	sniffer_flush_interval = 16;
	sniffer_drop_more = false;
	channel_counted_frames = 0;
	for (int i=0; i<14; i++) {
		channel_hop_delay[i] = 200;
	}
}
