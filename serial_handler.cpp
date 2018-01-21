/*
 * serial_handler.c
 *
 *  Created on: 9 Jan 2018
 *      Author: gaudat
 */

#include "esp_common.h"
#include "uart.h"
#include "gpio.h"
#include "globals.h"
#include "serial_handler.h"

/**
 * The buffer storing command received from the serial port.
 * The command processing task is only woken up by a newline.
 * The buffer size is taken as 128 byte as it is the same size
 * as the UART receive buffer.
 *
 * command_arg is not zero terminated. the length of the whole
 * command is stored at serial_command_buffer_len.
 */
static struct {
	u8 command;
	u8 command_arg[127];
} serial_command_buffer;
/**
 * The buffer length will be zeroed as soon as possible by the handler task.
 */
static size_t serial_command_buffer_len = 0;

/** This boolean will be polled by the serial command handler thread.
 *  It is set by the interrupt is a newline is detected.
 *  It will be cleared by the handler task when its job is done.
 */
static bool serial_command_handler_work = false;

/**
 * Big S - Start changing channels automatically.
 * As soon as the command is received, channels are changed automatically. This function remains active until an endline is received.
 */
void big_s() {
	change_channels_automatically = true;
	printf("fsm: changing channels automatically\n");
}


/**
 * small s - Stop changing channels automatically.
 * The channel stops changing as soon as the alphabet is received. After receiving the endline, the sniffing wifi channel is set to
 * the integer argument. If it is out of range tell an error.
 */
void small_s() {
	change_channels_automatically = false;

	u8 channel = 0;

	int i;
	// Length of args is total length minus 2, 1 for command 1 for endline
	for (i = 0; i < serial_command_buffer_len - 2; i++) {
		channel *= 10;
		channel += serial_command_buffer.command_arg[i] - '0';
		printf("channel $d i %d %c\n", channel, i, serial_command_buffer.command_arg[i]);
	}

	printf("fsm: waiting s: new channel is %d\n", channel);
	if (channel < 1 || channel > 14) {
		printf("channel: out of range (%d)\n",channel);
	} else {
		wifi_set_channel(channel);
	}
	printf("channel: %d\n", wifi_get_channel());
}

void big_p() {
	u8 channel = 0;

	int i;
	// Length of args is total length minus 2, 1 for command 1 for endline
	for (i = 0; i < serial_command_buffer_len - 2; i++) {
		channel *= 10;
		channel += serial_command_buffer.command_arg[i] - '0';
		printf("channel $d i %d %c\n", channel, i, serial_command_buffer.command_arg[i]);
	}

	if (channel < 0 || channel > 31) {
		printf("type: out of range (%d)\n", channel);
		return;
	}

	sniff_types_mask |= 1 << channel;
	printf("type: current mask: 31:0 %08x\n", sniff_types_mask);
}

void small_p() {
	u8 channel = 0;

	int i;
	// Length of args is total length minus 2, 1 for command 1 for endline
	for (i = 0; i < serial_command_buffer_len - 2; i++) {
		channel *= 10;
		channel += serial_command_buffer.command_arg[i] - '0';
		printf("channel $d i %d %c\n", channel, i, serial_command_buffer.command_arg[i]);
	}

	if (channel < 0 || channel > 31) {
		printf("type: out of range (%d)\n", channel);
		return;
	}

	sniff_types_mask &= ~( 1 << channel);
	printf("type: current mask: 31:0 %08x\n", sniff_types_mask);
}

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
 * Big W - Starts recording all collected MAC frame headers to the MicroSD card from now on.
 * small w - Stops recording MAC frame headers to MicroSD card.
 * Big C - Starts sniffing by putting the chip into promiscuous mode.
 * Small c - Stops sniffing by putting the chip into AP mode.
 */
void serial_handler(void* arg) {
	printf("serial_handler: task started\n");
	while (1) {
		// Poll if the handler should be working
		while (!serial_command_handler_work) {
//			printf("serial_handler: no work\n");
			vTaskDelay(200/portTICK_RATE_MS);
		}

		printf("serial_handler: command %c received\n",serial_command_buffer.command);

		// Check what the command is first
		switch (serial_command_buffer.command) {
		case 'S':
			big_s();
			break;
		case 's':
			small_s();
			break;
		case 'P':
			big_p();
			break;
		case 'p':
			small_p();
			break;
			//		case 'W':
			//			big_w();
			//			break;
			//		case 'w':
			//			small_w();
			//			break;
			//		case 'U':
			//			big_u();
			//			break;
			//		case 'u':
			//			small_u();
			//			break;
			//		case 'C':
			//			big_c();
			//			break;
			//		case 'c':
			//			small_c();
			//			break;
		default:
			printf("unknown command %c\n", serial_command_buffer.command);
			break;
		}

		// The handler has finished its job
		printf("serial_handler: back from handler\n");
		serial_command_handler_work = false;
		serial_command_buffer_len = 0;
	}
	vTaskDelete(NULL);
}

void serial_intr_handler(void* arg) {
	u8 serial_intr_len = ((READ_PERI_REG(UART_STATUS(UART0))>>UART_RXFIFO_CNT_S)&UART_RXFIFO_CNT);
	printf("serial_intr: called %d\n", serial_intr_len);
	//	printf("serial_handler: intr'd len %d\n", serial_intr_len);
	if (serial_command_buffer_len >= sizeof(serial_command_buffer)) {
		// Prevent buffer overflow, ignore the incoming character
		// if the command buffer is full.
		printf("serial_handler: command too long\n");
		// Clear FIFO. The interrupt can only be cleared by doing this.
	} else {
		while (serial_intr_len-->0 && !serial_command_handler_work) {
			((char*)&serial_command_buffer)[serial_command_buffer_len++] = READ_PERI_REG(UART_FIFO(UART0));
			// Check if the received character is newline. IF it is, tell the cmomand processing function to do its work.
			// The command function is not directly called to minimize time spent in interrupt.
			char last_ch = ((char*)&serial_command_buffer)[serial_command_buffer_len-1];
			if (last_ch == '\r' || last_ch == '\n') {
				printf("serial_handler: sending command %c arg len %d\n", serial_command_buffer.command, serial_command_buffer_len);
				serial_command_handler_work = true;
			}
		}
	}
	serial_intr_len = ((READ_PERI_REG(UART_STATUS(UART0))>>UART_RXFIFO_CNT_S)&UART_RXFIFO_CNT);
	if (serial_intr_len > 0) {
		while (serial_intr_len-->0) {
			volatile u8 c = READ_PERI_REG(UART_FIFO(UART0));
			WRITE_PERI_REG(UART_INT_CLR(UART0), UART_RXFIFO_FULL_INT_CLR);
		}
	}
	// Clear the interrupt
	WRITE_PERI_REG(UART_INT_CLR(UART0), UART_RXFIFO_FULL_INT_CLR);
}
