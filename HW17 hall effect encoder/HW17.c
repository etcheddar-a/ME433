#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "as5600.h"

#define I2C_PORT i2c1
#define I2C_SDA 2
#define I2C_SCL 3

int main()
{
    stdio_init_all();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    as5600_init();

    while (true) {
        int angle = as5600_read();
        float degrees = (angle / 4096.0) * 360.0; // Convert to degrees
        printf("Degrees: %.2f\r\n", degrees);
        sleep_ms(200);
    }
}
