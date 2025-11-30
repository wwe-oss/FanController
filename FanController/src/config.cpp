#include "Config.h"
#include <ArduinoJson.h>

FanConfig g_config;

static const char *DEFAULT_CONFIG_JSON = R"json(
{
  "pins": {
    "fanPwm": 9,
    "fanTach": 2,
    "button": 4,
    "pot": 0
  },
  "features": {
    "buttonEnabled": true,
    "potEnabled": true
  },
  "fan": {
    "pulsesPerRev": 2
  },
  "presets": [0.25, 0.40, 0.60, 1.00],
  "intervals": {
    "rpmSampleMs": 1000,
    "inputScanMs": 10,
    "fanUpdateMs": 50,
    "logMs": 1000
  }
}
)json";

static void applyDefaults(FanConfig &c)
{
  c.fanPwmPin = 9;
  c.fanTachPin = 2;
  c.buttonPin = 4;
  c.potPin = 0;

  c.buttonEnabled = true;
  c.potEnabled = true;

  c.pulsesPerRev = 2;

  c.presetSpeeds[0] = 0.25f;
  c.presetSpeeds[1] = 0.40f;
  c.presetSpeeds[2] = 0.60f;
  c.presetSpeeds[3] = 1.00f;

  c.rpmSampleIntervalMs = 1000;
  c.inputScanIntervalMs = 10;
  c.fanUpdateIntervalMs = 50;
  c.logIntervalMs = 1000;
}

void loadDefaultConfig()
{
  applyDefaults(g_config);
}

bool loadConfigFromJson(const char *json)
{
  applyDefaults(g_config);
  if (!json)
    return false;

  JsonDocument doc(1024);
  if (deserializeJson(doc, json))
    return false;

  JsonObject root = doc.as<JsonObject>();

  if (auto p = root["pins"].as<JsonObject>())
  {
    if (p["fanPwm"].is<uint8_t>())
      g_config.fanPwmPin = p["fanPwm"];
    if (p["fanTach"].is<uint8_t>())
      g_config.fanTachPin = p["fanTach"];
    if (p["button"].is<uint8_t>())
      g_config.buttonPin = p["button"];
    if (p["pot"].is<uint8_t>())
      g_config.potPin = p["pot"];
  }

  if (auto f = root["features"].as<JsonObject>())
  {
    if (f["buttonEnabled"].is<bool>())
      g_config.buttonEnabled = f["buttonEnabled"];
    if (f["potEnabled"].is<bool>())
      g_config.potEnabled = f["potEnabled"];
  }

  if (auto fan = root["fan"].as<JsonObject>())
  {
    if (fan["pulsesPerRev"].is<uint16_t>())
      g_config.pulsesPerRev = fan["pulsesPerRev"];
  }

  if (auto arr = root["presets"].as<JsonArray>())
  {
    uint8_t i = 0;
    for (float v : arr)
    {
      if (i >= FanConfig::PRESET_COUNT)
        break;
      if (v < 0.0f)
        v = 0.0f;
      if (v > 1.0f)
        v = 1.0f;
      g_config.presetSpeeds[i++] = v;
    }
  }

  if (auto t = root["intervals"].as<JsonObject>())
  {
    if (t["rpmSampleMs"].is<uint32_t>())
      g_config.rpmSampleIntervalMs = t["rpmSampleMs"];
    if (t["inputScanMs"].is<uint32_t>())
      g_config.inputScanIntervalMs = t["inputScanMs"];
    if (t["fanUpdateMs"].is<uint32_t>())
      g_config.fanUpdateIntervalMs = t["fanUpdateMs"];
    if (t["logMs"].is<uint32_t>())
      g_config.logIntervalMs = t["logMs"];
  }

  return true;
}

bool loadDefaultJsonConfig()
{
  return loadConfigFromJson(DEFAULT_CONFIG_JSON);
}
