#include <Arduino.h>
#include "Config.h"
#include "FanController.h"
#include "InputButton.h"
#include "InputPot.h"
#include "Scheduler.h"
#include "InputButton.h"
#include "InputPot.h"

// Globals
FanController* g_fan = nullptr;
InputButton*   g_button = nullptr;
InputPot*      g_pot = nullptr;
TaskScheduler  scheduler;

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

    loadConfigFromJson(nullptr);  // uses defaults

    // Initialize fan
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
