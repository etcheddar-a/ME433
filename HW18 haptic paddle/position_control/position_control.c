#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "as5600.h"
#include "hx711.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c1
#define I2C_SDA 2
#define I2C_SCL 3

// UART defines
// By default the stdout UART is `uart0`, so we will use the second one
#define UART_ID uart0
#define BAUD_RATE 115200

// Use pins 4 and 5 for UART1
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define UART_TX_PIN 16
#define UART_RX_PIN 17

int main()
{
    stdio_init_all();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Set up our UART
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // custom inits
    as5600_init();
    hx711_init();

    // introduce initial value for hx711
    int sum = 0;
    for (int i=0; i<400; i++){
        sum += hx711_read();
    }
    int f_avg = sum / 400;
    int f_zero = f_avg;

    while (true) {
        // get force data
        int force = hx711_read();
        f_avg = 0.9*f_avg + 0.1*force; // apply IIR
        int f_diff = f_avg - f_zero;

        // get angle data
        int16_t angle = as5600_read();
        float degrees = (angle / 4096.0) * 360.0; // Convert to degrees

        // set desired current based on angle
        const float K_wall = 20.0f;   // current units per degree
        const int16_t I_max = 40;    // max wall strength

        int16_t desired_current = 0;

        if (degrees > 45.0f) {
            float penetration = degrees - 45.0f;

            // positive current pushes back toward center
            desired_current = (int16_t)(K_wall * penetration);

        } else if (degrees < -45.0f) {
            float penetration = -45.0f - degrees;

            // negative current pushes back toward center
            desired_current = -(int16_t)(K_wall * penetration);

        } else {
            // weightless region
            desired_current = 0;
        }

        // saturate
        if (desired_current > I_max)
            desired_current = I_max;
        else if (desired_current < -I_max)
            desired_current = -I_max;
        
        // send data over UART
        uart_write_blocking(UART_ID, (uint8_t *)&desired_current, 2);

        printf("Angle: %.2f degrees, Force: %d\n", degrees, f_diff);
    }
}
