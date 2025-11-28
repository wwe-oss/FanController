#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <Arduino.h>

class TaskScheduler
{
public:
    typedef void (*TaskFn)();

    struct Task
    {
        TaskFn   fn;
        uint32_t intervalMs;
        uint32_t lastRunMs;
        bool     enabled;
    };

    static const uint8_t MAX_TASKS = 8;

    TaskScheduler();

    // Returns index of the task, or 0xFF if no slot.
    uint8_t addTask(TaskFn fn, uint32_t intervalMs, bool enabled = true);

    void setTaskInterval(uint8_t index, uint32_t intervalMs);
    void enableTask(uint8_t index, bool enabled);

    // Call from loop() with millis()
    void update(uint32_t nowMs);

private:
    Task    _tasks[MAX_TASKS];
    uint8_t _taskCount;
};

#endif // SCHEDULER_H
