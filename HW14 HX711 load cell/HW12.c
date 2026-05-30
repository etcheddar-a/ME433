#include <stdio.h>
#include "pico/stdlib.h"
#include "hx711.h"

int main()
{
    stdio_init_all();

    hx711_init(); // change

    while (true) {
        int data[1000];
        int raw_data[1000];
        int n_samp = 0;
        unsigned int t[1000];

        // read in desired number of data points
        scanf("%d", &n_samp);

        // introduce initial value
        int avg = hx711_read();

        // run until desired number of data points is reached
        if(n_samp < 1000){
            for (int i=0; i<n_samp; i++){
                int raw = hx711_read();
                // implement IIR
                avg = 0.8*avg + 0.2*raw;
                data[i] = avg;
                raw_data[i] = raw;
                // save timestamp
                t[i] = to_ms_since_boot(get_absolute_time());
                // print to computer
                printf("%d %d %d\n", t[i], data[i], raw_data[i]);
            }
        }
    }
}
