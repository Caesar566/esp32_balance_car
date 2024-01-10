#include "driver/ledc.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "driver/i2c_types.h"
#include "PWM.h"
#include "gpio_define.h"
#include "I2C_set.h"
#include "GPIO_Init.h"
#include "mpu6050.h"
#include "OLEDDisplay.h"
#include "Hall.h"
#include "driver/pulse_cnt.h"


void Init(){
    PWM_Init();
    GPIO_Init();
    I2c_Init(OUTPUT_MPU_SDA, OUTPUT_MPU_SCL, I2C_NUM_0);
    I2c_Init(OUTPUT_OLED_SDA, OUTPUT_OLED_SCL, I2C_NUM_1);
    MPU6050_Init();
}
/*
moto 控制
mpu读取并显示
*/



// void app_main(void)
// {
//     Init();
//     OLEDDisplay_t *oled = OLEDDisplay_init(I2C_NUM_1, 0x78, OUTPUT_OLED_SDA, OUTPUT_OLED_SCL);
//     MPU6050_Angle data;  
//     char display_data_x_angle[10];
//     char display_data_y_angle[10];
//     char display_data_z_angle[10];
//     vTaskDelay(500 / portTICK_PERIOD_MS);  

//     while (1){
//         MPU6050_Get_Angle(&data);
//         // printf("temp is %f\t", (MPU6050_Get_Data(TEMP_OUT_H, 2)/ 340.0 + 36.53));
//         // printf("ACCEL_X: %lf\t", MPU6050_Get_Data(ACCEL_XOUT_H,2) / 16384.0);
//         // printf("ACCEL_Y: %lf\t", MPU6050_Get_Data(ACCEL_YOUT_H,2) / 16384.0);
//         // printf("ACCEL_Z: %lf\t", MPU6050_Get_Data(ACCEL_ZOUT_H,2) / 16384.0);
//         // printf("X_Angle = %lf° ", data.X_Angle);
//         // printf("Y_Angle = %lf° ", data.Y_Angle);
//         // printf("Z_Angle = %lf° ", data.Z_Angle);
//         // printf("\n");
//         sprintf(display_data_x_angle, "%f", data.X_Angle);
//         sprintf(display_data_y_angle, "%f", data.Y_Angle);
//         sprintf(display_data_z_angle, "%f", data.Z_Angle);
//         display_data_x_angle[5] = '\0';
//         display_data_y_angle[5] = '\0';
//         display_data_z_angle[5] = '\0';
//         // OLEDDisplay_drawString(oled, 7, 7, display_data);
        
//         // OLEDDisplay_display(oled);
//         	// OLEDDisplay_setColor(oled,INVERSE);
//             // OLEDDisplay_fillRect(oled,6,6,20,20);
// 			// OLEDDisplay_fillRect(oled,6,32,20,20);
//             OLEDDisplay_clear(oled);
//             OLEDDisplay_drawString(oled, 5, 5, display_data_x_angle);
//             OLEDDisplay_drawString(oled, 5, 25, display_data_y_angle);
//             OLEDDisplay_drawString(oled, 5, 45, display_data_z_angle);
// 			OLEDDisplay_display(oled);
// 			vTaskDelay(100 / portTICK_PERIOD_MS);
//     }

//     //run moto
// //     ledc_fade_func_install(0);
// //     ledc_set_duty_and_update(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 500, 500);
// //     ledc_set_duty_and_update(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 500, 500);
// //     gpio_set_level(OUTPUT_AIN1, 1);
// //     gpio_set_level(OUTPUT_AIN2, 0);
// //     gpio_set_level(OUTPUT_BIN1, 1);
// //     gpio_set_level(OUTPUT_BIN2, 0);
// }

void app_main(void){
    Init();
    //     //run moto
    // ledc_fade_func_install(0);
    // ledc_set_duty_and_update(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 500, 500);
    // ledc_set_duty_and_update(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 500, 500);
    pcnt_unit_handle_t PCNT_Handle;
    PCNT_Handle = counter_Init();
    OLEDDisplay_t *oled = OLEDDisplay_init(I2C_NUM_1, 0x78, OUTPUT_OLED_SDA, OUTPUT_OLED_SCL);
    int pulse_count = 0;
    char counter_read[10];
    while (1)
    {
        pcnt_unit_get_count(PCNT_Handle, &pulse_count);
        // printf("num is %d\n", pulse_count);
        sprintf(counter_read, "%d", pulse_count);
        OLEDDisplay_clear(oled);
        OLEDDisplay_drawString(oled, 5, 5, counter_read);
        OLEDDisplay_display(oled);
		// vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    
}