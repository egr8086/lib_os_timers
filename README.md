# Software timers library

Used to call functions with a given periodicity or one-time
through a specified period.

## Main concept
The structure of the task contains a pointer to the function (\* f), its arguments (void \* args), the current delay before the next call (delay) and the value of the call period (period). All tasks are merged into an array.

The os_timer_tick () function calls every millisecond from the timer interrupt, such as SysTick for STM32. In this function for all tasks delay decreases by 1.

The os_timer_dispatcher () function calls from main loop. This function calls all functions tasks which delay = 0.

## Integration

1. From timer ISR call os_timer_tick()
2. From main loop call os_timer_dispatcher()
3. Implement timer functions in user code
4. Use os_timer_start(f, delay, period) for charge timer, os_timer_stop(f) for stop timer

## Examples
```c
void led_green_toggle_timer(const void * args)
{
	led_green_toggle();
}

void led_red_toggle_timer(const void * args)
{
	led_red_toggle();
}

void system_start_delay_complete(const void * args)
{
	/* Stop red led blinking after system started */
	os_timer_stop(led_red_toggle_timer);
	/* Start user application */
	start application();
}

int main(void)
{
	/* System init */
	
	/* Toggle green led every 100 ms. Start blinking write now (delay = 0) */
	os_timer_start(led_green_toggle_timer, 0, 100);
	/* Toggle red led every 20 ms, start blink after 10 ms */
	os_timer_start(led_red_toggle_timer, 10, 20);
	/* Start user application after 200 ms. After execution task will be removed automatically because period = 0 */
	os_timer_start(system_start_delay_complete, 200, 0);
	while(1) {
		os_timer_dispatcher();
	}
}
```
