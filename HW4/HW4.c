#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/adc.h"
#include "ssd1306.h"
#include "write_ssd1306.h"

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

    // initialize ssd1306
    ssd1306_setup();
    ssd1306_clear();

    // initialize adc
    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);

    // initialize fps value
    unsigned int last_time = 0;

    // iniitialize message
    char message[25];
    sprintf(message, "ABCDEFGHIJKLMNOPQRSTUVWXY");
    char adc_message[25];
    char fps_message[25];

    while (true) {
        led_heartbeat();
        
        // messages to display
        ssd1306_drawString(0,0,message);
        ssd1306_drawString(0,8,message);
        ssd1306_drawString(0,16,message);

        // read adc and convert to voltage
        uint16_t adc_value = adc_read();
        float voltage = adc_value * 3.3 / (1 << 12); // convert to voltage
        sprintf(adc_message, "ADC: %.2f V", voltage);
        ssd1306_drawString(64,24,adc_message);
    
        // compute fps
        unsigned int t = to_us_since_boot(get_absolute_time());
        unsigned int dt = t - last_time;
        last_time = t;

        float fps = 1e6 / dt;
        sprintf(fps_message, "FPS: %.2f", fps);
        ssd1306_drawString(0,24,fps_message);

        ssd1306_update();
    }
}
