#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

struct FanConfig
{
    // Hardware pins
    uint8_t fanPwmPin;
    uint8_t fanTachPin;
    uint8_t buttonPin;
    uint8_t potPin;

    bool    buttonEnabled;
    bool    potEnabled;

    // Fan characteristics
    uint16_t pulsesPerRev;

    // Preset speeds [0.0, 1.0]
    static const uint8_t PRESET_COUNT = 4;
    float    presetSpeeds[PRESET_COUNT];

    // Scheduling intervals (ms)
    uint32_t rpmSampleIntervalMs;
    uint32_t inputScanIntervalMs;
    uint32_t fanUpdateIntervalMs;
    uint32_t logIntervalMs;
};

// Global configuration instance
extern FanConfig g_config;

// Load configuration from JSON text
// Returns true on success; false = JSON invalid or missing fields.
// On failure, g_config is left with defaults (via loadDefaultConfig()).
bool loadConfigFromJson(const char* json);

// Initialize g_config with compiled-in defaults
void loadDefaultConfig();

#endif // CONFIG_H
