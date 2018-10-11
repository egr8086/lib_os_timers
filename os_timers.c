#include "os_timers.h"

#define MAX_TASKS      24              // Максимальна кількість задач

/* Структура задачі */
typedef struct
{
  timer_cb function;     // Вказівник на ф-ю таймера
  void * arguments;                        // Аргументи ф-ї
  uint32_t delay;                          // Затримка
  uint32_t period;                         // период запуска задачи
}task_t;

task_t taskList[MAX_TASKS];            // Список задас
uint8_t taskListTail;                  // Хвіст списку задач

/* Ініціалізація таймерної служби */
inline void os_timers_init()
{
  taskListTail = 0;
}

/* Додавання задачі */
void os_timer_start(timer_cb taskFunc, uint32_t taskDelay, uint32_t taskPeriod)
{
  if (!taskFunc) {
    return;
  }
  
  for(uint8_t i = 0; i < taskListTail; ++i) {
    if (taskList[i].function == taskFunc) {
      taskList[i].delay  = taskDelay;
      taskList[i].period = taskPeriod;
      return;
    }
  }
  
  if (taskListTail < MAX_TASKS) {
    taskList[taskListTail].function  = taskFunc;
    taskList[taskListTail].delay  = taskDelay;
    taskList[taskListTail].period = taskPeriod;
    ++taskListTail;
  }
}

/* Встановлення аргументів для задачі */
void os_timer_set_arg(timer_cb taskFunk, void * args)
{
  for(uint8_t i = 0; i < taskListTail; ++i) {
    if (taskList[i].function == taskFunk) {
      taskList[i].arguments = args;
    }
  }
}

/* Видалення задачі */
void os_timer_stop(timer_cb taskFunc)
{
  for(uint8_t i = 0; i < taskListTail; ++i) {
    if (taskList[i].function == taskFunc) {
      if (i != (taskListTail - 1)) {
        taskList[i] = taskList[taskListTail - 1];
      }
      --taskListTail;
      return;
    }
  }
}

/* Системний тік, викликається з періодом 1 мс */
void os_timer_tick(void)
{
  for(uint8_t i = 0; i < taskListTail; ++i) {
    if(taskList[i].delay) {
      --taskList[i].delay;
    }
  }
}

/* Диспетчер таймерної служби, викликається з основного циклу */
void os_timer_dispatcher(void)
{
  for(uint8_t i = 0; i < taskListTail; ++i) {
    if (taskList[i].delay == 0) {
      taskList[i].function(taskList[i].arguments);
      
      if(taskList[i].period != 0) {
        taskList[i].delay = taskList[i].period;
      }
      // Сама ф-я може змінити свій delay
      else if (taskList[i].delay == 0) {
        os_timer_stop(taskList[i].function);
      }
    }
  }
}

uint8_t os_timer_is_active(timer_cb taskFunc)
{
  for(uint8_t i = 0; i < taskListTail; ++i) {
    if (taskList[i].function == taskFunc) {
      return 1;
    }
  }
  return 0;
}
