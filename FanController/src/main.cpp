#include <Arduino.h>
#include "Config.h"
#include "FanController.h"
#include "InputButton.h"
#include "InputPot.h"
#include "Scheduler.h"

// Global scheduler
TaskScheduler scheduler;

// Global fan
FanController* g_fan = nullptr;
InputButton*   g_button = nullptr;
InputPot*      g_pot = nullptr;

static uint8_t TASK_INPUTS    = 0xFF;
static uint8_t TASK_UPDATE_FAN= 0xFF;
static uint8_t TASK_LOG       = 0xFF;

void doInputTask();
void doFanUpdateTask();
void doLogTask();

void setup()
{
    Serial.begin(115200);
    loadDefaultJsonConfig();

    while (!Serial) { /* wait on native USB; harmless on Mega */ }
    loadConfigFromJson(nullptr);  // uses defaults

    // Initialize fan with config pins
    static FanController fan(
        g_config.fanPwmPin,
        g_config.fanTachPin,
        g_config.pulsesPerRev
    );

    g_fan = &fan;
    g_fan->begin();
    g_fan->setDuty(g_config.presetSpeeds[1]);

    // Initialize inputs
    if (g_config.buttonEnabled)
    {
        static InputButton btn(g_config.buttonPin, true);
        g_button = &btn;
        g_button->begin();
    }
    if (g_config.potEnabled)
    {
        static InputPot pot(g_config.potPin);
        g_pot = &pot;
        g_pot->begin();
    }

    // Register tasks
    TASK_INPUTS     = scheduler.addTask(doInputTask,     g_config.inputScanIntervalMs, true);

    TASK_UPDATE_FAN = scheduler.addTask(doFanUpdateTask, g_config.fanUpdateIntervalMs, true);
    
    TASK_LOG        = scheduler.addTask(doLogTask,       g_config.logIntervalMs,       true);

    // Register tasks with scheduler
    TASK_UPDATE_FAN = scheduler.addTask(
        taskUpdateFan,
        g_config.fanUpdateIntervalMs,
        true
    );
    TASK_SAMPLE_RPM = scheduler.addTask(
        taskSampleRpm,
        g_config.rpmSampleIntervalMs,
        true
    );
    TASK_LOG_STATUS = scheduler.addTask(
        taskLogStatus,
        g_config.logIntervalMs,
        true
    );
}

void loop()
{
    scheduler.update(millis());

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

void doInputTask()
{
    uint32_t now = millis();
    if (g_button) g_button->update(now);
    if (g_pot)    g_pot->update(now);

    if (g_button)
    {
        auto ev = g_button->getEvent();
        switch (ev)
        {
            case InputButton::SINGLE_PRESS:
            {
                static uint8_t idx = 1;
                idx = (idx + 1) % FanConfig::PRESET_COUNT;
                g_fan->setDuty(g_config.presetSpeeds[idx]);
            }
            break;
            case InputButton::DOUBLE_PRESS:
                g_fan->setDuty(0.0f);
                break;
            case InputButton::HOLD:
                g_fan->setDuty(1.0f);
                break;
            default:
                break;
        }
    }

    if (g_pot && g_config.potEnabled)
    {
        float d = g_pot->getDuty();
        g_fan->setDuty(d);
    }
}

void doFanUpdateTask()
{
    g_fan->update(millis());

}

void doLogTask()
{
      // If RPM sampling is already in FanController::update(now),
      // this can be a no-op or used later for additional logic.
      // Kept as a separate hook for future temperature/logic.
}

static void taskLogStatus()
{
      if (!g_fan) return;

      Serial.print(F("Duty="));
      Serial.print(g_fan->getDuty(), 2);
      Serial.print(F(" RPM="));
      Serial.println(g_fan->getRpm());

}
