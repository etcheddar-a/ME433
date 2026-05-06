#include "mpu6050.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <stdio.h>

void mpu6050_init(){
    unsigned char who_am_i; // read who_am_i register for 0x68 check
    unsigned char reg = WHO_AM_I;
    i2c_write_blocking(i2c_default, MPU6050_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c_default, MPU6050_ADDR, &who_am_i, 1, false);
    if (who_am_i != 0x68 && who_am_i != 0x98) { // if not 0x68 or 0x98, indicate error by blinking LED
        //printf("mpu6050 not found\n");
        while(true){
            gpio_put(PICO_DEFAULT_LED_PIN, 1);
            sleep_ms(50);
            gpio_put(PICO_DEFAULT_LED_PIN, 0);
            sleep_ms(50);
        }
    }

    // turn chip on
    unsigned char pwr_mgmt_1_data[2] = {PWR_MGMT_1, 0x00};
    i2c_write_blocking(i2c_default, MPU6050_ADDR, pwr_mgmt_1_data, 2, false);

    // enable accelerometer, set sensitivity to ±2g
    unsigned char accel_config_data[2] = {ACCEL_CONFIG, 0x00};
    i2c_write_blocking(i2c_default, MPU6050_ADDR, accel_config_data, 2, false);

    // enable gyro, set sensitivity to 2000 dps
    unsigned char gyro_config_data[2] = {GYRO_CONFIG, 0x18};
    i2c_write_blocking(i2c_default, MPU6050_ADDR, gyro_config_data, 2, false);

    return;
}

void mpu6050_read(unsigned char *buffer){ // function to read 14 sequential bytes from 0x3B to 0x48
    unsigned char reg = ACCEL_XOUT_H;
    i2c_write_blocking(i2c_default, MPU6050_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c_default, MPU6050_ADDR, buffer, 14, false);
    return;
}

void mpu6050_convert_data(unsigned char *buffer, float *accel, float *gyro, float *temperature){
    signed short accel_data[3];
    signed short gyro_data[3];
    // convert 14 bytes of data into accel, gyro, and temp values
    accel_data[0] = (buffer[0] << 8) | buffer[1]; // accel x
    accel_data[1] = (buffer[2] << 8) | buffer[3]; // accel y
    accel_data[2] = (buffer[4] << 8) | buffer[5]; // accel z
    *temperature = (signed short)((buffer[6] << 8) | buffer[7]) / 340.0 + 36.53; // temp (°C)
    gyro_data[0] = (buffer[8] << 8) | buffer[9];   // gyro x
    gyro_data[1] = (buffer[10] << 8) | buffer[11]; // gyro y
    gyro_data[2] = (buffer[12] << 8) | buffer[13]; // gyro z

    // multiply accel by 0.000061 --> convert to g
    accel[0] = accel_data[0] * 0.000061;
    accel[1] = accel_data[1] * 0.000061;
    accel[2] = accel_data[2] * 0.000061;

    // multiply gyro by 0.007630 --> convert to dps
    gyro[0] = gyro_data[0] * 0.007630;
    gyro[1] = gyro_data[1] * 0.007630;
    gyro[2] = gyro_data[2] * 0.007630;
    return;
}