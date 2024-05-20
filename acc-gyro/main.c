#include <stdio.h>

#include "board.h"
#include "periph_conf.h"

#include "periph/gpio.h"
#include "periph/i2c.h"
#include "lsm6ds3.h"
#include "lsm6ds3_regs.h"
#include "xtimer.h"

lsm6ds3_t lsm;
#define LSM_CS (GPIO_PIN(PORT_B, 1))
#define LSM_SA0 (GPIO_PIN(PORT_B, 0))

#define KICK_THRESHOLD 200

int main(void) {
    gpio_init(LSM_CS, GPIO_OUT);
    gpio_init(LSM_SA0, GPIO_OUT);
    gpio_set(LSM_CS);
    gpio_clear(LSM_SA0);
    i2c_init(I2C_DEV(1));

    lsm.params.i2c = I2C_DEV(1);
    lsm.params.i2c_addr = 0x6B;

    lsm.params.gyro_enabled = true;
    lsm.params.gyro_range = LSM6DS3_ACC_GYRO_FS_G_500dps;
    lsm.params.gyro_sample_rate = LSM6DS3_ACC_GYRO_ODR_XL_1660Hz;
    lsm.params.gyro_bandwidth = LSM6DS3_ACC_GYRO_BW_XL_400Hz;
    lsm.params.gyro_fifo_enabled = true;
    lsm.params.gyro_fifo_decimation = true;

    lsm.params.accel_enabled = true;
    lsm.params.accel_odr_off = true;
    lsm.params.accel_range = LSM6DS3_ACC_GYRO_FS_XL_16g;
    lsm.params.accel_sample_rate = LSM6DS3_ACC_GYRO_ODR_XL_1660Hz;
    lsm.params.accel_bandwidth = LSM6DS3_ACC_GYRO_BW_XL_400Hz;
    lsm.params.accel_fifo_enabled = true;
    lsm.params.accel_fifo_decimation = true;

    lsm.params.comm_mode = 1;

    lsm.params.fifo_threshold = 3000;
    lsm.params.fifo_sample_rate = LSM6DS3_ACC_GYRO_ODR_FIFO_1600Hz;
    lsm.params.fifo_mode_word = 0;

    if (lsm6ds3_init(&lsm) < 0)
    {
        printf("\rError initializing LSM6DS3!\r\n");
    }
    else
    {
        printf("\rLSM6DS3 initialized successfully!\r\n");
    }

    static lsm6ds3_data_t lsm_data;
    static int16_t last_x, last_y, last_z;

    lsm6ds3_poweron(&lsm);
    lsm6ds3_read_acc(&lsm, &lsm_data);
    lsm6ds3_poweroff(&lsm);

    last_x = lsm_data.acc_x;
    last_y = lsm_data.acc_y;
    last_z = lsm_data.acc_z;

    while (1)
    {
        int16_t x, y, z;

        lsm6ds3_poweron(&lsm);
        lsm6ds3_read_acc(&lsm, &lsm_data);
        lsm6ds3_poweroff(&lsm);

        x = lsm_data.acc_x;
        y = lsm_data.acc_y;
        z = lsm_data.acc_z;

        printf("x %i, y %i, z %i\r\n", x, y, z);

        if (abs(x - last_x) >= KICK_THRESHOLD || abs(y - last_y) >= KICK_THRESHOLD || abs(z - last_z) >= KICK_THRESHOLD)
        {
            printf("Ouch!\r\n");
        }

        last_x = x;
        last_y = y;
        last_z = z;

        xtimer_msleep(100);
    }
}