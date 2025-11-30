#ifndef AUTO_SCHEDULE_H
#define AUTO_SCHEDULE_H

#include <Arduino.h>

struct AutoSchedule
{
  uint32_t onMs;
  uint32_t offMs;
  uint32_t lastSwitch;
  bool state;
};

class AutoController
{
public:
  AutoController();
  void begin(uint32_t onMs, uint32_t offMs);
  void update(uint32_t now);
  bool shouldRun() const { return state; }

private:
  uint32_t onMs;
  uint32_t offMs;
  uint32_t lastSwitch;
  bool state;
};

#endif
