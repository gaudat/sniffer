/*
 * sniffer_backend.c
 *
 *  Created on: 3 Jan 2018
 *      Author: m2
 */

#include "globals.h"
#include "sniffer.h"

/**
 * Wi-Fi sniffer backend task function.
 *
 * @param arg Struct for communicating between other tasks and this one
 */


void promiscuous_rx_cb(uint8_t* buf, uint16_t len) {
	// len is at most 128, any packet longer than that gets truncated

	// First in buf is RxControl from the PHY of ESP8266
	struct RxControl *rx_ctrl = (struct RxControl *)buf;
	// Next is the MAC header, the part we are most interested in
	struct ieee80211_hdr* hdr = (struct ieee80211_hdr*)((u8*)buf + sizeof(struct RxControl));

	// filter packet types
	if (!(sniff_types_mask >> (hdr->frame_control.type*16+hdr->frame_control.subtype)&0x01)) return;

	// Dump to serial port
	printf("%010u ", micros());
	printf("CH%02d RI%02d ", rx_ctrl->channel, rx_ctrl->rssi);

	printf("%x%x ", hdr->frame_control.type, hdr->frame_control.subtype);
	printf("%x%x ", hdr->frame_control.to_ds, hdr->frame_control.from_ds);

	printf("%02x%02x%02x%02x%02x%02x<", hdr->addr1[0], hdr->addr1[1], hdr->addr1[2],
							hdr->addr1[3], hdr->addr1[4], hdr->addr1[5]);
	printf("%02x%02x%02x%02x%02x%02x:", hdr->addr2[0], hdr->addr2[1], hdr->addr2[2],
								hdr->addr2[3], hdr->addr2[4], hdr->addr2[5]);
	printf("%02x%02x%02x%02x%02x%02x-", hdr->addr3[0], hdr->addr3[1], hdr->addr3[2],
								hdr->addr3[3], hdr->addr3[4], hdr->addr3[5]);
	printf("%02x%02x%02x%02x%02x%02x\r\n", hdr->addr4[0], hdr->addr4[1], hdr->addr4[2],
								hdr->addr4[3], hdr->addr4[4], hdr->addr4[5]);
	digitalWrite(2, !digitalRead(2));
}

/*
 * CH06 RSSI-68 10 11 7917a0acffff<ffffd8fdfe3f:100800804483-c8090000a5a5
CH06 RSSI-68 10 11 7917a0acffff<ffffd8fdfe3f:100800804483-c8090000a5a5
CH06 RSSI-65 10 11 7917a0acffff<ffffd8fdfe3f:100800804483-c8090000a5a5
CH06 RSSI-64 18 11 d83dff3f2800<0000d0622440:790800003c4b-0e62004fc300
CH06 RSSI-65 35 11 d83dff3f2800<0000d0622440:1a0100002578-0e2e0c4fc300
CH06 RSSI-64 31 11 783eff3f2800<0080e0772440:79080000454b-0f4e55cfc300
CH06 RSSI-65 31 11 c8f5fe3f2800<0080e0772440:79080000414b-0f1e49cfc300
CH06 RSSI-66 35 11 d83dff3f2800<0000d0622440:1a0100002578-200025300053
CH06 RSSI-66 19 11 c8f5fe3f2800<0080e0772440:790800001e4b-0f6200cfc300
CH06 RSSI-67 19 11 d83dff3f2800<0000d0622440:1a0100004348-326420525353
CH06 RSSI-66 30 11 d83dff3f2800<0000d0622440:1a0100004348-326420525353
CH06 RSSI-65 30 11 d83dff3f2800<0000d0622440:1a0100002578-200025300053
CH06 RSSI-66 2e 11 d83dff3f2800<0000d0622440:1a0100002578-0f6200cfc300
CH06 RSSI-64 1e 11 d83dff3f2800<0000d0622440:1a0100004348-326420525353
CH06 RSSI-63 1d 11 d83dff3f2800<0000d0622440:1a0100004348-326420525353
CH06 RSSI-64 3d 11 d83dff3f2800<0000d0622440:1a0100002578-200025300053
CH06 RSSI-64 35 11 d83dff3f2800<0000d0622440:1a0100004348-326420525353
 */

