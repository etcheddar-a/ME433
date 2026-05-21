#include "hx711.h"

#define DELAY 10
#define DATA_PIN 9
#define CLOCK_PIN 8

void hx711_init() {
    gpio_init(DATA_PIN);
    gpio_set_dir(DATA_PIN, GPIO_IN);
    gpio_pull_up(DATA_PIN);

    gpio_init(CLOCK_PIN);
    gpio_set_dir(CLOCK_PIN, GPIO_OUT);
    gpio_put(CLOCK_PIN, 0);
}

int hx711_read() { // int is a 32-bit output
    // wait for data ready
    while (gpio_get(DATA_PIN) == 1){
        tight_loop_contents();
    }
    
    sleep_us(DELAY);
    // read data by driving clock 24 times
    unsigned int data = 0;
    for (int i=0; i<24; i++){
        gpio_put(CLOCK_PIN, 1);
        sleep_us(DELAY);
        data = (data << 1) | gpio_get(DATA_PIN); // shift data left and add new bit
        gpio_put(CLOCK_PIN, 0);
        sleep_us(DELAY);
    }

    // 25th pulse to set gain
    gpio_put(CLOCK_PIN, 1);
    sleep_us(DELAY);
    gpio_put(CLOCK_PIN, 0);
    sleep_us(DELAY);

    // sign-extend 24-bit two's complement into 32-bit signed int
    if(data & 0x800000) {
        data |= 0xFF000000;
    }
    
    return data;
}