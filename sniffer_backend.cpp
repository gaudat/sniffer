/*
 * sniffer_backend.c
 *
 *  Created on: 3 Jan 2018
 *      Author: m2
 */

#include "globals.h"
#include "sniffer.h"
#include "SD.h"

/**
 * Wi-Fi sniffer backend task function.
 *
 * @param arg Struct for communicating between other tasks and this one
 */


static const int line_buffer_size = 128;
static int line_buffer_pos = 0;
static char line_buffer[line_buffer_size];

typedef struct mac_address {
	u8 addr[6];
	mac_address* next;
} mac_address;

static mac_address* aps = NULL;

void promiscuous_rx_cb(uint8_t* buf, uint16_t len) {
	(void)len;

	static unsigned int i = 0;
	// len is at most 128, any packet longer than that gets truncated

	// First in buf is RxControl from the PHY of ESP8266
	struct RxControl *rx_ctrl = (struct RxControl *)buf;
	// Next is the MAC header, the part we are most interested in
	struct ieee80211_hdr* hdr = (struct ieee80211_hdr*)((u8*)buf + sizeof(struct RxControl));

	line_buffer_pos = 0;

	// Count the number of frames detected to determing if the channel is busy
	channel_counted_frames++; // not actually beacon

	if (!digitalRead(0)) {
		/**
		 * If too many packets appear at once, writing to SD card will take so much time
		 * that commands from the serial port do not get to be processed.
		 */
		digitalWrite(2, channel_counted_frames%2); // Blink on each alternating frame
		printf("sniffer: bypassed as flash btn is pressed\r\n");
		return;
	}

	// Preliminary frame filters incoming
	// If the frame originates from the AP ignore it, as the RSSI is not useful to tracking user devices.
	bool frame_filtered = false; // Should this frame be filtered?
	mac_address* li = aps;
	while (li != NULL) {
		if (memcmp(li->addr, hdr->addr2, 6*sizeof(u8)) == 0) {
			// The source address matches one of the APs
			frame_filtered = true;
			if (sniffer_drop_more) return;
		}
		if (li->next == NULL) break;
		li = li->next;
	}
	// li should be null now
	// If it is a beacon packet, add the originating MAC address to the filter list
	if (hdr->frame_control.type == 0 && hdr->frame_control.subtype == 8 && !frame_filtered) {
		// It is a beacon frame
		// Add to end of li
		mac_address* li_new = new mac_address;
		memcpy(li_new->addr, hdr->addr2, 6*sizeof(u8));
		li_new->next = NULL;
		if (aps != NULL) {
			li->next = li_new;
		} else {
			aps = li_new;
		}
		printf("%010lu AP%02x%02x%02x%02x%02x%02x\r\n", micros(),
				li_new->addr[0], li_new->addr[1], li_new->addr[2],
				li_new->addr[3], li_new->addr[4], li_new->addr[5]);
		// Still drop the beacon frame
		if (sniffer_drop_more) return;
	}

	// Preliminary frame filters end

	// filter packet types
	if (hdr->frame_control.type - 2 >= 0) {
		// It is either 3 or 2
		if (!(sniff_types_mask_32 >> ((hdr->frame_control.type-2)*16+hdr->frame_control.subtype) & 0x1)) return;
	} else {
		// It is either 1 or 0
		if (!(sniff_types_mask_10 >> ((hdr->frame_control.type)*16+hdr->frame_control.subtype) & 0x1)) return;
	}

	line_buffer_pos += snprintf(line_buffer+line_buffer_pos, line_buffer_size-line_buffer_pos,
			"%010lu ", micros());

	line_buffer_pos += snprintf(line_buffer+line_buffer_pos, line_buffer_size-line_buffer_pos,
			"CH%02d RI%02d ", rx_ctrl->channel, rx_ctrl->rssi);

	line_buffer_pos += snprintf(line_buffer+line_buffer_pos, line_buffer_size-line_buffer_pos,
			"%x%x ", hdr->frame_control.type, hdr->frame_control.subtype);

	line_buffer_pos += snprintf(line_buffer+line_buffer_pos, line_buffer_size-line_buffer_pos,
			"%x%x ", hdr->frame_control.to_ds, hdr->frame_control.from_ds);

	line_buffer_pos += snprintf(line_buffer+line_buffer_pos, line_buffer_size-line_buffer_pos,
			"%02x%02x%02x%02x%02x%02x<", hdr->addr1[0], hdr->addr1[1], hdr->addr1[2],
			hdr->addr1[3], hdr->addr1[4], hdr->addr1[5]);

	line_buffer_pos += snprintf(line_buffer+line_buffer_pos, line_buffer_size-line_buffer_pos,
			"%02x%02x%02x%02x%02x%02x:", hdr->addr2[0], hdr->addr2[1], hdr->addr2[2],
			hdr->addr2[3], hdr->addr2[4], hdr->addr2[5]);

	line_buffer_pos += snprintf(line_buffer+line_buffer_pos, line_buffer_size-line_buffer_pos,
			"%02x%02x%02x%02x%02x%02x-", hdr->addr3[0], hdr->addr3[1], hdr->addr3[2],
			hdr->addr3[3], hdr->addr3[4], hdr->addr3[5]);

	line_buffer_pos += snprintf(line_buffer+line_buffer_pos, line_buffer_size-line_buffer_pos,
			"%02x%02x%02x%02x%02x%02x\r\n", hdr->addr4[0], hdr->addr4[1], hdr->addr4[2],
			hdr->addr4[3], hdr->addr4[4], hdr->addr4[5]);

	int li_len = 0;
	li = aps;
	while (li != NULL) {
		li = li->next;
		li_len++;
	}
	//	printf("%d AP%d\r\n", line_buffer_pos, li_len);
	printf("%d %s", line_buffer_pos, line_buffer);

	/**
	 * Only sync to SD card every once in a while.
	 */
	if (sniffer_write_to_sd) {
		if (!sniffer_log) {
			// Either the log is closed or it is never opened.
			sniffer_log = SD.open("sniffer.log", FILE_WRITE);
			if (!sniffer_log) {
				Serial.println("sniffer: error opening sniffer.log");
			}
		}
		sniffer_log.print(line_buffer);
		if (++i % sniffer_flush_interval == 0) {
			// Only close the sniffer_log and sync the files every 16 lines.
			//			Serial.println("sniffer: closing file once a while");
			sniffer_log.close();
		}

	} else {
		// The sniffer is not writing to SD card now, but itstill needs to sync the written lines.
		if (sniffer_log) {
			sniffer_log.close();
		}
	}

	// Turn off LED
	digitalWrite(2, 1);
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

