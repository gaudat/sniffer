/*
 * serial_handler.c
 *
 *  Created on: 9 Jan 2018
 *      Author: gaudat
 */

#include "esp_common.h"
#include "uart.h"
#include "globals.h"

/**
 * Serial handler task function. Listens on UART0 for commands, modifies global variables to change sniffer working mode.
 * @param arg nothing
 */

static const int CONTROL_UART = 0; // Which UART to listen on

void fsm_error();
void waiting_alpha();
void waiting_big_s();
void waiting_small_s();
void waiting_big_p() {}
void waiting_small_p() {}
void waiting_big_w() {}
void waiting_small_w() {}
void waiting_big_u() {}
void waiting_small_u() {}
void waiting_big_c() {}
void waiting_small_c() {}
void waiting_small_t() {}

typedef void (*state_func)();
state_func state;

void fsm_error() {
	printf("fsm: error\n");
	char c;
	//scanf("%c",&c);
	printf("fsm: received %d\n", c);
	switch (c) {
	case '\r':
	case '\n':
	state = waiting_alpha;
	break;
	}
}

void waiting_alpha() {
	printf("fsm: waiting_alpha\n");
	char c;
	//scanf("%c",&c);
	printf("fsm: received %d\n", c);
	switch (c) {
	case 'S':
		state = waiting_big_s;
		break;
	case 's':
		state = waiting_small_s;
		break;
	case 'P':
		state = waiting_big_p;
		break;
	case 'p':
		state = waiting_small_p;
		break;
	case 'W':
		state = waiting_big_w;
		break;
	case 'w':
		state = waiting_small_w;
		break;
	case 'U':
		state = waiting_big_u;
		break;
	case 'u':
		state = waiting_small_u;
		break;
	case 'C':
		state = waiting_big_c;
		break;
	case 'c':
		state = waiting_small_c;
		break;
	case 't':
		state = waiting_small_t;
		break;
	case '\r':
	case '\n':
		state = waiting_alpha;
		break;
	default:
		state = fsm_error;
		break;
	}
}

/**
 * Big S - Start changing channels automatically.
 * As soon as the command is received, channels are changed automatically. This function remains active until an endline is received.
 */
void waiting_big_s() {
	printf("fsm: waiting_big_s\n");
	change_channels_automatically = true;

	char c;
	//scanf("%c",&c);
	while (!(c == '\n' || c == '\r')) {
		printf("waiting_eol: received %d\n", c);
		//scanf("%c",&c);
	}
	state = waiting_alpha;
}


/**
 * small s - Stop changing channels automatically.
 * The channel stops changing as soon as the alphabet is received. After receiving the endline, the sniffing wifi channel is set to
 * the integer argument. If it is out of range tell an error.
 */
void waiting_small_s() {
	printf("fsm: waiting s\n");
	change_channels_automatically = false;
	char c;
	//scanf("%c",&c);
	int channel = 0;
	while (!(c == '\n' || c == '\r')) {
		if (c >= '0' && c <= '9') {
			printf("fsm: entered %c\n",c);
			channel *= 10;
			channel += c-'0';
		}
		//scanf("%c",&c);
	}
	printf("fsm: waiting s: new channel is %d\n", channel);
	if (channel < 1 || channel > 14) {
		printf("channel: out of range (%d)\n",channel);
	} else {
	wifi_set_channel(channel);
	}
	printf("channel: %d\n", wifi_get_channel());
	state = waiting_alpha;
}

void serial_handler(void* arg) {
	/**
	 * This task implements a finite-state machine for parsing command line control inputs.
	 *
	 * The command consist of a capital or small letter followed by zero or more digits.
	 * The command ends with a newline (\n).
	 *
	 * The default state is waiting_alpha, which waits for an alphabet character.
	 * Next the FSM will move to waiting_[alphabet] state, which reads all numbers until a newline then performs an action.
	 * After that the FSM will move back to waiting_alpha.
	 * If more than one alphabet is received, the FSM will go into an error state, which turns into waiting_alpha at the next newline.
	 *
	 * Current commands are:
	 *
	 * Big S - Start changing channels automatically (smartly).
	 * small s - Stop changing channels automatically and change to the specified channel. If it is not given stay at the last channel.
	 * Big P - Keep the specified kind of MAC frame headers. The number is between 0 to 31, 16*frame_type+frame_subtype.
	 * small p - Filter the specified kind of MAC frame headers. The number is same as the Big P command.
	 *
	 * small t - Sends the current RTC time of the sniffer in microseconds.
	 *
	 * Big W - Starts recording all collected MAC frame headers to the MicroSD card from now on.
	 * small w - Stops recording MAC frame headers to MicroSD card.
	 * Big U - Starts writing filtered MAC frame headers to UART0.
	 * small u - Stops writing filtered MAC frame headers to UART0.
	 * Big C - Starts sniffing by putting the chip into promiscuous mode.
	 * Small c- Stops sniffing by putting the chip into AP mode.
	 */

	printf("serial_handler: started\n");
	state = waiting_alpha;

	// Clean the input buffer first
	UART_ResetFifo(0);

	while (1) {
		state();
		vTaskDelay(10);
	}

	vTaskDelete(NULL);
}

//TODO: Rewrite as serial interrupt handler
