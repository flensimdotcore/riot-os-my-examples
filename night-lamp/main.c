#include <stdio.h>

#include "board.h"
#include "periph_conf.h"

#include "periph/gpio.h"
#include "periph/adc.h"
#include "periph/pwm.h"

#include "ztimer.h"

#define ADC ADC_LINE(5)
#define LED_THRESHOLD 2000

#define PWM_FREQ 1024
#define PWM_RES 1024

int main(void) {
    int16_t brightness = 0;
    pwm_t pwm = 0;

    if (pwm_init(PWM_DEV(pwm), PWM_LEFT, PWM_FREQ, PWM_RES) == 0)
    {
        printf("\rPWM initialization failure!\r\n");
    }
    else
    {
        printf("\rPWM initialized successfully!\r\n");
    }

    if (adc_init(ADC) < 0)
    {
        printf("\rADC initialization failure!\r\n");
    }
    else 
    {
        printf("\rADC initialized successfully!\r\n");
    }

    while (1)
    {
        brightness = adc_sample(ADC, ADC_RES_12BIT);
        uint16_t pwm_value = (brightness < 500) ? 0 : brightness;
        pwm_set(PWM_DEV(pwm), 0, pwm_value);
        pwm_set(PWM_DEV(pwm), 1, pwm_value);
        pwm_set(PWM_DEV(pwm), 2, pwm_value);
        printf("%i\r\n", (brightness < 1000) ? 0 : brightness);
        ztimer_sleep(ZTIMER_MSEC, 10);
    }
}