#include <stdio.h>

#include "board.h"
#include "periph_conf.h"

#include "periph/gpio.h"
#include "periph/gpio_ll_irq.h"
#include "xtimer.h"
#include "atomic_utils.h"

#define BTN_LONG_PRESS_TIME_MS (1500U)
#define LED_BLINK_PERIOD_500_MS (500 * 1000U)
#define LED_BLINK_PERIOD_1000_MS (1000 * 1000U)
#define BTN_DEBOUNCE_TIME_MS (50U)

static bool led_state = 0;
static uint8_t led_freq = 0;
static bool btn_long_release = 0;

void problem_1_i(gpio_t led_pin, gpio_t btn_pin);
void problem_1_ii(gpio_t led_pin, gpio_t btn_pin);
static void button_handler(void *arg);

static void led_toggle_on_click(gpio_t led_pin, gpio_t btn_pin);
static void led_state_handler(gpio_t led_pin);

static void debounce_timer_callback(void *arg);
static void long_press_timer_callback(void *arg);

static ztimer_t timer[2];

int main(void) {
    problem_1_ii(LED0_PIN, BTN0_PIN);

    return 0;
}

void problem_1_i(gpio_t led_pin, gpio_t btn_pin)
{
    gpio_init(led_pin, GPIO_OUT);
    gpio_init(btn_pin, GPIO_IN_PD);

    while (1) 
    {
        led_toggle_on_click(led_pin, btn_pin);
    }
}

static void led_toggle_on_click(gpio_t led_pin, gpio_t btn_pin)
{
    static bool new_btn_state = 0;
    static bool prev_btn_state = 0;

    new_btn_state = gpio_read(btn_pin) > 0 ? 1 : 0;
    if (new_btn_state == 1 && prev_btn_state == 0)
    {
        gpio_toggle(led_pin);
    }
    prev_btn_state = new_btn_state;
}

void problem_1_ii(gpio_t led_pin, gpio_t btn_pin)
{
    gpio_init(led_pin, GPIO_OUT);
    gpio_init(btn_pin, GPIO_IN_PD);
    gpio_init_int(btn_pin, GPIO_IN, GPIO_BOTH, button_handler, NULL);
    timer[0].callback = debounce_timer_callback;
    timer[1].callback = long_press_timer_callback;

    gpio_irq_enable(btn_pin);

    while (1)
    {
        led_state_handler(led_pin);
    }
}

static void led_state_handler(gpio_t led_pin)
{
    static uint32_t led_last_time = 0;
    static uint32_t led_blink_period = 0;

    led_blink_period = (atomic_load_u8(&led_freq) == 0) ? LED_BLINK_PERIOD_500_MS : LED_BLINK_PERIOD_1000_MS;

    if (led_state == 1 && ((xtimer_now_usec() < led_last_time) ? UINT32_MAX - led_last_time + xtimer_now_usec() : xtimer_now_usec() - led_last_time) > led_blink_period)
    {
        gpio_toggle(led_pin);
        led_last_time = xtimer_now_usec();
    }
    else if (led_state == 0)
    {
        gpio_clear(led_pin);
    }
}

static void button_handler(void *arg)
{
    (void)arg;

    if (gpio_read(BTN0_PIN) == 1)
    {
        gpio_irq_disable(BTN0_PIN);
        ztimer_set(ZTIMER_MSEC, &timer[0], BTN_DEBOUNCE_TIME_MS);
        ztimer_set(ZTIMER_MSEC, &timer[1], BTN_LONG_PRESS_TIME_MS);
    }
    else if (gpio_read(BTN0_PIN) == 0)
    {
        gpio_irq_disable(BTN0_PIN);
        ztimer_set(ZTIMER_MSEC, &timer[0], BTN_DEBOUNCE_TIME_MS);
        
        if (btn_long_release == 0)
        {
            led_state = !led_state;
            ztimer_remove(ZTIMER_MSEC, &timer[1]);  
        }
        else
        {
            atomic_store_u8(&led_freq, !led_freq);
            // led_freq = !led_freq;
            btn_long_release = 0;
        }
    }
}

static void debounce_timer_callback(void *arg)
{
    (void)arg;
    ztimer_remove(ZTIMER_MSEC, &timer[0]);
    gpio_irq_enable(BTN0_PIN);
}

static void long_press_timer_callback(void *arg)
{
    (void)arg;
    ztimer_remove(ZTIMER_MSEC, &timer[1]);

    if (gpio_read(BTN0_PIN) == 1)
    {
        btn_long_release = 1;
    }
}