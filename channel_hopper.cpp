/*
 * channel_hopper.cpp
 *
 *  Created on: 25 Jan 2018
 *      Author: m2
 */

extern "C" {
#include <user_interface.h>
}
#include "globals.h"

void channel_hopper(void* arg) {
	// This function is called every 10ms by os_timer if channel switching is active.

	// Look for beacon frames detected, if no beacon frames in this channel, only listen for 200ms.
			// Each beacon frame adds 50ms to the dwell time up to a maximum of 2000ms.
			// As a result, channels with no APs, meaning there is only very little traffic.
			// These channels will be scanned quickly.
			// In contrast, channels with many APs and many beacon frames will add to the dwell time a lot.
			// These channels will soon be stayed with the maximum dwell time of 2s, capturing
			// the most traffic in these channels.

	u8 cur_channel = wifi_get_channel();
	static int ticks_remaining;
	if (ticks_remaining <= 0) {
		printf("hopper: time's up! ch %d beacons %d\r\n", cur_channel, channel_counted_frames);
		// Time is up, switch to another channel
		int channel_next_time = 50 + channel_counted_frames * 50;
		if (channel_next_time > 2000) channel_next_time = 2000;
		printf("hopper: next time ch %d gets %d ms\r\n", cur_channel, channel_next_time);
		channel_hop_delay[cur_channel] = channel_next_time;
		channel_counted_frames = 0;
		cur_channel++;
		if (cur_channel > 14) cur_channel = 1;
		printf("hopper: switching to ch %d\r\n", cur_channel);
		wifi_set_channel(cur_channel);
		ticks_remaining = channel_hop_delay[cur_channel] / 10;
		printf("hopper: waiting for %d0 ms\r\n", ticks_remaining);
	} else {
		ticks_remaining--;
	}
}
