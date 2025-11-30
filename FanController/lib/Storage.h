#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>

bool loadConfigFromSD(const char *path);
bool saveConfigToSD(const char *path);
bool loadConfigFromEEPROM();
bool saveConfigToEEPROM();

#endif
