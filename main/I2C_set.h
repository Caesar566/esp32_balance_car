#ifndef _I2C_SET_H_
#define _I2C_SET_H_

#include "driver/i2c.h"
#include "driver/i2c_types.h"

void I2c_Init(unsigned char sda_num, unsigned char scl_num);
int esp32_i2c_write(unsigned char slave_addr, unsigned char reg_addr, unsigned char length, unsigned char const *data);
int esp32_i2c_read(unsigned char slave_addr, unsigned char reg_addr, unsigned char length, unsigned char *data);

#endif

