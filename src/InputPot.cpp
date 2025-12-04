#include "InputPot.h"

InputPot::InputPot(uint8_t pin)
    : _pin(pin), _duty(0.0f), _lastReadMs(0)
{
}

void InputPot::begin()
{
  pinMode(_pin, INPUT);
}

void InputPot::update(uint32_t nowMs)
{
#include "InputPot.h"

  InputPot::InputPot(uint8_t p) : pin(p),
                                  val(0.0f)
  {
  }

  void InputPot::begin()
  {
    if (pin > 0)
      pinMode(pin, INPUT);
  }

  void InputPot::scan()
  {
    if (pin == 0)
      return;
    int raw = analogRead(pin);
    val = raw / 1023.0f;
  }

  if ((nowMs - _lastReadMs) < READ_INTERVAL_MS)
    return;
  _lastReadMs = nowMs;

  int raw = analogRead(_pin);
  _duty = (float)raw / 1023.0f;
  if (_duty < 0.0f)
    _duty = 0.0f;
  if (_duty > 1.0f)
    _duty = 1.0f;
}
