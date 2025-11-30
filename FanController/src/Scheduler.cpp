#include "Scheduler.h"

Scheduler::Scheduler() : count(0) {}

void Scheduler::every(uint32_t ms, void (*cb)())
{
  if (count >= MAX_TASKS)
    return;
  tasks[count] = {ms, 0, cb};
  count++;
}

void Scheduler::run(uint32_t now)
{
  for (uint8_t i = 0; i < count; i++)
  {
    if (now - tasks[i].lastRun >= tasks[i].interval)
    {
      tasks[i].lastRun = now;
      tasks[i].callback();
    }
  }
}
