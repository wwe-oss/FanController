#include "Scheduler.h"

TaskScheduler::TaskScheduler()
    : _taskCount(0)
{
    for (uint8_t i = 0; i < MAX_TASKS; ++i)
    {
        _tasks[i].fn         = nullptr;
        _tasks[i].intervalMs = 0;
        _tasks[i].lastRunMs  = 0;
        _tasks[i].enabled    = false;
    }
}

uint8_t TaskScheduler::addTask(TaskFn fn, uint32_t intervalMs, bool enabled)
{
    if (fn == nullptr || intervalMs == 0 || _taskCount >= MAX_TASKS)
    {
        return 0xFF;
    }

    uint8_t idx = _taskCount++;
    _tasks[idx].fn         = fn;
    _tasks[idx].intervalMs = intervalMs;
    _tasks[idx].lastRunMs  = millis();
    _tasks[idx].enabled    = enabled;

    return idx;
}

void TaskScheduler::setTaskInterval(uint8_t index, uint32_t intervalMs)
{
    if (index >= _taskCount || intervalMs == 0) return;
    _tasks[index].intervalMs = intervalMs;
}

void TaskScheduler::enableTask(uint8_t index, bool enabled)
{
    if (index >= _taskCount) return;
    _tasks[index].enabled = enabled;
}

void TaskScheduler::update(uint32_t nowMs)
{
    for (uint8_t i = 0; i < _taskCount; ++i)
    {
        Task& t = _tasks[i];
        if (!t.enabled || t.fn == nullptr) continue;

        uint32_t elapsed = nowMs - t.lastRunMs;
        if (elapsed >= t.intervalMs)
        {
            t.lastRunMs = nowMs;
            t.fn();
        }
    }
}
