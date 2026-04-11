#ifndef WRITE_SSD1306_H
#define WRITE_SSD1306_H

void ssd1306_drawChar(unsigned char x, unsigned char y, unsigned char c);
void ssd1306_drawString(unsigned char x, unsigned char y, char *str);

#endif // WRITE_SSD1306_H