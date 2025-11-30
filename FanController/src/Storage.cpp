#include "Storage.h"
#include "Config.h"
#include <EEPROM.h>
#include <SD.h>

static constexpr const char *DEFAULT_PATH = "/fanconfig.json";

bool loadConfigFromSD(const char *path)
{
  if (!SD.begin())
    return false;
  File f = SD.open(path, FILE_READ);
  if (!f)
    return false;

  String json = f.readString();
  f.close();
  return loadConfigFromJson(json.c_str());
}

bool saveConfigToSD(const char *path)
{
  if (!SD.begin())
    return false;

  DynamicJsonDocument doc(1024);
  JsonObject root = doc.to<JsonObject>();

  JsonObject pins = root.createNestedObject("pins");
  pins["fanPwm"] = g_config.fanPwmPin;
  pins["fanTach"] = g_config.fanTachPin;
  pins["button"] = g_config.buttonPin;
  pins["pot"] = g_config.potPin;

  JsonObject feats = root.createNestedObject("features");
  feats["buttonEnabled"] = g_config.buttonEnabled;
  feats["potEnabled"] = g_config.potEnabled;

  JsonObject fan = root.createNestedObject("fan");
  fan["pulsesPerRev"] = g_config.pulsesPerRev;

  JsonArray presets = root.createNestedArray("presets");
  for (uint8_t i = 0; i < FanConfig::PRESET_COUNT; i++)
    presets.add(g_config.presetSpeeds[i]);

  JsonObject ints = root.createNestedObject("intervals");
  ints["rpmSampleMs"] = g_config.rpmSampleIntervalMs;
  ints["inputScanMs"] = g_config.inputScanIntervalMs;
  ints["fanUpdateMs"] = g_config.fanUpdateIntervalMs;
  ints["logMs"] = g_config.logIntervalMs;

  File f = SD.open(path, FILE_WRITE | O_TRUNC);
  if (!f)
    return false;

  serializeJsonPretty(doc, f);
  f.close();
  return true;
}

bool saveConfigToEEPROM()
{
  EEPROM.put(0, g_config);
  return true;
}

bool loadConfigFromEEPROM()
{
  FanConfig temp;
  EEPROM.get(0, temp);

  // basic sanity check
  if (temp.fanPwmPin > 70)
    return false;
  g_config = temp;
  return true;
}
