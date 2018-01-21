extern "C" {
  #include <user_interface.h>
}
#include "serial_handler.h"

#include "private.h"

void setup() {
  // set the WiFi chip to "promiscuous" mode aka monitor mode
  Serial.begin(115200);
  Serial.setTimeout(1);
  delay(10);
  wifi_set_opmode(STATION_MODE);
  wifi_set_channel(1);
  wifi_promiscuous_enable(0);
  delay(10);
  wifi_set_promiscuous_rx_cb(sniffer_callback);
  delay(10);
  wifi_promiscuous_enable(1);
}

void loop() {
  // Serial handler
	while (Serial.available() <= 0) {
		delay(10);
	}
	while (Serial.available() > 0) {
		serial_handler_do_once();
	}
}

