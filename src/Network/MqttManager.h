#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <WiFi.h>
#include <PubSubClient.h>
#include "Config.h"
#include "HAL/Actuators.h"
#include "Logic/PayloadBuilder.h"

class MqttManager {
public:
    void init();
    void update();
    bool isConnected();
    
    void publishTelemetry(const SensorData& data);
    void publishState(const String& powerState);
    void publishAck(const String& controlId, const String& status);

private:
    WiFiClient espClient;
    PubSubClient mqttClient;
    
    unsigned long lastReconnectAttempt = 0;
    
    void setupWiFi();
    bool reconnectMqtt();
    static void mqttCallback(char* topic, byte* payload, unsigned int length);
};

extern MqttManager mqttManager;

#endif // MQTT_MANAGER_H
