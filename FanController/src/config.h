#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

struct FanConfig {
    static constexpr uint8_t PRESET_COUNT = 4;

    // Pin assignments
    uint8_t fanPwmPin;
    uint8_t fanTachPin;
    uint8_t buttonPin;
    uint8_t potPin;

    // Features
    bool buttonEnabled;
    bool potEnabled;

    // Fan parameters
    uint16_t pulsesPerRev;
    float presetSpeeds[PRESET_COUNT];

    // Timers
    uint32_t rpmSampleIntervalMs;
    uint32_t inputScanIntervalMs;
    uint32_t fanUpdateIntervalMs;
    uint32_t logIntervalMs;
};

extern FanConfig g_config;

void loadDefaultConfig();
bool loadConfigFromJson(const char* json);
bool loadDefaultJsonConfig();

#endif
