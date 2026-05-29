#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ---------------------------------------------------------
// DEVICE & MQTT CONFIGURATION
// ---------------------------------------------------------
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASS ""
#define MQTT_BROKER "broker.emqx.io" // Public EMQX Broker - internet connection
#define MQTT_PORT 1883
#define MQTT_USERNAME "admin"
#define MQTT_PASSWORD "123456789"
#define MQTT_CLIENT_ID "ESP32_01"

// Topic Structure
#define TOPIC_SENSOR "hust/A/699b0889512fe68fc2da3d82/ESP32_01/up/sensor"
#define TOPIC_STATE  "hust/A/699b0889512fe68fc2da3d82/ESP32_01/up/state"
#define TOPIC_ACK    "hust/A/699b0889512fe68fc2da3d82/ESP32_01/up/control/response"
#define TOPIC_CTRL   "hust/A/699b0889512fe68fc2da3d82/ESP32_01/down/control"

// ---------------------------------------------------------
// HARDWARE PIN MAPPING
// ---------------------------------------------------------
// Sensors
#define PIN_DHT     4
#define PIN_PIR     13
#define PIN_LDR     34
#define PIN_VOLT    35
#define PIN_SMOKE   32
#define PIN_DOOR    14

// Actuators
#define PIN_NEOPIXEL 25
#define NUM_PIXELS 16
#define PIN_BUZZER   12

// Indicators
#define PIN_LED_WIFI      2
#define PIN_LED_MQTT      15
#define PIN_LED_EMERGENCY 5

// ---------------------------------------------------------
// RUNTIME TIMING & INTERVALS
// ---------------------------------------------------------
#define INTERVAL_SENSOR_READ 5000     // 5 seconds
#define INTERVAL_TELEMETRY   5000     // 5 seconds (Reduced publish interval for high-frequency realtime telemetry)
#define DEBOUNCE_DELAY       50       // 50ms for door sensor

// ---------------------------------------------------------
// SAFETY THRESHOLDS
// ---------------------------------------------------------
#define VOLTAGE_MAX_SAFE     240.0

#endif // CONFIG_H
