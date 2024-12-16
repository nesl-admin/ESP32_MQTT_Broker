#include <PicoMQTT.h>
#include <EthernetLarge.h>
#include <config.h>

#ifndef WIFI_SSID
#define WIFI_SSID "Starlink_2GEXT"
#endif

#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "tikka2013"
#endif

//instantiate the correct server (WiFi or Ethernet)
#ifdef ENABLE_WIFI
PicoMQTT::Server mqtt_broker;
#else
EthernetServer ethernet_server(1883);
PicoMQTT::Server mqtt_broker(ethernet_server);
#endif

unsigned long last_publish_time = 0;
int greeting_number = 1;
String default_topic;

// Another approach is to have a single callback function that looks at the topic and routes
// accordingly. If subs are constantly logging energy usage stats we may want them funneled into a
std::function<void(char *, char *)> in_sub1_processor = [](char* a, char* b) {
    Serial.printf("Received message from SUB1 '%s': %s\n", a, b);
    return;
};
std::function<void(char *, char *)> in_sub2_processor = [](char* a, char* b) {
    Serial.printf("Received message from SUB2 '%s': %s\n", a, b);
    return;
};
std::function<void(char *, char *)> in_sub3_processor = [](char* a, char* b) {
    Serial.printf("Received message from SUB3 '%s': %s\n", a, b);
    return;
};

void setup_mqtt_broker() {
    default_topic = "NESL";//getUniqueID();
    //default_topic.concat("/esp32-");
    //default_topic.concat(WiFi.macAddress());

    // Connect to WiFi
#ifdef ENABLE_WIFI
    Serial.printf("Connecting to WiFi %s\n", WIFI_SSID);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) { delay(1000); }
    Serial.printf("WiFi connected, IP: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("Default topic: %s\n", default_topic.c_str());
#endif

    // Subscribe to a topic and attach a callback
    mqtt_broker.subscribe("energy_sub1", in_sub1_processor);
    mqtt_broker.subscribe("energy_sub2", in_sub2_processor);
    mqtt_broker.subscribe("energy_sub3", in_sub3_processor);

    mqtt_broker.begin();
}

void loop_mqtt_broker() {
    mqtt_broker.loop();

    // Publish a greeting message every 10 seconds.
    if (millis() - last_publish_time >= 10000) {
        // We're publishing to a topic, which we're subscribed too, but these message will *not* be delivered locally.
        String message = "Hello #" + String(greeting_number++);
        Serial.printf("Publishing message in topic '%s': %s\n", default_topic.c_str(), message.c_str());
        mqtt_broker.publish(default_topic, message);
        last_publish_time = millis();
    }
}
