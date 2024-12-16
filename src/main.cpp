#include <Arduino.h>
#include <mqtt_broker.h>

void setup() {
  Serial.begin(115200);
  setup_mqtt_broker();
}
void loop() {
  loop_mqtt_broker();
}