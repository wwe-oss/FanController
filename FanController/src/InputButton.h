#ifndef INPUT_BUTTON_H
#define INPUT_BUTTON_H

#include <Arduino.h>

class InputButton
{
public:
  InputButton(uint8_t pin);

  void begin();
  void scan();

  bool wasSingleClick();
  bool wasDoubleClick();
  bool wasLongHold();

private:
  uint8_t pin;

  bool lastState;
  uint32_t lastDebounce;
  uint32_t pressTime;
  uint8_t clickCount;

  bool singleFlag;
  bool doubleFlag;
  bool holdFlag;
};

#endif
