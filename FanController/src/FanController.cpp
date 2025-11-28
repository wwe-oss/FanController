#include "FanController.h"

static FanController* g_fanInstance = nullptr;

void fanTachISRWrapper()
{
    if (g_fanInstance != nullptr)
    {
        g_fanInstance->_tachPulseCount++;
    }
}

FanController::FanController(uint8_t pwmPin, uint8_t tachPin, uint16_t pulsesPerRevolution)
    : _pwmPin(pwmPin),
      _tachPin(tachPin),
      _pulsesPerRev(pulsesPerRevolution),
      _tachPulseCount(0),
      _lastRpmSampleMs(0),
      _sampleIntervalMs(1000),  // 1 s sample window
      _rpm(0),
      _duty(0.0f)
{
}

void FanController::begin()
{
    pinMode(_pwmPin, OUTPUT);
    pinMode(_tachPin, INPUT_PULLUP);

    _tachPulseCount = 0;
    _lastRpmSampleMs = millis();

    g_fanInstance = this;
    attachInterrupt(digitalPinToInterrupt(_tachPin), fanTachISRWrapper, FALLING);

    _applyDuty();
}

void FanController::setDuty(float duty)
{
    if (duty < 0.0f) duty = 0.0f;
    if (duty > 1.0f) duty = 1.0f;

    _duty = duty;
    _applyDuty();
}

void FanController::_applyDuty()
{
    uint8_t pwmValue = static_cast<uint8_t>(_duty * 255.0f + 0.5f);
    analogWrite(_pwmPin, pwmValue);
}

void FanController::update(uint32_t nowMs)
{
    uint32_t elapsed = nowMs - _lastRpmSampleMs;
    if (elapsed < _sampleIntervalMs)
    {
        return;
    }

    noInterrupts();
    uint16_t pulses = _tachPulseCount;
    _tachPulseCount = 0;
    interrupts();

    _lastRpmSampleMs = nowMs;

    if (elapsed == 0 || _pulsesPerRev == 0)
    {
        _rpm = 0;
        return;
    }

    float pulsesPerMinute = (static_cast<float>(pulses) * 60000.0f) / static_cast<float>(elapsed);
    float revsPerMinute   = pulsesPerMinute / static_cast<float>(_pulsesPerRev);

    if (revsPerMinute < 0.0f)
    {
        _rpm = 0;
    }
    else if (revsPerMinute > 65535.0f)
    {
        _rpm = 65535;
    }
    else
    {
        _rpm = static_cast<uint16_t>(revsPerMinute + 0.5f);
    }
}
