#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "mpu6050.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 12
#define I2C_SCL 13

void led_heartbeat(){
    static uint64_t last_toggle = 0;
    static bool led_on = false;
    uint64_t now = time_us_64();
    if (now - last_toggle >= 500000) {
        last_toggle = now;
        led_on = !led_on;
        gpio_put(PICO_DEFAULT_LED_PIN, led_on);
    }
}

int main()
{
    stdio_init_all();

    // initialize onboard LED
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // initialize MPU6050
    mpu6050_init();

    // initialize data arrays
        unsigned char data[14];
        float accel[3];
        float gyro[3];
        float temperature;

    while (true) {
        led_heartbeat();

        // read data and store in accel, gyro, and temperature vars
        mpu6050_read(data);
        mpu6050_convert_data(data, accel, gyro, &temperature);

        // print data
        printf("%.3f, %.3f\n", accel[0], accel[1]); // print accel x and y

        sleep_ms(50);
    }
}
