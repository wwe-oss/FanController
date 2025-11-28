#include <Arduino.h>
#include "FanController.h"

// Adjust to your actual wiring
static constexpr uint8_t FAN_PWM_PIN  = 9;  // PWM output to fan
static constexpr uint8_t FAN_TACH_PIN = 2;  // Tach input (interrupt capable)
static constexpr uint16_t FAN_PULSES_PER_REV = 2; // typical PC fan

FanController fan(FAN_PWM_PIN, FAN_TACH_PIN, FAN_PULSES_PER_REV);

static uint32_t lastLogMs = 0;

void setup()
{
    Serial.begin(115200);
    while (!Serial) { /* wait on native USB, harmless on Mega */ }

    Serial.println(F("FanController starting..."));

    fan.begin();
    fan.setDuty(0.5f);   // start at 50% duty
}

void loop()
{
    uint32_t now = millis();

    fan.update(now);

    if (now - lastLogMs >= 1000)
    {
        lastLogMs = now;
        Serial.print(F("Duty="));
        Serial.print(fan.getDuty(), 2);
        Serial.print(F(" RPM="));
        Serial.println(fan.getRpm());
    }

    // Placeholder for control logic (e.g., temperature-based)
}
