#include "AutoSchedule.h"

AutoController::AutoController()
    : onMs(0), offMs(0), lastSwitch(0), state(false) {}

void AutoController::begin(uint32_t onMs_, uint32_t offMs_)
{
  onMs = onMs_;
  offMs = offMs_;
  lastSwitch = millis();
  state = true; // start ON
}

void AutoController::update(uint32_t now)
{
  uint32_t elapsed = now - lastSwitch;

  if (state && elapsed >= onMs)
  {
    state = false;
    lastSwitch = now;
  }
  else if (!state && elapsed >= offMs)
  {
    state = true;
    lastSwitch = now;
  }
}
