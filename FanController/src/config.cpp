#include "Config.h"
#include <ArduinoJson.h>

FanConfig g_config;

static const char* DEFAULT_CONFIG_JSON = R"json(
{
  "pins": {
    "fanPwm": 9,
    "fanTach": 2,
    "button": 4,
    "pot":  A0
  },
  "features": {
    "buttonEnabled": true,
    "potEnabled": true
  },
  "fan": {
    "pulsesPerRev": 2
  },
  "presets": [0.0, 0.30, 0.60, 1.00],
  "intervals": {
    "rpmSampleMs":   1000,
    "inputScanMs":   10,
    "fanUpdateMs":   50,
    "logMs":         1000
  }
}
)json";

static void applyDefaults(FanConfig& cfg)
{
    cfg.fanPwmPin  = 9;
    cfg.fanTachPin = 2;
    cfg.buttonPin  = 4;
    cfg.potPin     = A0;

    cfg.buttonEnabled = true;
    cfg.potEnabled    = true;

    cfg.pulsesPerRev = 2;

    cfg.presetSpeeds[0] = 0.0f;
    cfg.presetSpeeds[1] = 0.30f;
    cfg.presetSpeeds[2] = 0.60f;
    cfg.presetSpeeds[3] = 1.00f;

    cfg.rpmSampleIntervalMs = 1000;
    cfg.inputScanIntervalMs = 10;
    cfg.fanUpdateIntervalMs = 50;
    cfg.logIntervalMs       = 1000;
}

void loadDefaultConfig()
{
    applyDefaults(g_config);
}

bool loadConfigFromJson(const char* json)
{
    applyDefaults(g_config);

    if (json == nullptr) {
        return false;
    }

    // Resize docCapacity if your JSON becomes larger
    DynamicJsonDocument doc(1024);
    DeserializationError err = deserializeJson(doc, json);
    if (err) {
        return false;
    }

    JsonObject root = doc.as<JsonObject>();

    // Pins
    if (JsonObject pins = root["pins"].as<JsonObject>()) {
        if (pins["fanPwm"].is<uint8_t>())  g_config.fanPwmPin  = pins["fanPwm"];
        if (pins["fanTach"].is<uint8_t>()) g_config.fanTachPin = pins["fanTach"];
        if (pins["button"].is<uint8_t>())  g_config.buttonPin  = pins["button"];
        if (pins["pot"].is<uint8_t>())     g_config.potPin     = pins["pot"];
    }

    // Features
    if (JsonObject feats = root["features"].as<JsonObject>()) {
        if (feats["buttonEnabled"].is<bool>()) g_config.buttonEnabled = feats["buttonEnabled"];
        if (feats["potEnabled"].is<bool>())    g_config.potEnabled    = feats["potEnabled"];
    }

    // Fan
    if (JsonObject fan = root["fan"].as<JsonObject>()) {
        if (fan["pulsesPerRev"].is<uint16_t>()) {
            uint16_t ppr = fan["pulsesPerRev"];
            if (ppr > 0) g_config.pulsesPerRev = ppr;
        }
    }

    // Presets
    if (JsonArray presets = root["presets"].as<JsonArray>()) {
        uint8_t i = 0;
        for (float val : presets) {
            if (i >= FanConfig::PRESET_COUNT) break;
            if (val < 0.0f) val = 0.0f;
            if (val > 1.0f) val = 1.0f;
            g_config.presetSpeeds[i++] = val;
        }
    }

    // Intervals
    if (JsonObject ints = root["intervals"].as<JsonObject>()) {
        if (ints["rpmSampleMs"].is<uint32_t>()) g_config.rpmSampleIntervalMs = ints["rpmSampleMs"];
        if (ints["inputScanMs"].is<uint32_t>()) g_config.inputScanIntervalMs = ints["inputScanMs"];
        if (ints["fanUpdateMs"].is<uint32_t>()) g_config.fanUpdateIntervalMs = ints["fanUpdateMs"];
        if (ints["logMs"].is<uint32_t>())       g_config.logIntervalMs       = ints["logMs"];
    }

    // Validate intervals
    if (g_config.rpmSampleIntervalMs == 0) g_config.rpmSampleIntervalMs = 1000;
    if (g_config.inputScanIntervalMs == 0) g_config.inputScanIntervalMs = 10;
    if (g_config.fanUpdateIntervalMs == 0) g_config.fanUpdateIntervalMs = 50;
    if (g_config.logIntervalMs == 0)       g_config.logIntervalMs       = 1000;

    return true;
}

bool loadDefaultJsonConfig()
{
    return loadConfigFromJson(DEFAULT_CONFIG_JSON);
}
