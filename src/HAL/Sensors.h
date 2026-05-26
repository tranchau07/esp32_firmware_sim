#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include <DHT.h>
#include "Config.h"

struct SensorData {
    float temperature;
    float humidity;
    int occupancy;
    int lightLevel;
    bool motionDetected;
    float voltage;
    bool smokeDetected;
    bool doorOpen;
};

class Sensors {
public:
    void init();
    void update(); // Non-blocking update loop
    SensorData getData();
    
    // Interrupt handler statics
    static void IRAM_ATTR handleMotionInterrupt();
    static void IRAM_ATTR handleDoorInterrupt();

private:
    DHT dht{PIN_DHT, DHT22};
    SensorData currentData;
    unsigned long lastReadTime = 0;

    void readAnalogSensors();
    static volatile bool motionFlag;
    static volatile bool doorFlag;
    static unsigned long lastDoorDebounceTime;
};

extern Sensors sensors;

#endif // SENSORS_H
