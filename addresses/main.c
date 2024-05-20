#include <stdio.h>

#include "board.h"
#include "periph_conf.h"

#include "periph/gpio.h"
#include "periph/gpio_ll_irq.h"
#include "xtimer.h"

uint8_t global_variable = 0;
static uint8_t global_static_variable = 0;
static const uint8_t global_static_constant = 0;

static void button_handler(void *arg);
static void function(void);

int main(void) {
    static uint8_t local_static_variable = 0;
    uint8_t local_variable = 0;
    const uint8_t local_constant = 0;
    
    gpio_init(BTN0_PIN, GPIO_IN_PD);
    gpio_init_int(BTN0_PIN, GPIO_IN, GPIO_BOTH, button_handler, NULL);

    printf("\r\n");
    printf("Global variable's address is %p\r\n", &global_variable);
    printf("Global static variable's address is %p\r\n", &global_static_variable);
    printf("Global static constant's address is %p\r\n", &global_static_constant);
    printf("Local static variable's address is %p\r\n", &local_static_variable);
    printf("Local variable's address is %p\r\n", &local_variable);
    printf("Local constant's address is %p\r\n", &local_constant);
    printf("Interrupt handler's address is %p\r\n", &button_handler);
    printf("Function's address is %p\r\n", &function);
    
    function();

    while (1)
    {
        
    }

    return 0;
}

static void function(void)
{
    int i = 0;
    i++;
}

static void button_handler(void *arg)
{
    (void)arg;
    gpio_irq_disable(BTN0_PIN);
    uint8_t int_handler_variable = 0;
    printf("Interrupt handler variable's address is %p", &int_handler_variable);
    gpio_irq_enable(BTN0_PIN);
}