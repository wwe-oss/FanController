#include <Arduino.h>
#include "Config.h"
#include "FanController.h"
#include "InputButton.h"
#include "InputPot.h"
#include "Scheduler.h"
#include "Storage.h"
#include "AutoSchedule.h"

FanController fan(g_config.fanPwmPin, g_config.fanTachPin);
InputButton btn(g_config.buttonPin);
InputPot pot(g_config.potPin);
Scheduler scheduler;
AutoController autoCtl;

enum SystemMode { SYS_AUTO, SYS_MANUAL };
static SystemMode systemMode = SYS_AUTO;

void setup() {
    Serial.begin(115200);

    // 1. Load in order of priority
    if (!loadConfigFromSD("/fanconfig.json"))
        if (!loadConfigFromEEPROM())
            loadDefaultJsonConfig();

    fan.begin();
    btn.begin();
    if (g_config.potEnabled) pot.begin();

    // Example: 20 min ON / 40 min OFF
    autoCtl.begin(20UL * 60000UL, 40UL * 60000UL);

    // Scheduler tasks
    scheduler.every(g_config.inputScanIntervalMs, []() {
        btn.scan();
        if (g_config.potEnabled) pot.scan();
    });

    scheduler.every(g_config.fanUpdateIntervalMs, []() {
        fan.update(millis());
    });

    scheduler.every(g_config.logIntervalMs, []() {
        Serial.print("SYS=");
        Serial.print(systemMode);
        Serial.print(" MODE=");
        Serial.print(fan.getMode());
        Serial.print(" Duty=");
        Serial.print(fan.getDuty(), 2);
        Serial.print(" RPM=");
        Serial.println(fan.getRPM());
    });
}

void loop() {
    uint32_t now = millis();
    scheduler.run(now);

    //------------------------------------------------------
    // BUTTON INPUT — top-level mode switching
    //------------------------------------------------------
    if (btn.wasSingleClick()) {
        if (systemMode == SYS_MANUAL) {
            fan.nextPreset();    // manual preset cycle
        } else {
            systemMode = SYS_MANUAL;
            fan.setMode(MODE_MANUAL);
        }
    }

    if (btn.wasDoubleClick()) {
        systemMode = SYS_AUTO;
        fan.setMode(MODE_PRESET);  // auto mode uses presets
    }

    if (btn.wasLongHold()) {
        systemMode = SYS_MANUAL;
        fan.setMode(MODE_BOOST);
    }

    //------------------------------------------------------
    // AUTO MODE ENGINE
    //------------------------------------------------------
    if (systemMode == SYS_AUTO) {
        autoCtl.update(now);

        if (autoCtl.shouldRun()) {
            fan.setMode(MODE_PRESET);
        } else {
            fan.setMode(MODE_OFF);
        }
    }

    //------------------------------------------------------
    // MANUAL MODE ENGINE
    //------------------------------------------------------
    if (systemMode == SYS_MANUAL && g_config.potEnabled) {
        float v = pot.getValue();
        if (v > 0.02f) {
            fan.setManualDuty(v);
        }
    }

    //------------------------------------------------------
    // FAILSAFE — duty fallback on tach failure
    //------------------------------------------------------
    if (fan.getRPM() < 100 && fan.getDuty() > 0.4f) {
        fan.setManualDuty(0.8f); // fallback 80%
    }
}
