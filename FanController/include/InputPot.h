#ifndef INPUT_POT_H
#define INPUT_POT_H

#include <Arduino.h>

class InputPot
{
public:
  InputPot(uint8_t pin);
  void begin();
  void scan();
  float getValue() const { return val; }

private:
  uint8_t pin;
  float val;
};

#endif
