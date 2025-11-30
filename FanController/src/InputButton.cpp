#include "InputButton.h"

static constexpr uint32_t DEBOUNCE_MS = 20;
static constexpr uint32_t DOUBLE_MS = 300;
static constexpr uint32_t HOLD_MS = 500;

InputButton::InputButton(uint8_t p)
    : pin(p), lastState(false), lastDebounce(0),
      pressTime(0), clickCount(0),
      singleFlag(false), doubleFlag(false), holdFlag(false)
{
}

void InputButton::begin()
{
  pinMode(pin, INPUT_PULLUP);
}

void InputButton::scan()
{
  uint32_t now = millis();
  bool state = !digitalRead(pin); // active low

  if (state != lastState)
  {
    lastDebounce = now;
    lastState = state;
  }

  if (now - lastDebounce < DEBOUNCE_MS)
    return;

  if (state)
  { // pressed
    if (pressTime == 0)
      pressTime = now;

    if (!holdFlag && (now - pressTime) > HOLD_MS)
    {
      holdFlag = true;
      clickCount = 0;
    }
  }
  else
  { // released
    if (pressTime > 0 && !holdFlag)
    {
      clickCount++;
    }
    pressTime = 0;
  }

  if (clickCount == 1 && now - lastDebounce > DOUBLE_MS)
  {
    singleFlag = true;
    clickCount = 0;
  }
  else if (clickCount >= 2)
  {
    doubleFlag = true;
    clickCount = 0;
  }

  if (!state)
    holdFlag = false;
}

bool InputButton::wasSingleClick()
{
  bool f = singleFlag;
  singleFlag = false;
  return f;
}
bool InputButton::wasDoubleClick()
{
  bool f = doubleFlag;
  doubleFlag = false;
  return f;
}
bool InputButton::wasLongHold()
{
  bool f = holdFlag;
  holdFlag = false;
  return f;
}
