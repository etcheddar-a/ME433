#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"

// UART defines
// By default the stdout UART is `uart0`, so we will use the second one
#define UART_ID uart1
#define BAUD_RATE 115200

// Use pins 4 and 5 for UART1
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define UART_TX_PIN 4
#define UART_RX_PIN 5



int main()
{
    stdio_init_all();

    while(!stdio_usb_connected()) {
        tight_loop_contents();
    }

    sleep_ms(250);
    printf("USB Connected\n");

    // Set up our UART
    uart_init(UART_ID, BAUD_RATE);
    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    
    
    while (true) {
        // PC → UART
        int c = getchar_timeout_us(0);
        if (c != PICO_ERROR_TIMEOUT) {
            uart_putc(UART_ID, (char)c);
        }

        // UART → PC
        if (uart_is_readable(UART_ID)) {
            char c2 = uart_getc(UART_ID);
            printf("%c", c2);
        }

        // BLOCKING CODE
        // char buf[100];
        // fgets(buf, sizeof(buf), stdin); // read the rest of the line (if any) to clear the input buffer
        // uart_puts(UART_ID, buf);
    }
}
