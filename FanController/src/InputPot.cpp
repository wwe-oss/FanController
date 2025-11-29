#include "InputPot.h"

InputPot::InputPot(uint8_t p) : pin(p), val(0.0f) {}

void InputPot::begin() {
    if (pin > 0) pinMode(pin, INPUT);
}

void InputPot::scan() {
    if (pin == 0) return;
    int raw = analogRead(pin);
    val = raw / 1023.0f;
}
