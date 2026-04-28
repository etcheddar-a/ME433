#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <math.h>

#define PIN_CS 17

static inline void cs_select(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 0);
    asm volatile("nop \n nop \n nop"); // FIXME
}

static inline void cs_deselect(uint cs_pin) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs_pin, 1);
    asm volatile("nop \n nop \n nop"); // FIXME
}

void dac_write(int channel, float voltage) {
    uint8_t data[2];
    // first 8 bits
    data[0] = 0b01110000;
    data[0] = data[0] | ((channel & 0b1) << 7); // set the channel bit
    uint16_t v = voltage / 3.3 * 1023; // scale to 0-1023
    data[0] = data[0] | ((v >> 6)&0b00001111); // put into last 4 bits

    // second 8 bits
    data[1] = (v << 2) & 0xFF; // put the remaining 6 bits into the second byte

    cs_select(PIN_CS);
    spi_write_blocking(spi_default, data, 2); // where data is a uint8_t array with length len
    cs_deselect(PIN_CS);
}

int main()
{
    stdio_init_all();

    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1); // deassert the chip select

    // initialize Pico SPI
    spi_init(spi_default, 1000 * 1000); // the baud, or bits per second
    gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);

    // create sine wave buffer
    int f = 2; // 2 Hz
    float sine_buffer[100];
    for (int t = 0; t < 100; t++){
        sine_buffer[t] = (sin(2 * M_PI * f * t / 100)+1)/2*3.3; // scaled to 0-3.3V
    }

    // create 1 Hz triangle wave buffer
    float triangle_buffer[100];
    for (int t = 0; t < 100; t++){
        triangle_buffer[t] = (t < 50) ? (t / 50.0) * 3.3 : ((100 - t) / 50.0) * 3.3; // scaled to 0-3.3V
    }

    int i = 0;
    while (true) {
        dac_write(0, sine_buffer[i]); // sine wave to A
        dac_write(1, triangle_buffer[i]); // triangle wave to B
        i = (i + 1) % 100;
        sleep_ms(10);
    }
}
