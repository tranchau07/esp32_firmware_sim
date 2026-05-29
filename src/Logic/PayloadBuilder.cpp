#include "PayloadBuilder.h"

String PayloadBuilder::buildSensorPayload(const SensorData& data) {
    DynamicJsonDocument doc(512);
    
    JsonObject env = doc.createNestedObject("environment");
    env["temperature"] = data.temperature;
    env["humidity"] = data.humidity;
    env["occupancy"] = data.occupancy;
    env["lightLevel"] = data.lightLevel;
    env["motionDetected"] = data.motionDetected;
    
    doc["voltage"] = data.voltage;
    doc["smokeDetected"] = data.smokeDetected;
    doc["doorOpen"] = data.doorOpen;
    
    String output;
    serializeJson(doc, output);
    return output;
}

String PayloadBuilder::buildStatePayload(const String& powerStatus) {
    DynamicJsonDocument doc(1024);
    doc["power"] = powerStatus;
    
    String output;
    serializeJson(doc, output);
    return output;
}

String PayloadBuilder::buildAckPayload(const String& controlId, const String& status) {
    DynamicJsonDocument doc(1024);
    doc["controlId"] = controlId;
    doc["status"] = status;
    
    String output;
    serializeJson(doc, output);
    return output;
}

void PayloadBuilder::parseControlPayload(const String& payload, CommandData& outCommand) {
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, payload);
    
    if (!error) {
        outCommand.controlId = doc["controlId"].as<String>();
        
        JsonObject cmd = doc["command"];
        outCommand.power = cmd["power"].as<String>();
        outCommand.acMode = cmd["acMode"].as<String>();
        outCommand.acTemp = cmd["acTemp"].as<float>();
        
        JsonArray lightArr = cmd["lightStates"].as<JsonArray>();
        outCommand.lightStates.clear();
        for (JsonVariant v : lightArr) {
            outCommand.lightStates.push_back(v.as<int>());
        }
        
        JsonArray fanArr = cmd["fanSpeed"].as<JsonArray>();
        outCommand.fanSpeeds.clear();
        for (JsonVariant v : fanArr) {
            outCommand.fanSpeeds.push_back(v.as<int>());
        }
    } else {
        Serial.print("[JSON ERROR] parseControlPayload failed: ");
        Serial.println(error.c_str());
    }
}
