extern "C" {
  #include <user_interface.h>
}

#include "private.h"

void setup() {
  // set the WiFi chip to "promiscuous" mode aka monitor mode
  Serial.begin(921600);
  Serial.setTimeout(1);
  delay(10);
  wifi_set_opmode(STATION_MODE);
  wifi_set_channel(1);
  wifi_promiscuous_enable(0);
  delay(10);
  wifi_set_promiscuous_rx_cb(sniffer_callback);
  delay(10);
  wifi_promiscuous_enable(1);
  pinMode(2, OUTPUT);
}

void loop() {
  // Serial handler
	while (Serial.available() <= 0) {
		delay(10);
	}
	while (Serial.available() > 0) {
		digitalWrite(2, 0);
		delay(10);
		digitalWrite(2, 1);
		Serial.write(Serial.read());
	}
}

