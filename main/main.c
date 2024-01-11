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

int balance_UP(float Angle,float Mechanical_balance,float Gyro)
{  
    float Bias;//角度误差
    int balance;//直立环计算出来的电机控制pwm

    float balance_UP_KP = 24;//24
    float balance_UP_KD = -6.8;//-6.8
    Bias=Angle - Mechanical_balance;                   
    //===求出平衡的角度中值和机械相关

    balance=balance_UP_KP*Bias+balance_UP_KD*Gyro;  
    //===计算平衡控制的电机PWM  PD控制   kp是P系数 kd是D系数
    // printf("pwm is %d\n", balance); 
    return balance;
}


int velocity(int encoder_left,int encoder_right, float Angle)
{  
    static float Velocity,Encoder_Least,Encoder,Movement;
    static float Encoder_Integral;
   //=============速度PI控制器=======================//  
    Encoder_Least =(encoder_left + encoder_right)-0;
    Encoder = 0;
    //===获取最新速度偏差==测量速度（左右编码器之和）-目标速度（此处为零） 
    // Encoder *= 0.7;          //===一阶低通滤波器       
    Encoder += Encoder_Least*0.3;   //===一阶低通滤波器    
    Encoder_Integral +=Encoder; //===积分出位移 积分时间：10ms

    if(Encoder_Integral>10000)    Encoder_Integral=10000;   
    //===积分限幅
    if(Encoder_Integral<-10000)    Encoder_Integral=-10000;   
    //===积分限幅  
    Velocity=Encoder*5+Encoder_Integral*0.025;  
    //===速度控制
    if(Angle<-40 + 100||Angle>40 + 100)   Encoder_Integral=0;   
    //===电机关闭后清除积分
    printf("pwm is %lf\n", Velocity);
    return Velocity;
}

void app_main(void)
{
    Init();
    OLEDDisplay_t *oled = OLEDDisplay_init(I2C_NUM_1, 0x78, OUTPUT_OLED_SDA, OUTPUT_OLED_SCL);
    MPU6050_Angle data;  
    char display_data_x_angle[10];
    char display_data_y_angle[10];
    char display_data_z_angle[10];
    char display_data_x_gyro[10];
    char display_data_y_gyro[10];
    char display_data_z_gyro[10];
    ledc_fade_func_install(0);
    
    pcnt_unit_handle_t PCNT_Handle;
    PCNT_Handle = counter_Init();
    int pulse_count = 0;
    int data_1 = 0;
    int data_2 = 0;
    int data_end =0;
    char counter_read[10];
    int pwm_balance = 0;
    int Velocity = 0;


    vTaskDelay(500 / portTICK_PERIOD_MS);


    while (1){
        MPU6050_Get_Angle(&data);
        // printf("ACCEL_X: %lf\t", MPU6050_Get_Data(ACCEL_XOUT_H,2) / 16384.0);
        // printf("ACCEL_Y: %lf\t", MPU6050_Get_Data(ACCEL_YOUT_H,2) / 16384.0);
        // printf("ACCEL_Z: %lf\t", MPU6050_Get_Data(ACCEL_ZOUT_H,2) / 16384.0);
        // sprintf(display_data_x_angle, "%f", data.X_Angle);
        // sprintf(display_data_y_angle, "%f", data.Y_Angle);
        // sprintf(display_data_z_angle, "%f", data.Z_Angle);
        // display_data_x_angle[5] = '\0';
        // display_data_y_angle[5] = '\0';
        // display_data_z_angle[5] = '\0';

        // sprintf(display_data_x_gyro, "%f", data.X_GYRO);
        // sprintf(display_data_y_gyro, "%f", data.Y_GYRO);
        // sprintf(display_data_z_gyro, "%f", data.Z_GYRO);
        // display_data_x_gyro[5] = '\0';
        // display_data_y_gyro[5] = '\0';
        // display_data_z_gyro[5] = '\0';

        // OLEDDisplay_clear(oled);
        // OLEDDisplay_drawString(oled, 5, 5, display_data_x_angle);
        // OLEDDisplay_drawString(oled, 5, 25, display_data_y_angle);
        // OLEDDisplay_drawString(oled, 5, 45, display_data_z_angle);
        // OLEDDisplay_drawString(oled, 50, 5, display_data_x_gyro);
        // OLEDDisplay_drawString(oled, 50, 25, display_data_y_gyro);
        // OLEDDisplay_drawString(oled, 50, 45, display_data_z_gyro);
        // OLEDDisplay_display(oled);


        pcnt_unit_clear_count(PCNT_Handle);
        pcnt_unit_get_count(PCNT_Handle, &pulse_count);
        data_1 = pulse_count;
        vTaskDelay(10 / portTICK_PERIOD_MS);
        pcnt_unit_get_count(PCNT_Handle, &pulse_count);
        data_2 = pulse_count - data_1;
        pcnt_unit_clear_count(PCNT_Handle);



        

        pwm_balance = balance_UP(data.X_Angle, 100.0, data.Y_GYRO);
        Velocity = velocity(data_2, data_2, data.X_Angle);
        pwm_balance = Velocity + pwm_balance;
        // pwm_balance = Velocity;
        sprintf(counter_read, "%d", Velocity);
        OLEDDisplay_clear(oled);
        OLEDDisplay_drawString(oled, 5, 5, counter_read);
        OLEDDisplay_display(oled);

        if (pwm_balance < 2000 && pwm_balance > -2000){
            if(pwm_balance >= 0){
                gpio_set_level(OUTPUT_AIN1, 0);
                gpio_set_level(OUTPUT_AIN2, 1);
                gpio_set_level(OUTPUT_BIN1, 1);
                gpio_set_level(OUTPUT_BIN2, 0);
            }
            else if(pwm_balance < 0){
                gpio_set_level(OUTPUT_AIN1, 1);
                gpio_set_level(OUTPUT_AIN2, 0);
                gpio_set_level(OUTPUT_BIN1, 0);
                gpio_set_level(OUTPUT_BIN2, 1);
                pwm_balance = -pwm_balance;            
            }
            ledc_set_duty_and_update(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, pwm_balance, pwm_balance);
            ledc_set_duty_and_update(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, pwm_balance, pwm_balance);
        }
        else{
            ledc_set_duty_and_update(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0, 0);
            ledc_set_duty_and_update(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 0, 0);
        }

    }
}

// void app_main(void){
//     Init();
//     //     //run moto
//     // ledc_fade_func_install(0);
//     // ledc_set_duty_and_update(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 500, 500);
//     // ledc_set_duty_and_update(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 500, 500);
//     pcnt_unit_handle_t PCNT_Handle;
//     PCNT_Handle = counter_Init();
//     OLEDDisplay_t *oled = OLEDDisplay_init(I2C_NUM_1, 0x78, OUTPUT_OLED_SDA, OUTPUT_OLED_SCL);
//     int pulse_count = 0;
//     char counter_read[10];
//     while (1)
//     {
//         pcnt_unit_get_count(PCNT_Handle, &pulse_count);
//         // printf("num is %d\n", pulse_count);
//         sprintf(counter_read, "%d", pulse_count);
//         OLEDDisplay_clear(oled);
//         OLEDDisplay_drawString(oled, 5, 5, counter_read);
//         OLEDDisplay_display(oled);
// 		// vTaskDelay(10 / portTICK_PERIOD_MS);
//     }
    
// }