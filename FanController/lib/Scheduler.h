#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <Arduino.h>

struct Task
{
  uint32_t interval;
  uint32_t lastRun;
  void (*callback)();
};

class Scheduler
{
public:
  Scheduler();

  void every(uint32_t ms, void (*cb)());
  void run(uint32_t now);

private:
  static constexpr uint8_t MAX_TASKS = 8;
  Task tasks[MAX_TASKS];
  uint8_t count;
};

#endif
