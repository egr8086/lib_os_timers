#ifndef OS_TIMERS_H
#define OS_TIMERS_H

#include <stdint.h>

#ifdef __cplusplus
  extern "C" {
#endif

typedef void (*timer_cb)(const void * args);
    
void os_timers_init(void);
void os_timer_start(timer_cb taskFunc, uint32_t taskDelay, uint32_t taskPeriod);
void os_timer_stop(timer_cb taskFunc);
void os_timer_set_arg(timer_cb taskFunc, void * args);
void os_timer_tick(void);
void os_timer_dispatcher(void);
uint8_t os_timer_is_active(timer_cb taskFunc);

#ifdef __cplusplus
  }
#endif

#endif
