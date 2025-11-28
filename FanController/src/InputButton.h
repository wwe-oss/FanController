#ifndef INPUT_BUTTON_H
#define INPUT_BUTTON_H

#include <Arduino.h>

class InputButton
{
public:
    enum Event { NONE, SINGLE_PRESS, DOUBLE_PRESS, HOLD };

    InputButton(uint8_t pin, bool pullup = true);

    void begin();
    void update(uint32_t nowMs);
    Event getEvent();

private:
    uint8_t _pin;
    bool    _pullup;

    bool     _lastState;
    uint32_t _lastChangeMs;
    uint32_t _pressStartMs;
    uint8_t  _clickCount;
    uint32_t _lastClickMs;

    Event    _pendingEvent;

    void processEvent(uint32_t nowMs);
};

#endif // INPUT_BUTTON_H
