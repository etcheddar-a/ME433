#include "write_ssd1306.h"
#include "ssd1306.h"
#include "font.h"
#include <stdlib.h>

void ssd1306_drawChar(unsigned char x, unsigned char y, unsigned char c) {
    if (c < 0x20 || c > 0x7F) {
        return; // ignore non-printable characters
    }
    for (int i = x; i < x + 5; i++) {
        for (int j = y; j < y + 8; j++) {
            unsigned char state = (ASCII[c - 0x20][i - x] >> (j - y)) & 0x01; // select character, select column w/ x, select bit w/ y
            ssd1306_drawPixel(i, j, state);
        }
    }
}

void ssd1306_drawString(unsigned char x, unsigned char y, char *str) {
    unsigned int index = 0;
    while (str[index] != '\0') {
        ssd1306_drawChar(x + index*5, y, str[index]);
        index++;
    }
}

void ssd1306_drawLine(unsigned char x_start, unsigned char y_start, unsigned char x_end, unsigned char y_end) {
    int dx = x_end - x_start;
    int dy = y_end - y_start;
    
     // determine number of steps as max of dx or dy
    int steps = (abs(dx) > abs(dy)) ? abs(dx) : abs(dy);

    // find increment in x and y for each step
    float x_inc = dx / (float) steps;
    float y_inc = dy / (float) steps;

    // draw line from start to finish
    // each time, x and y increased and rounded to nearest pixel
    for(int i = 0; i <= steps; i++) {
        ssd1306_drawPixel(x_start + (int)(i * x_inc), y_start + (int)(i * y_inc), 1);
    }
}