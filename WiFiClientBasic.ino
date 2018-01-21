extern "C" {
  #include <user_interface.h>
}

#include "private.h"
#include "globals.h"
#include "serial_handler.h"

#include <TaskScheduler.h>

Scheduler runner;

Task serial_handler_task(200, TASK_FOREVER, &serial_handler);

void setup() {
  // set the WiFi chip to "promiscuous" mode aka monitor mode
  Serial.begin(921600);
  Serial.setTimeout(1);
  delay(10);

  initialize_globals();

  runner.init();
  runner.addTask(serial_handler_task);
  serial_handler_task.enable();

//  wifi_set_opmode(STATION_MODE);
//  wifi_set_channel(1);
//  wifi_promiscuous_enable(0);
//  delay(10);
//  wifi_set_promiscuous_rx_cb(sniffer_callback);
//  delay(10);
//  wifi_promiscuous_enable(1);
  pinMode(2, OUTPUT);
}

void loop() {
  // Serial handler
	if (Serial.available() > 0) {
		serial_intr_handler(NULL);
	}
	runner.execute();
}

