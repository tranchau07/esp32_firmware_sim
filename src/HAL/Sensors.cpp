#include "Sensors.h"

Sensors sensors;

volatile bool Sensors::motionFlag = false;
volatile bool Sensors::doorFlag = false;
unsigned long Sensors::lastDoorDebounceTime = 0;

void IRAM_ATTR Sensors::handleMotionInterrupt() {
    motionFlag = true;
}

void IRAM_ATTR Sensors::handleDoorInterrupt() {
    if ((millis() - lastDoorDebounceTime) > DEBOUNCE_DELAY) {
        doorFlag = true;
        lastDoorDebounceTime = millis();
    }
}

void Sensors::init() {
    dht.begin();
    
    pinMode(PIN_PIR, INPUT);
    pinMode(PIN_LDR, INPUT);
    pinMode(PIN_VOLT, INPUT);
    pinMode(PIN_SMOKE, INPUT_PULLUP);
    pinMode(PIN_DOOR, INPUT_PULLUP);
    
    attachInterrupt(digitalPinToInterrupt(PIN_PIR), handleMotionInterrupt, RISING);
    attachInterrupt(digitalPinToInterrupt(PIN_DOOR), handleDoorInterrupt, CHANGE);
    
    // Initial read
    readAnalogSensors();
    currentData.doorOpen = digitalRead(PIN_DOOR);
    currentData.occupancy = 0; // Default
}

void Sensors::update() {
    unsigned long currentMillis = millis();
    
    // Handle interrupts fast flags
    if (motionFlag) {
        currentData.motionDetected = true;
        currentData.occupancy = 1; // Simplified: PIR detects -> Occupancy > 0
        motionFlag = false;
    } else {
        currentData.motionDetected = false;
    }
    
    if (doorFlag) {
        currentData.doorOpen = digitalRead(PIN_DOOR);
        doorFlag = false;
    }

    // Polling slow sensors every INTERVAL_SENSOR_READ
    if (currentMillis - lastReadTime >= INTERVAL_SENSOR_READ) {
        lastReadTime = currentMillis;
        
        float h = dht.readHumidity();
        float t = dht.readTemperature();
        if (!isnan(h) && !isnan(t)) {
            currentData.temperature = t;
            currentData.humidity = h;
        }
        
        readAnalogSensors();
    }
}

void Sensors::readAnalogSensors() {
    // Read light (0-4095 mapped to 0-1000 lux roughly)
    currentData.lightLevel = map(analogRead(PIN_LDR), 0, 4095, 1000, 0); 
    
    // Read voltage (Simulated 0-4095 mapped to 0-300V)
    currentData.voltage = map(analogRead(PIN_VOLT), 0, 4095, 0, 300);
    
    // Smoke is digital slide switch in simulation (High = smoke)
    currentData.smokeDetected = digitalRead(PIN_SMOKE);
}

SensorData Sensors::getData() {
    return currentData;
}
