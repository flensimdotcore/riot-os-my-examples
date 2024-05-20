#include <stdio.h>

#include "board.h"
#include "periph_conf.h"

#include "periph/gpio.h"
#include "periph/gpio_ll_irq.h"
#include "xtimer.h"

// #define MORSE_STRING "ABCDEFGHIJKLMNOPQRS TUVWXYZ"
#define MORSE_STRING "SOS"

#define BLANK_INSIDE_LETTER (uint8_t)(1)
#define BLANK_BETWEEN_LETTERS (uint8_t)(3 - BLANK_INSIDE_LETTER) 
#define BLANK_BETWEEN_WORDS (uint8_t)(5 - BLANK_BETWEEN_LETTERS)

#define DOT 0b10
#define DASH 0b11
#define BLANK 0b00
static uint8_t morse_code_current = 0b00000000;

static const uint32_t TIME_UNIT = 100;
static const uint32_t TIME_DOT = 1 * TIME_UNIT;
static const uint32_t TIME_DASH = 3 * TIME_UNIT;

void problem_4(gpio_t led_pin, gpio_t btn_pin);
static void button_is_pressed(void *arg);

static void morse_code(char *morse_str);
static void parse_morse_byte(void);
static void send_dot(uint8_t dots_amount);
static void send_dash(uint8_t dashes_amount);
static void send_blank(uint8_t blanks_amount);

static void _light_led(gpio_t led_pin, bool led_state, uint32_t time_ms);

int main(void) {
    problem_4(LED0_PIN, BTN0_PIN);

    return 0;
}

void problem_4(gpio_t led_pin, gpio_t btn_pin)
{
    gpio_init(led_pin, GPIO_OUT);
    gpio_init(btn_pin, GPIO_IN_PD);
    gpio_init_int(btn_pin, GPIO_IN, GPIO_RISING, button_is_pressed, NULL);
    gpio_irq_enable(btn_pin);

    while (1)
    {
        morse_code(MORSE_STRING);
        xtimer_msleep(1000);
    }
}

static void button_is_pressed(void *arg)
{
    static uint32_t last_time = 0;

    gpio_irq_disable(BTN0_PIN);
    puts(arg);

    if (xtimer_now_usec() - last_time > 200)
    {
        last_time = xtimer_now_usec();
    }

    gpio_irq_enable(BTN0_PIN);
}

static void morse_code(char *morse_str)
{
    static uint8_t ascii_A = 65;
    static const uint8_t ascii_morse_table[26] = {
        0b10110000, // A
        0b11100000, // B
        0b11101110, // C
        0b11101000, // D
        0b10000000, // E
        0b10101110, // F
        0b11111000, // G
        0b10101010, // H
        0b10100000, // I
        0b10111111, // J
        0b11101100, // K
        0b10111010, // L
        0b11110000, // M
        0b11100000, // N
        0b11111100, // O
        0b10111110, // P
        0b11111011, // Q
        0b10111000, // R
        0b10101000, // S
        0b11000000, // T
        0b10101100, // U
        0b10101011, // V
        0b10111100, // W
        0b11101011, // X
        0b11101111, // Y
        0b11111010, // Z
    };

    int i = 0;

    while (morse_str[i] != '\0')
    {
        for (int j = 0; j < 26; j++)
        {
            if ((int)morse_str[i] == ascii_A + j)
            {
                morse_code_current = ascii_morse_table[j];
                send_blank(BLANK_BETWEEN_LETTERS);
                break;
            }
            if ((int)morse_str[i] == 32)
            {
                send_blank(2);
            }
        }
        parse_morse_byte();
        i++;
    }
}

static void parse_morse_byte(void)
{
    for (int i = 3; i >= 0; i--)
    {
        if (((morse_code_current & (0b11 << i*2)) >> i*2) == DOT)
        {
            send_dot(1);
        } else if (((morse_code_current & (0b11 << i*2)) >> i*2) == DASH)
        {
            send_dash(1);
        }
    }
}

static void send_dot(uint8_t dots_amount)
{
    for (int i = 0; i < dots_amount; i++)
    {
        _light_led(LED0_PIN, 1, TIME_DOT);
        send_blank(BLANK_INSIDE_LETTER);
    }
}

static void send_dash(uint8_t dashes_amount)
{
    for (int i = 0; i < dashes_amount; i++)
    {
        _light_led(LED0_PIN, 1, TIME_DASH);
        send_blank(BLANK_INSIDE_LETTER);
    }
}

static void send_blank(uint8_t blanks_amount)
{
    _light_led(LED0_PIN, 0, blanks_amount * TIME_UNIT);
}

static void _light_led(gpio_t led_pin, bool led_state, uint32_t time_ms)
{
    led_state ? gpio_set(led_pin) : gpio_clear(led_pin);
    xtimer_msleep(time_ms);
    gpio_clear(led_pin);
}