/*
 * globals.h
 *
 *  Created on: 9 Jan 2018
 *      Author: gaudat
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_

/**
 * LED is connected to GPIO2.
 */
#define LED_GPIO GPIO_Pin_2
#define LED_OFF 1
#define LED_ON 0

/**
 * Call this function in user_init to set all globals to a defined initial value.
 */
extern void initialize_globals();

/**
 * Set to true if the sniffer is running completely autonomous.
 * The GPIO0 button is used to toggle this bool.
 * If the sniffer is not in autonomous mode, the LED flashes whenever a MAC header is received.
 * If the sniffer is in autonomous mode, the LED turns off.
 */
extern bool is_autonomous;

/**
 * Set to true if the Wi-Fi channel listening to is changed automatically.
 */
extern bool change_channels_automatically;

/**
 * Sniffer control variables.
 */

/**
 * Set to true if the sniffer is in promiscuous mode and is capturing packets.
 * Set to false if the sniffer is in AP mode and is waiting for a connection to download captured packets.
 */
extern bool is_capturing;

extern uint32_t sniff_types_mask;

void led_on() {
	digitalWrite(2, 0);
}

void led_off() {
	digitalWrite(2, 1);
}

#endif /* GLOBALS_H_ */
