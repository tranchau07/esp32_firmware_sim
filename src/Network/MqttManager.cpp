#include "MqttManager.h"
#include "Logic/AutomationEngine.h"

MqttManager mqttManager;

void MqttManager::init() {
    setupWiFi();
    mqttClient.setClient(espClient);
    mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
    mqttClient.setBufferSize(1024);
    mqttClient.setCallback(mqttCallback);
}

void MqttManager::setupWiFi() {
    Serial.print("Connecting to WiFi ");
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        actuators.setWifiLed(!digitalRead(PIN_LED_WIFI)); // Blink
    }
    Serial.println("\nWiFi connected!");
    actuators.setWifiLed(true); // Solid
}

bool MqttManager::reconnectMqtt() {
    if (WiFi.status() != WL_CONNECTED) {
        actuators.setWifiLed(false);
        setupWiFi();
    }
    
    Serial.print("Attempting MQTT connection...");
    // Last Will and Testament
    String willPayload = PayloadBuilder::buildStatePayload("OFF");
    
    if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD, TOPIC_STATE, 1, true, willPayload.c_str())) {
        Serial.println("connected");
        actuators.setMqttLed(true);
        
        // Publish actual state based on emergency lock
        String actualState = autoEngine.isEmergency() ? "OFF" : "ON";
        publishState(actualState);
        
        // Subscribe to control commands
        mqttClient.subscribe(TOPIC_CTRL, 1);
        return true;
    } else {
        Serial.print("failed, rc=");
        Serial.println(mqttClient.state());
        actuators.setMqttLed(false);
        return false;
    }
}

void MqttManager::update() {
    if (!mqttClient.connected()) {
        actuators.setMqttLed(false);
        unsigned long now = millis();
        if (now - lastReconnectAttempt > 5000) {
            lastReconnectAttempt = now;
            if (reconnectMqtt()) {
                lastReconnectAttempt = 0;
            }
        }
    } else {
        mqttClient.loop();
    }
}

bool MqttManager::isConnected() {
    return mqttClient.connected();
}

void MqttManager::publishTelemetry(const SensorData& data) {
    if (isConnected()) {
        String payload = PayloadBuilder::buildSensorPayload(data);
        Serial.printf("[MQTT-OUT] Publishing Telemetry to %s\n", TOPIC_SENSOR);
        Serial.printf("           Payload: %s\n", payload.c_str());
        mqttClient.publish(TOPIC_SENSOR, payload.c_str(), false); // QoS 0 for telemetry in PubSubClient, or use QoS 1 if supported by lib
    }
}

void MqttManager::publishState(const String& powerState) {
    if (isConnected()) {
        String payload = PayloadBuilder::buildStatePayload(powerState);
        Serial.printf("[MQTT-OUT] Publishing State (Heartbeat) to %s\n", TOPIC_STATE);
        Serial.printf("           Payload: %s\n", payload.c_str());
        mqttClient.publish(TOPIC_STATE, payload.c_str(), true); // Retained = true
    }
}

void MqttManager::publishAck(const String& controlId, const String& status) {
    if (isConnected()) {
        String payload = PayloadBuilder::buildAckPayload(controlId, status);
        Serial.printf("[MQTT-OUT] Publishing ACK to %s\n", TOPIC_ACK);
        Serial.printf("           Payload: %s\n", payload.c_str());
        mqttClient.publish(TOPIC_ACK, payload.c_str(), false);
    }
}

void MqttManager::mqttCallback(char* topic, byte* payload, unsigned int length) {
    String msg;
    for (int i = 0; i < length; i++) {
        msg += (char)payload[i];
    }
    Serial.printf("[MQTT-IN] Topic: %s | Payload: %s\n", topic, msg.c_str());
    
    if (String(topic) == TOPIC_CTRL) {
        CommandData cmd;
        PayloadBuilder::parseControlPayload(msg, cmd);
        
        if (cmd.power == "CLEAR_FAULT") {
            autoEngine.clearEmergency();
            mqttManager.publishAck(cmd.controlId, "SUCCESS");
            return;
        }

        if (autoEngine.isEmergency()) {
            Serial.println("[MQTT] Ignored control command due to local EMERGENCY mode.");
            mqttManager.publishAck(cmd.controlId, "FAILED");
            return;
        }

        if (cmd.power == "ON" || cmd.power == "OFF") {
            actuators.applySmartState(cmd);
            mqttManager.publishAck(cmd.controlId, "SUCCESS");
        } else {
            mqttManager.publishAck(cmd.controlId, "FAILED");
        }
    }
}
