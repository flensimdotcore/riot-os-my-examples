#include <stdio.h>

#include "board.h"
#include "periph_conf.h"

#include "periph/gpio.h"
#include "periph/gpio_ll_irq.h"
#include "xtimer.h"

#define RED_LED_TIME_SEC (10)
#define GREEN_LED_TIME_SEC (3)
#define RED_LED_THRESHOLD_TIME_SEC (3)

#define BTN_DEBOUNCE_TIME_MS (50U)

typedef enum
{
    TL_RED_LED,
    TL_GREEN_LED
} tl_state_t;

static tl_state_t traffic_light_state = TL_RED_LED;

static ztimer_t timer[3];

static gpio_t RED_LED_PIN = LED1_PIN;
static gpio_t GREEN_LED_PIN = LED0_PIN;
static gpio_t BTN_PIN = BTN0_PIN;

static ztimer_now_t led_last_time = 0;

static void button_is_pressed(void *arg);

static void debounce_timer_callback(void *arg);
static void green_led_timer_callback(void *arg);
static void red_led_timer_callback(void *arg);

int main(void) {
    gpio_init(RED_LED_PIN, GPIO_OUT);
    gpio_init(GREEN_LED_PIN, GPIO_OUT);
    gpio_init(BTN_PIN, GPIO_IN_PD);
    gpio_init_int(BTN_PIN, GPIO_IN, GPIO_RISING, button_is_pressed, NULL);
    gpio_irq_enable(BTN_PIN);

    timer[0].callback = green_led_timer_callback;
    timer[1].callback = red_led_timer_callback;
    timer[2].callback = debounce_timer_callback;

    ztimer_set(ZTIMER_SEC, &timer[0], RED_LED_TIME_SEC);
    gpio_set(RED_LED_PIN);

    while (1)
    {

    }
}

static void button_is_pressed(void *arg)
{
    (void)arg;
    gpio_irq_disable(BTN0_PIN);
    
    ztimer_acquire(ZTIMER_SEC);
    if (traffic_light_state == TL_RED_LED && ztimer_now(ZTIMER_SEC) - led_last_time < RED_LED_TIME_SEC - RED_LED_THRESHOLD_TIME_SEC)
    {
        ztimer_remove(ZTIMER_SEC, &timer[0]);
        ztimer_set(ZTIMER_SEC, &timer[0], RED_LED_THRESHOLD_TIME_SEC);
    }

    ztimer_set(ZTIMER_MSEC, &timer[2], BTN_DEBOUNCE_TIME_MS);
}

static void debounce_timer_callback(void *arg)
{
    (void)arg;
    gpio_irq_enable(BTN0_PIN);
    ztimer_remove(ZTIMER_MSEC, &timer[2]);
}

static void green_led_timer_callback(void *arg)
{
    (void)arg;
    ztimer_remove(ZTIMER_SEC, &timer[0]);
    gpio_clear(RED_LED_PIN);
    gpio_set(GREEN_LED_PIN);
    traffic_light_state = TL_GREEN_LED;
    ztimer_set(ZTIMER_SEC, &timer[1], GREEN_LED_TIME_SEC);
}

static void red_led_timer_callback(void *arg)
{
    (void)arg;

    ztimer_acquire(ZTIMER_SEC);
    led_last_time = ztimer_now(ZTIMER_SEC);

    ztimer_remove(ZTIMER_SEC, &timer[1]);
    gpio_clear(GREEN_LED_PIN);
    gpio_set(RED_LED_PIN);
    traffic_light_state = TL_RED_LED;
    ztimer_set(ZTIMER_SEC, &timer[0], RED_LED_TIME_SEC);
}