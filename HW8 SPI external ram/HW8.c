#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <math.h>

#define PIN_CS_RAM 20
#define PIN_CS_DAC 17

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

// function prototypes
void update_dac_from_ram(int);
void test_run();

void spi_ram_init();
void spi_ram_write(uint16_t, uint8_t *, int);
void spi_ram_read(uint16_t, uint8_t *, int);

void ram_write_sine();

int main()
{
    stdio_init_all();

    // while(!stdio_usb_connected()){
    //     tight_loop_contents();
    // }
    // sleep_ms(1000);
    // printf("USB connected, starting...\n");

    // initialize Pico SPI
    spi_init(spi_default, 1000*1000); // the baud, or bits per second
    gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);

    // initialize chip select pins
    gpio_init(PIN_CS_RAM);
    gpio_set_dir(PIN_CS_RAM, GPIO_OUT);
    gpio_put(PIN_CS_RAM, 1); // deassert the chip select
    gpio_init(PIN_CS_DAC);
    gpio_set_dir(PIN_CS_DAC, GPIO_OUT);
    gpio_put(PIN_CS_DAC, 1); // deassert the chip select

    // initailize SPI RAM
    spi_ram_init();

    // fill RAM with bitshifted sine wave
    ram_write_sine();

    printf("Sine wave written to RAM.\n");

    int i = 0;
    while (true) {
        // update_dac_from_ram(i*2);
        // i = (i+1) % 1024;
        // sleep_ms(1);
        for (i=0; i<1024*2; i=i+2){
            // update the DAC from the RAM
            update_dac_from_ram(i);

            //test_run(i);

            sleep_ms(1);
        }
    }
}

void test_run(int i) {
    uint16_t data_short = 0;
    uint8_t data[2];

    uint16_t voltage = (uint16_t)((sin(2 * M_PI * i / 1024.0)+1)*511.5);

    data_short = (0b0 << 15);       // channel A
    data_short |= (0b111 << 12);    // control bits
    data_short |= ((voltage & 0x3FF) << 2);

    data[0] = (data_short >> 8) & 0xFF;
    data[1] = data_short & 0xFF;

    cs_select(PIN_CS_DAC);
    spi_write_blocking(spi_default, data, 2);
    cs_deselect(PIN_CS_DAC);
}

void spi_ram_init() {
    uint8_t data[2];
    int len = 2;
    data[0] = 0b00000001; // write mode register, could also be 0b00000101
    data[1] = 0b01000000; // sequential mode
    cs_select(PIN_CS_RAM);
    spi_write_blocking(spi_default, data, len);
    cs_deselect(PIN_CS_RAM);
}

void ram_write_sine() {
    int i = 0;
    uint8_t data[2];
    uint16_t data_short = 0;
    uint8_t channel = 0b0;  // select channel A
    uint16_t voltage = 0;
    uint16_t address = 0;

    for (i = 0; i<1024; i++){
        data_short = (channel&0b1) << 15; // place channel bit
        data_short = data_short | (0b0111<<12); // place static bits
        
        voltage = (uint16_t)((sinf(2.0f * M_PI * i / 1024.0)+1)*511.5); // scaled to 0-1023

        // place last 10 voltage bits into bits 2-11 of data_short
        data_short = data_short | ((voltage & 0x3FF) << 2);
        
        // split into 8-bit values
        data[0] = (data_short >> 8) & 0xFF;
        data[1] = data_short & 0xFF;

        // write to SPI RAM
        spi_ram_write(address, data, 2);

        // check read/write same
        spi_ram_read(address, data, 2);
        if (data[0] != data[0] || data[1] != data[1]) {
            printf("Error: Read and write data don't match!\n");
        }

        address = address + 2;
    }

}

void spi_ram_write(uint16_t address, uint8_t *data, int len) {
    uint8_t packet[5];
    packet[0] = 0b00000010; // write command
    packet[1] = address>>8; // 16-bit address stored in packet[1] and packet[2]
    packet[2] = address & 0xFF;
    packet[3] = data[0];
    packet[4] = data[1];

    cs_select(PIN_CS_RAM);
    spi_write_blocking(spi_default, packet, 5);
    cs_deselect(PIN_CS_RAM);
}

void update_dac_from_ram(int i) {
    uint8_t data[2];
    spi_ram_read(i, data, 2);

    cs_select(PIN_CS_DAC);
    spi_write_blocking(spi_default, data, 2);
    cs_deselect(PIN_CS_DAC);
}

void spi_ram_read(uint16_t address, uint8_t *data, int len) {
    uint8_t packet[5];
    packet[0] = 0b00000011; // read command
    packet[1] = address>>8; // 16-bit address stored in packet[1] and packet[2]
    packet[2] = address & 0xFF;
    packet[3] = 0;
    packet[4] = 0;

    uint8_t datastore[5];
    cs_select(PIN_CS_RAM);
    spi_write_read_blocking(spi_default, packet, datastore, 5);
    cs_deselect(PIN_CS_RAM);
    data[0] = datastore[3];
    data[1] = datastore[4];
}