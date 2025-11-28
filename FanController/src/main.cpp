#include <Arduino.h>
#include "Config.h"
#include "FanController.h"
#include "InputButton.h"
#include "InputPot.h"
#include "Scheduler.h"

FanController fan(g_config.fanPwmPin, g_config.fanTachPin);
InputButton btn(g_config.buttonPin);
InputPot pot(g_config.potPin);

Scheduler scheduler;

void setup() {
    Serial.begin(115200);
    loadDefaultJsonConfig();

    fan.begin();
    btn.begin();
    if (g_config.potEnabled) pot.begin();

    scheduler.every(g_config.inputScanIntervalMs, []() {
        btn.scan();
        if (g_config.potEnabled) pot.scan();
    });

    scheduler.every(g_config.fanUpdateIntervalMs, []() {
        uint32_t now = millis();
        fan.update(now);
    });

    scheduler.every(g_config.logIntervalMs, []() {
        Serial.print("Mode=");
        Serial.print(fan.getMode());
        Serial.print(" Duty=");
        Serial.print(fan.getDuty(), 2);
        Serial.print(" RPM=");
        Serial.println(fan.getRPM());
    });
}

void loop() {
    scheduler.run(millis());

    // BUTTON → mode logic
    if (btn.wasSingleClick()) {
        if (fan.getMode() == MODE_PRESET) {
            fan.nextPreset();
        } else {
            fan.setMode(MODE_PRESET);
        }
    }

    if (btn.wasDoubleClick()) {
        fan.setMode(MODE_OFF);
    }

    if (btn.wasLongHold()) {
        fan.setMode(MODE_BOOST);
    }

    // POT → manual mode
    if (g_config.potEnabled) {
        float val = pot.getValue();
        if (val > 0.02f) {  // small dead zone
            fan.setManualDuty(val);
        }
    }
}
