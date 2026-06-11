#include "as5600.h"

volatile uint16_t zero_offset;

void as5600_init() {
    char buf[2];
    uint8_t reg = RAW_ANGLE_REG;
    i2c_write_blocking(I2C_PORT, AS5600_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, AS5600_ADDR, buf, 2, false);
    zero_offset = (buf[0] << 8) | buf[1];
}

int as5600_read() {
    char buf[2];
    uint8_t reg = RAW_ANGLE_REG;
    i2c_write_blocking(I2C_PORT, AS5600_ADDR, &reg, 1, true);
    i2c_read_blocking(I2C_PORT, AS5600_ADDR, buf, 2, false);
    uint16_t raw_angle = (buf[0] << 8) | buf[1];
    int diff = (raw_angle - zero_offset) % 4096; // angle in range [0, 4095]
    if (diff > 2047) { // if the difference is greater than half the range, we assume it wrapped around
        diff -= 4096; // adjust for wrap-around
    }
    return diff;
}