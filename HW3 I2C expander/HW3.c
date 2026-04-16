#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// I2C defines
#define I2C_PORT i2c0
#define I2C_SDA 12
#define I2C_SCL 13

// led counter
unsigned char counter = 0;

void write_i2c(unsigned char address, unsigned char reg, unsigned char data) {
    // writes to register reg w/ data
    unsigned char buf[2] = {reg, data};
    i2c_write_blocking(I2C_PORT, address, buf, 2, false);
}

unsigned char read_i2c(unsigned char address, unsigned char reg) {
    // reads from register reg into data
    i2c_write_blocking(I2C_PORT, address, &reg, 1, true);
    unsigned char buf;
    i2c_read_blocking(I2C_PORT, address, &buf, 1, false);
    return buf;
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

    // mcp23008 values
    unsigned char address = 0b0100000;
    unsigned char iodir_register = 0x00;
    unsigned char gpio_register = 0x09;
    unsigned char olat_register = 0x0A;
    unsigned char iodirections = 0b01111111;

    // set gp7 to output
    write_i2c(address, iodir_register, iodirections);

    while (true) {
        // if gp0 low, set gp7 high. else, set gp7 low
        unsigned char gpio_value = read_i2c(address, gpio_register);
        if ((gpio_value & 0b00000001) == 0) {
            write_i2c(address, olat_register, 0b10000000);
        } else {
            write_i2c(address, olat_register, 0b00000000);
        }
        // onboard LED blinks to show program is running
        gpio_put(PICO_DEFAULT_LED_PIN, counter % 5);
        counter++;
        sleep_ms(100);
    }
}
