#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include <ArduinoJson.h>

#define SD_CS_PIN 53

// Config structure
struct FanConfig {
  uint8_t defaultDuty = 80;
  uint8_t minDuty = 30;
  uint8_t maxDuty = 100;
  uint16_t runTimeMin = 1800; // seconds (30 min)
  uint16_t restTimeMin = 3600; // seconds (60 min)
  bool autoMode = true;
};

FanConfig config;

bool loadConfigFromSD(const char* filename = "/config.json") {
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD init failed.");
    return false;
  }

  File file = SD.open(filename);
  if (!file) {
    Serial.println("Failed to open config file.");
    return false;
  }

  StaticJsonDocument<512> doc;
  DeserializationError err = deserializeJson(doc, file);
  if (err) {
    Serial.print("JSON parse error: ");
    Serial.println(err.c_str());
    file.close();
    return false;
  }

  config.defaultDuty = doc["defaultDuty"] | config.defaultDuty;
  config.minDuty = doc["minDuty"] | config.minDuty;
  config.maxDuty = doc["maxDuty"] | config.maxDuty;
  config.runTimeMin = doc["runTimeMin"] | config.runTimeMin;
  config.restTimeMin = doc["restTimeMin"] | config.restTimeMin;
  config.autoMode = doc["autoMode"] | config.autoMode;

  file.close();
  Serial.println("Config loaded from SD.");
  return true;
}
