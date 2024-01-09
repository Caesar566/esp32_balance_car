#include <stdio.h>
#include <math.h>
#include "I2C_set.h"
#include "mpu6050.h"



void MPu6050_Init(){
    uint8_t data[2];
    uint8_t send_data[2];
    esp32_i2c_read(DEV_ADDR, WHO_AM_I, 2, data);
    printf("WHO_AM_I = %X\n", data[0]);

    send_data[0] = 0x80; 
    esp32_i2c_write(DEV_ADDR, PWR_MGMT_1, 2, send_data);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    send_data[0] = 0x01;
    esp32_i2c_write(DEV_ADDR, PWR_MGMT_1, 2, send_data);

    send_data[0] = 0x07;
    esp32_i2c_write(DEV_ADDR, SMPLRT_DIV, 2, send_data);
    send_data[0] = 0x06;
    esp32_i2c_write(DEV_ADDR, CONFIG, 2, send_data);
    send_data[0] = 0x18;
    esp32_i2c_write(DEV_ADDR, GYRO_CONFIG, 2, send_data);
    send_data[0] = 0x01;
    esp32_i2c_write(DEV_ADDR, ACCEL_CONFIG, 2, send_data);
}


int16_t MPU6050_Get_Data(uint8_t regAddr, unsigned char length)
{
    uint16_t data;
    uint8_t data_L[2];
    uint8_t data_H[2];
    esp32_i2c_read(DEV_ADDR, regAddr, length, data_H);
    esp32_i2c_read(DEV_ADDR, regAddr + 1, length, data_L);
    data = (data_H[0] << 8) | data_L[0];  // 合成数据 
    return data;
}

void MPU6050_Get_Angle(MPU6050_Angle *data)
{   
    /* 计算x, y, z 轴倾角，返回弧度值*/
    data->X_Angle = acos((MPU6050_Get_Data(ACCEL_XOUT_H, 2) + X_ACCEL_OFFSET) / 16384.0);
    data->Y_Angle = acos((MPU6050_Get_Data(ACCEL_YOUT_H, 2) + Y_ACCEL_OFFSET) / 16384.0);
    data->Z_Angle = acos((MPU6050_Get_Data(ACCEL_ZOUT_H, 2) + Z_ACCEL_OFFSET) / 16384.0);

    /* 弧度值转换为角度值 */
    data->X_Angle = data->X_Angle * 57.29577;
    data->Y_Angle = data->Y_Angle * 57.29577;
    data->Z_Angle = data->Z_Angle * 57.29577;
}
