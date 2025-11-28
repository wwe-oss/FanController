#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

struct FanConfig
{
    uint8_t fanPwmPin;
    uint8_t fanTachPin;
    uint8_t buttonPin;
    uint8_t potPin;

    bool    buttonEnabled;
    bool    potEnabled;

    uint16_t pulsesPerRev;

    static const uint8_t PRESET_COUNT = 4;
    float    presetSpeeds[PRESET_COUNT];

    uint32_t rpmSampleIntervalMs;
    uint32_t inputScanIntervalMs;
    uint32_t fanUpdateIntervalMs;
    uint32_t logIntervalMs;
};

extern FanConfig g_config;

bool loadConfigFromJson(const char* json);
void loadDefaultConfig();

#endif // CONFIG_H
