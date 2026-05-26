#ifndef PAYLOAD_BUILDER_H
#define PAYLOAD_BUILDER_H

#include <ArduinoJson.h>
#include "HAL/Sensors.h"
#include "Config.h"
#include <vector>

struct CommandData {
    String controlId;
    String power;
    String acMode;
    float acTemp;
    std::vector<int> lightStates;
    std::vector<int> fanSpeeds;
};

class PayloadBuilder {
public:
    static String buildSensorPayload(const SensorData& data);
    static String buildStatePayload(const String& powerStatus);
    static String buildAckPayload(const String& controlId, const String& status);
    
    // Parses incoming control payload
    static void parseControlPayload(const String& payload, CommandData& outCommand);
};

#endif // PAYLOAD_BUILDER_H
