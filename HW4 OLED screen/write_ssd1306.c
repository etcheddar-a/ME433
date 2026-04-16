#include "write_ssd1306.h"
#include "ssd1306.h"
#include "font.h"

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