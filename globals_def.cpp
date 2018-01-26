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
bool skip_quiet_channels;
int beacon_scan_interval;

void initialize_globals() {
	is_autonomous = true;
	is_capturing = true;
	sniff_types_mask_32 = ~0;
	sniff_types_mask_10 = ~0;
	sniffer_write_to_sd = true;
	sniffer_flush_interval = 16;
	sniffer_drop_more = true;
	channel_counted_frames = 0;
	skip_quiet_channels = false;
	for (int i=0; i<14; i++) {
		channel_hop_delay[i] = 200;
	}
	beacon_scan_interval = 300000; // 5 minutes
}
