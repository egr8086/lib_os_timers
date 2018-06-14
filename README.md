# Бібліотека програмних таймерів

Використовується для виклику функцій із заданою періодичністю або одноразово
через заданий період.

## Принцип роботи

Структура задачі містить в собі вказівник на функцію (*f), її аргументи
(void * args), поточне значення затримки перед наступним викликом (delay)
і значення періоду виклику (period).
Всі задачі об’єднуються в масив розмірністю MAX_TASKS.

Функція os_timer_tick() має викликатись з періодичністю SysTick,
наприклад, 1 мс (з переривання таймера, наприклад SysTick для STM32). В цій
функції для всіх задач delay зменшується на 1, якщо він більший 0.

Функція os_timer_dispatcher() має викликатись з періодичністю не більше SysTick,
наприклад в цикли while(1). В цій функції відбувається виклик всіх функцій
задач (*f), для яких delay == 0.

## Інтеграція в проект

1. Налаштувати таймер на періодичне спрацювання із заданим періодом, наприклад,
1 мс. Ініціалізацію таймера можна помістити в функцію os_timers_init() і 
викликати її з main(). Для STM32 доцільно використати таймер SysTick.
2. З функції переривання таймера викликати функцію os_timer_tick()
3. У "вічному циклі" в функції main викликати функцію os_timer_dispatcher()
4. Для створення задачі таймера викликати os_timer_start((*f), delay, period);
5. Для видалення задачі викликати os_timer_stop((*f))
6. Задачі, для яких period == 0 , будуть виконані один раз при досягненні
умови delay == 0 і будуть автоматично видалені.
7. Для зміни аргументів, з якими буде викликатись функція задачі,
викликати os_timer_set_arg(). Тип аргументів функції таймера - void*, тому
функції можна передати вказівник на будь-який тип (примітивний тип, масив чи 
структура), але для роботи з аргументами в тілі функції потрібно реалізувати
приведення до конкретного типу. В даному випадку відсутній контроль типів
компілятором.

## Приклади використання

### 1. Блимання 2-ма світлодіодами з періодом 150 мс і 500 мс.
```
static void led_blink_1(const void * args) {
	HAL_GPIO_TogglePin(LED_1_PORT, LED_1_PIN);
}

static void led_blink_2(const void * args) {
	HAL_GPIO_TogglePin(LED_2_PORT, LED_2_PIN);
}

ind main(void) {
...
os_timers_init();
os_timer_start(led_blink_1, 0, 150);
os_timer_start(led_blink_2, 0, 500);
}

while(1) {
	os_timer_dispatcher();
}
```
### 2. Перемикання перекидного реле

Для перемикання перекидного реле на один з його контактів потрібно подати 
імпульс тривалістю 20 мс.

```
static void relay_on_impulse_stop(const void * args) {
	HAL_GPIO_WritePin(RELAY_ON_PORT, RELAY_ON_PIN, GPIO_PIN_RESET);
}

static void relay_off_impulse_stop(const void * args) {
	HAL_GPIO_WritePin(RELAY_OFF_PORT, RELAY_OFF_PIN, GPIO_PIN_RESET);
}

void relay_on(void)
{
	HAL_GPIO_WritePin(RELAY_ON_PORT, RELAY_ON_PIN, GPIO_PIN_SET);
	os_timer_start(relay_on_impulse_stop, 20, 0);
}

void relay_off(void)
{
	HAL_GPIO_WritePin(RELAY_OFF_PORT, RELAY_OFF_PIN, GPIO_PIN_SET);
	os_timer_start(relay_off_impulse_stop, 20, 0);
}

```

### 3. Зміна аргументів функції таймера

Вивести меандр частотою 100 Гц спочатку на А0, потім на А1.

```
typedef struct {
	GPIO_TypeDef * port;
	uint16_t pin;
}gpio_s;

gpio_s pin0 = {.port = GPIOA, .pin = 0};
gpio_s pin1 = {.port = GPIOA, .pin = 1};

static void meandr(const void * args)
{
	if(!args) return;
	gpio_s * pin = (gpio_s*)args;
	HAL_GPIO_TogglePin(pin->port, pin->pin);
}

...
	os_timer_start(meandr, 0, 5);
	os_timer_set_arg(meandr, &pin1);
...
	os_timer_set_arg(meandr, &pin2);

```


	