#include "InputButton.h"

static const uint32_t DEBOUNCE_MS = 25;
static const uint32_t DOUBLE_MS   = 250;
static const uint32_t HOLD_MS     = 500;

InputButton::InputButton(uint8_t pin, bool pullup)
    : _pin(pin), _pullup(pullup),
      _lastState(false), _lastChangeMs(0),
      _pressStartMs(0),
      _clickCount(0), _lastClickMs(0),
      _pendingEvent(NONE)
{}

void InputButton::begin()
{
    pinMode(_pin, _pullup ? INPUT_PULLUP : INPUT);
    _lastState = (digitalRead(_pin) == LOW);
}

void InputButton::update(uint32_t nowMs)
{
    bool raw = (digitalRead(_pin) == LOW);
    if (raw != _lastState)
    {
        if ((nowMs - _lastChangeMs) >= DEBOUNCE_MS)
        {
            _lastState    = raw;
            _lastChangeMs = nowMs;

            if (raw)
            {
                _pressStartMs = nowMs;
            }
            else
            {
                uint32_t pressDuration = nowMs - _pressStartMs;
                if (pressDuration >= HOLD_MS)
                {
                    _pendingEvent = HOLD;
                    _clickCount = 0;
                }
                else
                {
                    _clickCount++;
                    _lastClickMs = nowMs;
                }
            }
        }
    }

    processEvent(nowMs);
}

void InputButton::processEvent(uint32_t nowMs)
{
    if (_pendingEvent != NONE) return;

    if (_clickCount == 1)
    {
        if ((nowMs - _lastClickMs) >= DOUBLE_MS)
        {
            _pendingEvent = SINGLE_PRESS;
            _clickCount = 0;
        }
    }
    else if (_clickCount >= 2)
    {
        _pendingEvent = DOUBLE_PRESS;
        _clickCount = 0;
    }
}

InputButton::Event InputButton::getEvent()
{
    Event e = _pendingEvent;
    _pendingEvent = NONE;
    return e;
}
