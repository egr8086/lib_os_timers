#ifndef OS_TIMERS_H
#define OS_TIMERS_H

#include <stdint.h>

#ifdef __cplusplus
  extern "C" {
#endif

#define MAX_TASKS      24              // Максимальна кількість задач

/* Структура задачі */
typedef struct
{
  void (*function)(const void * args);     // Вказівник на ф-ю таймера
  void * arguments;                        // Аргументи ф-ї
  uint32_t delay;                          // Затримка
  uint32_t period;                         // период запуска задачи
}task_t;

void os_timers_init(void);
void os_timer_start(void (*taskFunc)(const void * args), uint32_t taskDelay, uint32_t taskPeriod);
void os_timer_stop(void (*taskFunc)(const void * args));
void os_timer_set_arg(void (*taskFunc)(const void *args), void * args);
void os_timer_tick(void);
void os_timer_dispatcher(void);

#ifdef __cplusplus
  }
#endif

#endif
