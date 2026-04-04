#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

#define PWM_PIN 16

volatile uint16_t wrap = 60000;

void init_pwm()
{
    gpio_set_function(PWM_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(PWM_PIN);
    float div = 50;
    pwm_set_clkdiv(slice_num, div);
    pwm_set_wrap(slice_num, wrap);
    pwm_set_enabled(slice_num, true);
}

void set_pwm(float angle){
    // angle must be between 0 and 180
    float duty = ((angle / 180.0) * 0.1 + 0.025) * wrap;
    pwm_set_gpio_level(PWM_PIN, duty);
}

int main()
{
    stdio_init_all();

    init_pwm();

    while (true) {
        // sweep servo from 0 to 180 degrees and back to 0 degrees
        float angle = 0;
        while (angle <= 180) {
            set_pwm(angle);
            sleep_ms(10);
            angle += 1;
        }
        while (angle >= 0) {
            set_pwm(angle);
            sleep_ms(10);
            angle -= 1;
        }
    }
}
