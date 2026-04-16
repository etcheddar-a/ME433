#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"
#include "write_ssd1306.h"

#define I2C_PORT i2c0
#define I2C_SDA 12
#define I2C_SCL 13
#define MPU6050_ADDR 0x68

// config registers
#define CONFIG 0x1A
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C
#define PWR_MGMT_1 0x6B
#define PWR_MGMT_2 0x6C
// sensor data registers:
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define TEMP_OUT_H   0x41
#define TEMP_OUT_L   0x42
#define GYRO_XOUT_H  0x43
#define GYRO_XOUT_L  0x44
#define GYRO_YOUT_H  0x45
#define GYRO_YOUT_L  0x46
#define GYRO_ZOUT_H  0x47
#define GYRO_ZOUT_L  0x48
#define WHO_AM_I     0x75

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

void mpu6050_init(){
    unsigned char who_am_i; // read who_am_i register for 0x68 check
    unsigned char reg = WHO_AM_I;
    i2c_write_blocking(i2c_default, MPU6050_ADDR, &reg, 1, true);
    i2c_read_blocking(i2c_default, MPU6050_ADDR, &who_am_i, 1, false);
    if (who_am_i != 0x68 && who_am_i != 0x98) { // if not 0x68 or 0x98, indicate error by blinking LED
        printf("mpu6050 not found\n");
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

void convert_data(unsigned char *buffer, float *accel, float *gyro, float *temperature){
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

void ssd1306_drawAccel(float *accel){
    // center values
    int x_center = 64;
    int y_center = 16;

    // scale accel values
    int x_end = x_center - (int)(accel[0] * 15); // invert x accel to match orientation of display
    int y_end = y_center + (int)(accel[1] * 15); 

    // draw line from center to (x_end, y_end)
    ssd1306_drawLine(x_center, y_center, x_end, y_end);
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

    // initialize ssd1306
    ssd1306_setup();
    ssd1306_clear();

    // create center dot
    ssd1306_drawPixel(64,16,1);
    ssd1306_update();

    // initialize fps value
    unsigned int last_time = 0;
    char fps_message[25];

    while (true) {
        led_heartbeat();

        // initialize data arrays
        unsigned char data[14];
        float accel[3];
        float gyro[3];
        float temperature;

        // read data and store in accel, gyro, and temperature vars
        mpu6050_read(data);
        convert_data(data, accel, gyro, &temperature);

        // compute fps
        unsigned int t = to_us_since_boot(get_absolute_time());
        unsigned int dt = t - last_time;
        last_time = t;
        float fps = 1e6 / dt;
        sprintf(fps_message, "FPS: %.2f", fps);

        // draw accel data on OLED
        ssd1306_clear();
        ssd1306_drawString(0,24,fps_message);
        ssd1306_drawAccel(accel);
        ssd1306_update();
    }
}
