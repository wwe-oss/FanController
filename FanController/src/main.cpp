#include <Arduino.h>
#include "Config.h"
#include "FanController.h"
#include "Scheduler.h"

// Global scheduler
TaskScheduler scheduler;

// Global fan
FanController* g_fan = nullptr;

// --- Task indices (for future reference if needed) ---
static uint8_t TASK_UPDATE_FAN   = 0xFF;
static uint8_t TASK_SAMPLE_RPM   = 0xFF;
static uint8_t TASK_LOG_STATUS   = 0xFF;
// Input-related tasks (button/pot) can be added later.

static void taskUpdateFan();
static void taskSampleRpm();
static void taskLogStatus();

void setup()
{
    Serial.begin(115200);
    while (!Serial) { /* wait on native USB; harmless on Mega */ }

    Serial.println(F("FanController with JSON config + scheduler starting..."));

    // Load config: defaults + JSON
    bool ok = loadConfigFromJson(nullptr); // will just apply defaults
    // If you want to use DEFAULT_CONFIG_JSON instead:
    // bool ok = loadConfigFromJson(DEFAULT_CONFIG_JSON);
    (void)ok; // unused for now, but you can log it

    // Initialize fan with config pins
    static FanController fan(
        g_config.fanPwmPin,
        g_config.fanTachPin,
        g_config.pulsesPerRev
    );
    g_fan = &fan;

    g_fan->begin();
    // Start at first preset speed
    g_fan->setDuty(g_config.presetSpeeds[1]); // e.g. 30%

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
    uint32_t now = millis();
    scheduler.update(now);
}

// --- Tasks ---

static void taskUpdateFan()
{
    if (!g_fan) return;
    // For now just keep PWM applied + internal updates
    // If FanController::update needs millis, you can extend it accordingly.
    // In the earlier design, update() used millis for RPM sampling.
    uint32_t now = millis();
    g_fan->update(now);
}

static void taskSampleRpm()
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
