#ifndef OS_TIMERS_H
#define OS_TIMERS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*timer_cb)(const void * args);

/* Module initialization */
void os_timers_init(void);
/* Start timer */
void os_timer_start(timer_cb taskFunc, uint32_t taskDelay, uint32_t taskPeriod);
/* Stop timer */
void os_timer_stop(timer_cb taskFunc);
/* Change timer function arguments */
void os_timer_set_arg(timer_cb taskFunc, void * args);
/* Change timer counters. Call from timer ISR */
void os_timer_tick(void);
/* Timer dispatcher. Call from main forever loop */
void os_timer_dispatcher(void);
/* Check is timer active */
uint8_t os_timer_is_active(timer_cb taskFunc);

#ifdef __cplusplus
}
#endif

#endif
