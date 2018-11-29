#include "os_timers.h"

/* Task structure (size = 16 bytes) */
typedef struct
{
  /* Timer function */
  timer_cb function;
  /* Timer function arguments */
  void * arguments;
  /* Delay before next function call. Decreased in every tick */
  uint32_t delay;
  /* Period of timer function calling.
  If period == 0 task will be remove automatically after first function call */
  uint32_t period;
}task_t;

/* Maximum tasks count. */
#define MAX_TASKS      24

/* Task list. Might be use dynamic array (aka vector) in projects with RTOS or malloc */
static task_t taskList[MAX_TASKS];

static uint8_t taskListTail;

/* Module static memory usage = MAX_TASKS * sizeof(task_t) + 1 */

static task_t * find_task_by_function(timer_cb function)
{
  for(uint8_t i = 0; i < taskListTail; ++i) {
    if(taskList[i].function == function) {
      return &taskList[i];
    }
  }
  return 0;
}

void os_timers_init(void)
{
  taskListTail = 0;
}

void os_timer_start(timer_cb taskFunc, uint32_t taskDelay, uint32_t taskPeriod)
{
  if (!taskFunc) {
    return;
  }

  /* Update time parameters if function is present in list */
  task_t * task = find_task_by_function(taskFunc);
  if(task) {
    task->delay  = taskDelay;
    task->period = taskPeriod;
    return;
  }

  /* Add function to list if its not present */
  if (taskListTail < MAX_TASKS) {
    taskList[taskListTail].function  = taskFunc;
    taskList[taskListTail].delay  = taskDelay;
    taskList[taskListTail].period = taskPeriod;
    ++taskListTail;
  }
}

void os_timer_set_arg(timer_cb taskFunc, void * args)
{
  task_t * task = find_task_by_function(taskFunc);
  if(task) {
    task->arguments = args;
  }
}

void os_timer_stop(timer_cb taskFunc)
{
  for(uint8_t i = 0; i < taskListTail; ++i) {
    if (taskList[i].function == taskFunc) {
      /* Replace removed function by last function in array */
      if (i != (taskListTail - 1)) {
        taskList[i] = taskList[taskListTail - 1];
      }
      --taskListTail;
      return;
    }
  }
}

void os_timer_tick(void)
{
  for(uint8_t i = 0; i < taskListTail; ++i) {
    if(taskList[i].delay) {
      --taskList[i].delay;
    }
  }
}

void os_timer_dispatcher(void)
{
  for(uint8_t i = 0; i < taskListTail; ++i) {
    task_t * task = &taskList[i];
    if (task->delay == 0) {
      task->function(taskList[i].arguments);

      if(task->period != 0) {
        task->delay = task->period;
      }
      /* Function can change its own delay */
      else if (task->delay == 0) {
        os_timer_stop(task->function);
      }
    }
  }
}

uint8_t os_timer_is_active(timer_cb taskFunc)
{
  return find_task_by_function(taskFunc) != 0;
}
