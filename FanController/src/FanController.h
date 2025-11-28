#ifndef FAN_CONTROLLER_H
#define FAN_CONTROLLER_H

#include <Arduino.h>

class FanController
{
public:
    // pwmPin = fan PWM control pin, tachPin = tach signal input
    FanController(uint8_t pwmPin, uint8_t tachPin, uint16_t pulsesPerRevolution = 2);

    void begin();

    // Set requested speed as a duty cycle [0.0, 1.0]
    void setDuty(float duty);

    // Call frequently from loop(); nowMs = millis()
    void update(uint32_t nowMs);

    // Get last measured RPM (tach-based)
    uint16_t getRpm() const { return _rpm; }

    // Get current duty (0.0–1.0)
    float getDuty() const { return _duty; }

    // ISR needs access to this
    friend void fanTachISRWrapper();

private:
    uint8_t  _pwmPin;
    uint8_t  _tachPin;
    uint16_t _pulsesPerRev;

    volatile uint16_t _tachPulseCount;  // updated in ISR
    uint32_t _lastRpmSampleMs;
    uint32_t _sampleIntervalMs;

    uint16_t _rpm;
    float    _duty;

    void _applyDuty();
};

// ISR hook
void fanTachISRWrapper();

#endif // FAN_CONTROLLER_H
