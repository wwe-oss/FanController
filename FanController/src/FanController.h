#ifndef FAN_CONTROLLER_H
#define FAN_CONTROLLER_H

#include <Arduino.h>
#include "Config.h"

// Operating modes
enum FanMode : uint8_t {
    MODE_OFF = 0,
    MODE_PRESET,
    MODE_MANUAL,
    MODE_BOOST
};

class FanController {
public:
    FanController(uint8_t pwmPin, uint8_t tachPin);

    void begin();
    void update(uint32_t now);
    void setMode(FanMode mode);
    void nextPreset();
    void setManualDuty(float d);

    float getDuty() const { return duty; }
    uint16_t getRPM() const { return rpm; }
    FanMode getMode() const { return mode; }

    void isrTachTick();  // Called from interrupt

private:
    uint8_t pwmPin;
    uint8_t tachPin;

    volatile uint16_t tachCount;
    uint16_t rpm;

    uint32_t lastRPMCalc;
    uint32_t lastSafetyCheck;
    uint32_t lastSoftStart;

    float duty;
    float targetDuty;

    FanMode mode;
    uint8_t presetIndex;

    void applyDuty(float d);
    void computeRPM(uint32_t now);
    void softStart(uint32_t now);
    void safetyCheck(uint32_t now);
};

#endif
