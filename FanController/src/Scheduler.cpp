#include "Scheduler.h"

TaskScheduler::TaskScheduler()
    : _count(0)
{
    for (uint8_t i = 0; i < MAX_TASKS; ++i) {
        _tasks[i].fn = nullptr;
        _tasks[i].intervalMs = 0;
        _tasks[i].lastRunMs = 0;
        _tasks[i].enabled = false;
    }
}

uint8_t TaskScheduler::addTask(TaskFn fn, uint32_t intervalMs, bool enabled)
{
    if (!fn || intervalMs == 0 || _count >= MAX_TASKS) return 0xFF;
    uint8_t idx = _count++;
    _tasks[idx] = { fn, intervalMs, millis(), enabled };
    return idx;
}

void TaskScheduler::setTaskInterval(uint8_t idx, uint32_t intervalMs)
{
    if (idx >= _count || intervalMs == 0) return;
    _tasks[idx].intervalMs = intervalMs;
}

void TaskScheduler::enableTask(uint8_t idx, bool enabled)
{
    if (idx >= _count) return;
    _tasks[idx].enabled = enabled;
}

void TaskScheduler::update(uint32_t nowMs)
{
    for (uint8_t i = 0; i < _count; ++i) {
        auto &t = _tasks[i];
        if (!t.enabled || !t.fn) continue;
        if ((nowMs - t.lastRunMs) >= t.intervalMs) {
            t.lastRunMs = nowMs;
            t.fn();
        }
    }
}
