#ifndef ACTUATORS_H
#define ACTUATORS_H

#include <Arduino.h>
#include "Config.h"
#include <Adafruit_NeoPixel.h>
#include "Logic/PayloadBuilder.h"

enum PowerState { POWER_OFF = 0, POWER_ON = 1 };

class Actuators {
public:
    void init();
    
    // NeoPixel Controls
    void applySmartState(const CommandData& cmd);
    void setAllOff(); // Emergency or Idle mode
    
    // Indicators
    void setWifiLed(bool state);
    void setMqttLed(bool state);
    void setEmergencyAlert(bool active); // Toggles Red LED and Buzzer
    
    // Update loop for animations (blinking)
    void update();

private:
    bool emergencyMode = false;
    unsigned long lastEmergencyToggle = 0;
    bool emergencyToggleState = false;
    Adafruit_NeoPixel strip;
};

extern Actuators actuators;

#endif // ACTUATORS_H
