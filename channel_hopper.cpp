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
#include "sniffer.h"

static bool has_beacon[14] = {true}; // If the channel has beacon frames
static bool scan_new_beacons = true;

static int beacon_frames;

static void beacon_promisc_rx_cb(uint8_t* buf, uint16_t len) {
	(void)len;
	// Next is the MAC header, the part we are most interested in
	struct ieee80211_hdr* hdr = (struct ieee80211_hdr*)((u8*)buf + sizeof(struct RxControl));
	if (hdr->frame_control.type == 0 && hdr->frame_control.subtype == 8) {
		// This is actually a beacon
		beacon_frames++;
		printf("beacon: detected %d\r\n", beacon_frames);
	}
}

/**
 * Scan all channels by staying in each channel for 200ms. If the channel has beacon frames,
 * that means, it have potentially interesting traffic, record the channel in has_beacon.
 */
void beacon_scanner(void* arg) {
	(void)arg;
	scan_new_beacons = true;
	printf("beacon: disabling channel hopper\r\n");
	os_timer_disarm(&channel_hopper_timer);
	for (int i=0; i<14; i++) {
		has_beacon[i] = false;
		// Clear all channels
	}
	printf("beacon: switching to new cb\r\n");
	wifi_promiscuous_enable(0);
	wifi_set_promiscuous_rx_cb(beacon_promisc_rx_cb);
	wifi_promiscuous_enable(1);
	for (int i=0; i<14; i++) {
		wifi_set_channel(i);
		beacon_frames = 0;
		delay(200);
		printf("beacon: CH%02d B%d\r\n", i, beacon_frames);
		if (beacon_frames > 0) has_beacon[i] = true;
	}
	// revert to the original rx_cb
	printf("beacon: switching to old cb\r\n");
	wifi_promiscuous_enable(0);
	wifi_set_promiscuous_rx_cb(promiscuous_rx_cb);
	wifi_promiscuous_enable(1);
}

void channel_hopper(void* arg) {
	(void)arg;
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

		// Skip quiet channels if it is enabled
		do {
			cur_channel++;
			if (cur_channel > 14) cur_channel = 1;
			printf("hopper: ch %d is active %d\r\n", cur_channel, has_beacon[cur_channel]);
		} while (!has_beacon[cur_channel] && skip_quiet_channels);

		printf("hopper: switching to ch %d\r\n", cur_channel);
		wifi_set_channel(cur_channel);
		ticks_remaining = channel_hop_delay[cur_channel] / 10;
		printf("hopper: waiting for %d0 ms\r\n", ticks_remaining);
	} else {
		ticks_remaining--;
	}
}
