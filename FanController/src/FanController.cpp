#include "FanController.h"

// Minimum safe duty for 3-wire fan
static constexpr float MIN_DUTY = 0.25f;
static constexpr float DUTY_STEP = 0.02f;     // soft-start increment
static constexpr uint32_t SOFT_START_MS = 40; // smooth on ramp
static constexpr uint32_t SAFETY_MS = 800;    // stall check interval

// Tach ISR pointer
static FanController* _fanInstance = nullptr;

void tachISR() {
    if (_fanInstance) _fanInstance->isrTachTick();
}

FanController::FanController(uint8_t pwm, uint8_t tach)
    : pwmPin(pwm), tachPin(tach), tachCount(0),
      rpm(0), lastRPMCalc(0), lastSafetyCheck(0),
      lastSoftStart(0), duty(0.0f), targetDuty(0.0f),
      mode(MODE_OFF), presetIndex(0)
{
}

void FanController::begin() {
    pinMode(pwmPin, OUTPUT);
    analogWrite(pwmPin, 0);

    pinMode(tachPin, INPUT_PULLUP);
    _fanInstance = this;
    attachInterrupt(digitalPinToInterrupt(tachPin), tachISR, FALLING);

    duty = 0.0f;
    targetDuty = 0.0f;
    presetIndex = 0;
}

void FanController::setMode(FanMode m) {
    mode = m;

    switch (mode) {
        case MODE_OFF:
            targetDuty = 0.0f;
            break;

        case MODE_PRESET:
            presetIndex = 0;
            targetDuty = g_config.presetSpeeds[presetIndex];
            break;

        case MODE_MANUAL:
            // manual duty set externally via setManualDuty()
            break;

        case MODE_BOOST:
            targetDuty = 1.0f;  // full
            break;
    }
}

void FanController::nextPreset() {
    presetIndex = (presetIndex + 1) % FanConfig::PRESET_COUNT;
    targetDuty = g_config.presetSpeeds[presetIndex];
}

void FanController::setManualDuty(float d) {
    if (d < 0.0f) d = 0.0f;
    if (d > 1.0f) d = 1.0f;
    mode = MODE_MANUAL;
    targetDuty = d;
}

void FanController::isrTachTick() {
    tachCount++;
}

void FanController::computeRPM(uint32_t now) {
    if (now - lastRPMCalc < g_config.rpmSampleIntervalMs) return;

    uint16_t count = tachCount;
    tachCount = 0;

    uint32_t deltaMs = now - lastRPMCalc;
    lastRPMCalc = now;

    if (deltaMs == 0 || g_config.pulsesPerRev == 0) {
        rpm = 0;
        return;
    }

    float hz = (float)count / (deltaMs / 1000.0f);
    rpm = (uint16_t)((hz * 60.0f) / g_config.pulsesPerRev);
}

void FanController::softStart(uint32_t now) {
    if (now - lastSoftStart < SOFT_START_MS) return;
    lastSoftStart = now;

    // if target is lower than MIN_DUTY, clamp it
    float desired = targetDuty;
    if (desired > 0.0f && desired < MIN_DUTY)
        desired = MIN_DUTY;

    // ramp toward target
    if (duty < desired) {
        duty += DUTY_STEP;
        if (duty > desired) duty = desired;
    } else {
        duty = desired;
    }

    applyDuty(duty);
}

void FanController::safetyCheck(uint32_t now) {
    if (now - lastSafetyCheck < SAFETY_MS) return;
    lastSafetyCheck = now;

    if (duty < MIN_DUTY) return;  // low or off → ignore

    if (rpm < 100 && duty > 0.30f) {
        // fan is commanded to run but RPM collapsed → restart
        duty = MIN_DUTY;
        targetDuty = MIN_DUTY;
        applyDuty(duty);
    }
}

void FanController::applyDuty(float d) {
    if (d <= 0.0f) {
        analogWrite(pwmPin, 0);
        return;
    }

    uint8_t out = (uint8_t)(d * 255.0f);
    analogWrite(pwmPin, out);
}

void FanController::update(uint32_t now) {
    computeRPM(now);
    softStart(now);
    safetyCheck(now);
}
