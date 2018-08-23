#include "os_timers.h"

task_t taskList[MAX_TASKS];            // Список задас
uint8_t taskListTail;                  // Хвіст списку задач

/* Ініціалізація таймерної служби */
inline void os_timers_init()
{
  taskListTail = 0;
}

/* Додавання задачі */
void os_timer_start(void (*function)(const void *argts), uint32_t taskDelay, uint32_t taskPeriod)
{
  if (!function)
  {
    return;
  }
  
  uint8_t i = taskListTail;
  while(i--)
  {
    if (taskList[i].function == function)
    {
      taskList[i].delay  = taskDelay;
      taskList[i].period = taskPeriod;
      return;
    }
  }
  
  if (taskListTail < MAX_TASKS)
  {
    taskList[taskListTail].function  = function;
    taskList[taskListTail].delay  = taskDelay;
    taskList[taskListTail].period = taskPeriod;
    ++taskListTail;
  }
}

/* Встановлення аргументів для задачі */
void os_timer_set_arg(void (*function)(const void *args), void * args)
{
  uint8_t i = taskListTail;
  while(i--)
  {
    if (taskList[i].function == function)
    {
      taskList[i].arguments = args;
    }
  }
}

/* Видалення задачі */
void os_timer_stop(void (*function)(const void *args))
{
  uint8_t i = taskListTail;
  while(i--)
  {
    if (taskList[i].function == function)
    {
      if (i != (taskListTail - 1))
      {
        taskList[i] = taskList[taskListTail - 1];
      }
      
      --taskListTail;
      return;
    }
  }
}

/* Системний тік, викликається з періодом 1 мс */
void os_timer_tick()
{
  uint8_t i = taskListTail;
  while(i--)
  {
    if(taskList[i].delay)
    {
      --taskList[i].delay;
    }
  }
}

/* Диспетчер таймерної служби, викликається з основного циклу */
void os_timer_dispatcher()
{
  uint8_t i = taskListTail;
  while(i--)
  {
    if (taskList[i].delay == 0)
    {
      taskList[i].function(taskList[i].arguments);
      
      if(taskList[i].period != 0)
      {
        taskList[i].delay = taskList[i].period;
      }
      else if (taskList[i].delay == 0) // Сама ф-я може змінити свій delay
      {
        os_timer_stop(taskList[i].function);
      }
    }
  }
}

uint8_t os_timer_is_active(void (*taskFunc)(const void * args))
{
  uint8_t i = taskListTail;
  while(i--) {
    if (taskList[i].function == taskFunc) {
      return 1;
    }
  }
  return 0;
}

