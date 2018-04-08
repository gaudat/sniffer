
extern "C" {
#include <user_interface.h>
}
#include "uart.h"
#include "gpio.h"
#include "globals.h"
#include "serial_writer.h"

mac_address sw_buffer[SW_BUFFER_SIZE];
u32 sw_lastseen[SW_BUFFER_SIZE];

void serial_writer() {
	// Send one mac address on Serial1, send everything from the oldest if all mac addresses are sent
	mac_address* sw_buf_this = &sw_buffer[sw_read_loc];
	if (sw_updated) {
		// write > read, catch up then set sw_updated = false
		if (sw_read_loc == sw_write_loc) sw_updated = false;
		// read >= write, overflow catch up
		Serial1.printf("N%02x%02x%02x%02x%02x%02x %010u\r\n", sw_buf_this->addr[0], sw_buf_this->addr[1], sw_buf_this->addr[2],
				sw_buf_this->addr[3], sw_buf_this->addr[4], sw_buf_this->addr[5], sw_lastseen[sw_read_loc]);

	} else {
		// write >= read, continue
		// read > write, continue

		Serial1.printf("R%02x%02x%02x%02x%02x%02x %010u\r\n", sw_buf_this->addr[0], sw_buf_this->addr[1], sw_buf_this->addr[2],
				sw_buf_this->addr[3], sw_buf_this->addr[4], sw_buf_this->addr[5], sw_lastseen[sw_read_loc]);

	}
	sw_read_loc++;
	if (sw_read_loc >= SW_BUFFER_SIZE) sw_read_loc = 0;
}
