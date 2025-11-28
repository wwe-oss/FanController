#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <Arduino.h>

class TaskScheduler
{
public:
    typedef void (*TaskFn)();
    struct Task { TaskFn fn; uint32_t intervalMs; uint32_t lastRunMs; bool enabled; };

    static const uint8_t MAX_TASKS = 8;

    TaskScheduler();

    uint8_t addTask(TaskFn fn, uint32_t intervalMs, bool enabled = true);
    void setTaskInterval(uint8_t idx, uint32_t intervalMs);
    void enableTask(uint8_t idx, bool enabled);
    void update(uint32_t nowMs);

private:
    Task _tasks[MAX_TASKS];
    uint8_t _count;
};

#endif // SCHEDULER_H
