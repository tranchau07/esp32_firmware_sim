#include "AutomationEngine.h"

AutomationEngine autoEngine;

void AutomationEngine::init() {
    emergencyState = false;
    isRoomEmpty = false;
}

void AutomationEngine::process(const SensorData& data) {
    // 1. HARDWARE-LEVEL SAFETY OVERRIDE (Highest Priority)
    if (data.voltage > VOLTAGE_MAX_SAFE || data.smokeDetected) {
        if (!emergencyState) {
            emergencyState = true;
            actuators.setEmergencyAlert(true); // Locks all relays OFF hardware-side
            Serial.println("[AUTO] EMERGENCY TRIGGERED! Voltage/Smoke overload. LATCHING FAULT ACTIVE.");
        }
    } 
    // Removed automatic clearing here. It must be cleared by explicit command.

    if (emergencyState) return; // Block all other automation

    // 2. Local Fallback Empty Room Shutdown
    // The backend already handles empty room shutdown (10 minutes), 
    // but we can have a local fallback (e.g. 15 minutes) in case MQTT disconnects
    if (data.occupancy == 0) {
        if (!isRoomEmpty) {
            isRoomEmpty = true;
            emptyStartTime = millis();
        } else {
            // 15 mins = 900000 ms. For demo/simulation, let's use 30 seconds
            if (millis() - emptyStartTime > 30000) { 
                actuators.setAllOff();
                // Serial.println("[AUTO] LOCAL FALLBACK: Room empty for 30s. Shutting down.");
            }
        }
    } else {
        isRoomEmpty = false;
    }
}

bool AutomationEngine::isEmergency() {
    return emergencyState;
}

void AutomationEngine::clearEmergency() {
    if (emergencyState) {
        emergencyState = false;
        actuators.setEmergencyAlert(false);
        Serial.println("[AUTO] EMERGENCY CLEARED MANUALLY VIA COMMAND.");
    }
}

