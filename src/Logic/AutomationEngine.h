#ifndef AUTOMATION_ENGINE_H
#define AUTOMATION_ENGINE_H

#include "HAL/Sensors.h"
#include "HAL/Actuators.h"
#include "Config.h"

class AutomationEngine {
public:
    void init();
    void process(const SensorData& data);
    bool isEmergency();
    void clearEmergency();

private:
    bool emergencyState = false;
    unsigned long emptyStartTime = 0;
    bool isRoomEmpty = false;
};

extern AutomationEngine autoEngine;

#endif // AUTOMATION_ENGINE_H
