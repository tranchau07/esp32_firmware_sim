#include "Actuators.h"

Actuators actuators;

void Actuators::init() {
    strip = Adafruit_NeoPixel(NUM_PIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
    strip.begin();
    strip.show(); // Initialize all pixels to 'off'

    pinMode(PIN_BUZZER, OUTPUT);
    
    pinMode(PIN_LED_WIFI, OUTPUT);
    pinMode(PIN_LED_MQTT, OUTPUT);
    pinMode(PIN_LED_EMERGENCY, OUTPUT);
    
    setAllOff();
    setWifiLed(false);
    setMqttLed(false);
}

void Actuators::applySmartState(const CommandData& cmd) {
    if (cmd.power == "OFF") {
        setAllOff();
        return;
    }
    
    // Map Lights (Index 0..7) - Max 8 Đèn
    for (int i = 0; i < 8; i++) {
        if (i < cmd.lightStates.size() && cmd.lightStates[i] > 0) {
            strip.setPixelColor(i, strip.Color(255, 200, 0)); // Yellow
        } else {
            strip.setPixelColor(i, strip.Color(0, 0, 0));
        }
    }
    
    // Map Fans (Index 8..14) - Max 7 Quạt
    for (int i = 0; i < 7; i++) {
        if (i < cmd.fanSpeeds.size() && cmd.fanSpeeds[i] > 0) {
            strip.setPixelColor(i + 8, strip.Color(0, 255, 0)); // Green
        } else {
            strip.setPixelColor(i + 8, strip.Color(0, 0, 0));
        }
    }
    
    // Map AC (Index 15)
    if (cmd.acMode == "COOL") {
        strip.setPixelColor(15, strip.Color(0, 0, 255)); // Blue
    } else if (cmd.acMode == "ECO") {
        strip.setPixelColor(15, strip.Color(0, 255, 255)); // Cyan
    } else {
        strip.setPixelColor(15, strip.Color(0, 0, 0));
    }
    
    strip.show();
}

void Actuators::setAllOff() {
    strip.clear();
    strip.show();
}

void Actuators::setWifiLed(bool state) {
    digitalWrite(PIN_LED_WIFI, state ? HIGH : LOW);
}

void Actuators::setMqttLed(bool state) {
    digitalWrite(PIN_LED_MQTT, state ? HIGH : LOW);
}

void Actuators::setEmergencyAlert(bool active) {
    emergencyMode = active;
    if (!active) {
        digitalWrite(PIN_LED_EMERGENCY, LOW);
        digitalWrite(PIN_BUZZER, LOW);
        strip.clear();
        strip.show();
    } else {
        setAllOff();
    }
}

void Actuators::update() {
    if (emergencyMode) {
        // Blink LED, Buzzer and all NeoPixels RED every 500ms
        unsigned long currentMillis = millis();
        if (currentMillis - lastEmergencyToggle >= 500) {
            lastEmergencyToggle = currentMillis;
            emergencyToggleState = !emergencyToggleState;
            digitalWrite(PIN_LED_EMERGENCY, emergencyToggleState ? HIGH : LOW);
            digitalWrite(PIN_BUZZER, emergencyToggleState ? HIGH : LOW);
            
            if (emergencyToggleState) {
                strip.fill(strip.Color(255, 0, 0));
            } else {
                strip.clear();
            }
            strip.show();
        }
    }
}
