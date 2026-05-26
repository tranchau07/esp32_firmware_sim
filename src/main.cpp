#include <Arduino.h>
#include "Config.h"
#include "HAL/Sensors.h"
#include "HAL/Actuators.h"
#include "Network/MqttManager.h"
#include "Logic/AutomationEngine.h"

unsigned long lastTelemetryTime = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("\n[SYSTEM] Booting Enterprise Smart Classroom Node...");
    
    actuators.init();
    sensors.init();
    autoEngine.init();
    mqttManager.init();
    
    Serial.println("[SYSTEM] Boot complete.");
}

void loop() {
    // 1. Maintain Network Connection
    mqttManager.update();
    
    // 2. Read Sensors (Non-blocking)
    sensors.update();
    SensorData currentData = sensors.getData();
    
    // 3. Process Automation & Safety Overrides locally
    autoEngine.process(currentData);
    
    // 4. Update Actuators (Blinking states, etc.)
    actuators.update();
    
    // 5. Scheduled Publish (Telemetry & Heartbeat)
    unsigned long now = millis();
    if (now - lastTelemetryTime >= INTERVAL_TELEMETRY) {
        lastTelemetryTime = now;
        
        if (mqttManager.isConnected()) {
            Serial.println("[SCHEDULER] Publishing Telemetry and Heartbeat State...");
            mqttManager.publishTelemetry(currentData);
            
            // For simulation, we assume if we aren't in emergency and there's no custom command, we just report ON.
            // Normally we'd track actual relay state here.
            String powerState = autoEngine.isEmergency() ? "OFF" : "ON";
            mqttManager.publishState(powerState);
        }
    }
}
