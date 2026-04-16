#ifndef WRITE_SSD1306_H
#define WRITE_SSD1306_H

void ssd1306_drawChar(unsigned char x, unsigned char y, unsigned char c);
void ssd1306_drawString(unsigned char x, unsigned char y, char *str);
void ssd1306_drawLine(unsigned char x_start, unsigned char y_start, unsigned char x_end, unsigned char y_end);

#endif // WRITE_SSD1306_H