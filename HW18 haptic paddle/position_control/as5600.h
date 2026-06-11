#ifndef AS5600_H
#define AS5600_H

#include "hardware/i2c.h"

#define I2C_PORT i2c1
#define AS5600_ADDR 0x36
#define RAW_ANGLE_REG 0x0C



void as5600_init();
int as5600_read();

#endif // AS5600_H