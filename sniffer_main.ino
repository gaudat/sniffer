extern "C" {
#include <user_interface.h>
}

#include "globals.h"
#include "serial_handler.h"
#include "serial_writer.h"
#include "channel_hopper.h"
#include "sniffer.h"
#include <TaskScheduler.h>

#include <SD.h>

Scheduler runner;

Task serial_handler_task(100, TASK_FOREVER, &serial_handler);
Task serial_writer_task(100, TASK_FOREVER, &serial_writer);

void setup() {
	// set the WiFi chip to "promiscuous" mode aka monitor mode
	Serial.begin(921600);
	Serial.setTimeout(1);
	Serial1.begin(38400);
	Serial1.setTimeout(1);
	delay(10);

	initialize_globals();

	runner.init();
	runner.addTask(serial_handler_task);
	runner.addTask(serial_writer_task);
	serial_handler_task.enable();
	serial_writer_task.enable();

	os_timer_disarm(&channel_hopper_timer);
	os_timer_setfn(&channel_hopper_timer, channel_hopper, NULL);
	os_timer_arm(&channel_hopper_timer, 10, true);


	wifi_set_opmode(STATION_MODE);
	wifi_set_channel(1);
	wifi_promiscuous_enable(0);
	delay(10);
	wifi_set_promiscuous_rx_cb(promiscuous_rx_cb);
	delay(10);
	wifi_promiscuous_enable(1);
//	pinMode(2, OUTPUT);

	Serial.print("Initializing SD card...");

	// see if the card is present and can be initialized:
	if (!SD.begin(chip_select)) {
		is_autonomous = false;
		Serial.println("Card failed, or not present");
		// Light up LED to signify this
//		digitalWrite(LED_GPIO,LED_ON);
	} else {
	Serial.println("card initialized.");
	}
}

void loop() {
	runner.execute();
	// Serial handler
	if (Serial.available() > 0) {
		serial_intr_handler(NULL);
	}
}

