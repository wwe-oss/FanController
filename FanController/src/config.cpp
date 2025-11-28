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
    if (!json) return false;

    StaticJsonDocument<1024> doc;
    auto err = deserializeJson(doc, json);
    if (err) return false;

    if (auto pins = doc["pins"].as<JsonObject>()) {
        if (pins.containsKey("fanPwm"))  g_config.fanPwmPin  = pins["fanPwm"];
        if (pins.containsKey("fanTach")) g_config.fanTachPin = pins["fanTach"];
        if (pins.containsKey("button"))  g_config.buttonPin  = pins["button"];
        if (pins.containsKey("pot"))     g_config.potPin     = pins["pot"];
    }

    if (auto feats = doc["features"].as<JsonObject>()) {
        if (feats.containsKey("buttonEnabled")) g_config.buttonEnabled = feats["buttonEnabled"];
        if (feats.containsKey("potEnabled"))    g_config.potEnabled    = feats["potEnabled"];
    }

    if (auto fan = doc["fan"].as<JsonObject>()) {
        if (fan.containsKey("pulsesPerRev")) {
            uint16_t ppr = fan["pulsesPerRev"];
            if (ppr > 0) g_config.pulsesPerRev = ppr;
        }
    }

    if (auto presets = doc["presets"].as<JsonArray>()) {
        uint8_t i = 0;
        for (auto v : presets) {
            if (i >= FanConfig::PRESET_COUNT) break;
            float val = v.as<float>();
            if (val < 0.0f) val = 0.0f;
            if (val > 1.0f) val = 1.0f;
            g_config.presetSpeeds[i++] = val;
        }
    }

    if (auto ints = doc["intervals"].as<JsonObject>()) {
        if (ints.containsKey("rpmSampleMs")) g_config.rpmSampleIntervalMs = ints["rpmSampleMs"];
        if (ints.containsKey("inputScanMs")) g_config.inputScanIntervalMs = ints["inputScanMs"];
        if (ints.containsKey("fanUpdateMs")) g_config.fanUpdateIntervalMs = ints["fanUpdateMs"];
        if (ints.containsKey("logMs"))       g_config.logIntervalMs       = ints["logMs"];
    }

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
