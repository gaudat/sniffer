/*
 * globals.h
 *
 *  Created on: 9 Jan 2018
 *      Author: gaudat
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <Arduino.h>
#include "SD.h"
extern "C" {
#include <user_interface.h>
}
/**
 * LED is connected to GPIO2.
 */
#define LED_GPIO GPIO_Pin_2
#define LED_OFF 1
#define LED_ON 0

#define chip_select 15

/**
 * Call this function in user_init to set all globals to a defined initial value.
 */
extern void initialize_globals();

/**
 * Set to true if the sniffer is running completely autonomous.
 * The GPIO0 button is used to toggle this bool.
 * If the sniffer is not in autonomous mode, the LED flashes whenever a MAC header is received.
 * If the sniffer is in autonomous mode, the LED stays off.
 */
extern bool is_autonomous;


/**
 * Sniffer control variables.
 */

/**
 * Set to true if the sniffer is in promiscuous mode and is capturing packets.
 * Set to false if the sniffer is in AP mode and is waiting for a connection to download captured packets.
 */
extern bool is_capturing;

extern uint32_t sniff_types_mask_32;
extern uint32_t sniff_types_mask_10;

extern bool sniffer_write_to_sd;

extern File sniffer_log;

extern unsigned int sniffer_flush_interval;

extern bool sniffer_drop_more;

extern int channel_counted_frames;

extern int channel_hop_delay[14];

extern os_timer_t channel_hopper_timer;

#endif /* GLOBALS_H_ */
