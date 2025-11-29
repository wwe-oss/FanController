#ifndef INPUT_POT_H
#define INPUT_POT_H

#include <Arduino.h>

class InputPot
{
public:
    InputPot(uint8_t pin);

    void begin();
    void update(uint32_t nowMs);

    float getDuty() const { return _duty; }

private:
    uint8_t _pin;
    float   _duty;
    uint32_t _lastReadMs;
    static const uint32_t READ_INTERVAL_MS = 50;
};

#endif // INPUT_POT_H
