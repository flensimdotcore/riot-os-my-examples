#include <stdio.h>

#include "board.h"
#include "periph_conf.h"

#include "periph/gpio.h"
#include "periph/i2c.h"
#include "bmx280.h"
#include "dht.h"

#include "shell.h"
#include "shell_lock.h"

static bmx280_t bmx280;
static dht_t dht;
#define DHT_GND (GPIO_PIN(PORT_A, 0))
#define DHT_VCC (GPIO_PIN(PORT_A, 2))

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

    while (1)
    {
        int bme_temp = bmx280_read_temperature(&bmx280);
        int bme_hum = bme280_read_humidity(&bmx280);

        int16_t dht_temp, dht_hum;

        printf("BME280: T = %i.%iC, H = %i.%i%%\t", bme_temp/100, bme_temp%10, bme_hum/100, bme_hum%10);
        if (dht_read(&dht, &dht_temp, &dht_hum) != DHT_OK)
        {
            printf("Error reading values from DHT!");
        }
        else
        {
            printf("DHT11:  T = %i.%iC, H = %i.%i%%\r\n", dht_temp/10, dht_temp%10, dht_hum/10, dht_hum%10);
        }
    }
}