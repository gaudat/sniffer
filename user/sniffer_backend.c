/*
 * sniffer_backend.c
 *
 *  Created on: 3 Jan 2018
 *      Author: m2
 */

#include "sniffer_backend.h"

#include "esp_common.h"

#include "globals.h"

/**
 * Wi-Fi sniffer backend task function.
 *
 * @param arg Struct for communicating between other tasks and this one
 */


void promiscuous_rx_cb(uint8_t* buf, uint16_t len) {
	printf("buf%p len%d",buf,len);
}


void sniffer_backend(void* arg) {
	// Promiscous mode can only be enabled in STATION_MODE
	wifi_promiscuous_enable(1);
	wifi_set_promiscuous_rx_cb(promiscuous_rx_cb);
	wifi_set_opmode(STATION_MODE);
	wifi_set_channel(11);

	vTaskDelete(NULL);
}
