/*
 * serial_handler.h
 *
 *  Created on: 9 Jan 2018
 *      Author: gaudat
 */

#ifndef SERIAL_HANDLER_H_
#define SERIAL_HANDLER_H_

void serial_handler();

void serial_intr_handler(void* arg);

typedef void (*state_func)();

struct serial_intr_handler_state {
	state_func state;
};

extern struct serial_intr_handler_state serial_intr_handler_global_state;

#endif /* SERIAL_HANDLER_H_ */
