#include <stdio.h>

#include "board.h"
#include "periph_conf.h"

#include "periph/gpio.h"
#include "periph/i2c.h"
#include "bmx280.h"
#include "dht.h"

#include "shell.h"
#include "shell_lock.h"
#include "string.h"
#include "strings.h"

static bmx280_t bmx280;
static dht_t dht;
#define DHT_GND (GPIO_PIN(PORT_A, 0))
#define DHT_VCC (GPIO_PIN(PORT_A, 2))

static char line_buf[60];

static int get_bme(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    
    int bme_temp_c = bmx280_read_temperature(&bmx280);
    int bme_hum = bme280_read_humidity(&bmx280);
    int bme_temp_f = ((bme_temp_c * 9) / 5) + 3200;

    if (strcmp(argv[1], "-F") == 0)
    {
        printf("BME280: T = %i.%iF, H = %i.%i%%\r\n", bme_temp_f/100, bme_temp_f%10, bme_hum/100, bme_hum%10);
    }
    else
    {
        printf("BME280: T = %i.%iC, H = %i.%i%%\r\n", bme_temp_c/100, bme_temp_c%10, bme_hum/100, bme_hum%10);
    }

    return 0;
}

static int get_dht(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    
    int16_t dht_temp_c, dht_hum, dht_temp_f;

    if (dht_read(&dht, &dht_temp_c, &dht_hum) != DHT_OK)
    {
        printf("Error reading values from DHT!\r\n");
    }
    else if (strcmp(argv[1], "-F") == 0)
    {
        dht_temp_f = ((dht_temp_c * 9) / 5) + 320;
        printf("DHT11:  T = %i.%iF, H = %i.%i%%\r\n", dht_temp_f/10, dht_temp_f%10, dht_hum/10, dht_hum%10);
    }
    else
    {
        printf("DHT11:  T = %i.%iC, H = %i.%i%%\r\n", dht_temp_c/10, dht_temp_c%10, dht_hum/10, dht_hum%10);
    }

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "getbme", "Get the bme280's values", get_bme},
    { "getdht", "Get the dht's values", get_dht},
    { NULL, NULL, NULL }
};

int main(void) {
    gpio_init(DHT_GND, GPIO_OUT);
    gpio_init(DHT_VCC, GPIO_OUT);
    gpio_clear(DHT_GND);
    gpio_set(DHT_VCC);

    i2c_init(I2C_DEV(1));

    const bmx280_params_t bmx280_params = {
        .i2c_dev = I2C_DEV(1),
        .i2c_addr = 0x76,
        .t_sb = BMX280_SB_0_5,
        .filter = BMX280_FILTER_OFF,
        .run_mode = BMX280_MODE_FORCED,
        .temp_oversample = BMX280_OSRS_X1,
        .press_oversample = BMX280_OSRS_X1,
        .humid_oversample = BMX280_OSRS_X1,
    };

    const dht_params_t dht_params = {
        .pin = GPIO_PIN(PORT_A, 1),
        .in_mode = GPIO_IN
    };
    
    if (bmx280_init(&bmx280, &bmx280_params) == BMX280_OK)
    {
        printf("\rBME280 initialized successfully!\r\n");
    }
    else
    {
        printf("\rBME280 initialization failed!\r\n");
    }
    
    if (dht_init(&dht, &dht_params) == DHT_OK)
    {
        printf("\rDHT11 initialized successfully!\r\n");
    }
    else
    {
        printf("\rDHT11 initialization failed!\r\n");
    }

    shell_run(shell_commands, line_buf, sizeof(line_buf));

    while (1)
    {

    }
}
