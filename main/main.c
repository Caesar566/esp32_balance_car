#include "driver/ledc.h"
#include "driver/gpio.h"
#include "PWM.h"
#include "gpio_define.h"
#include "I2C_set.h"

void Timer_Init(){
    ledc_timer_config_t Ledc_Timer = {
        .duty_resolution = LEDC_TIMER_10_BIT,
        .freq_hz = 10000,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_0
    };
    ledc_timer_config(&Ledc_Timer);
}

void app_main(void)
{
    Timer_Init();
    PWM_Init();
    // GPIO_Init();
    ledc_fade_func_install(0);
    ledc_set_duty_and_update(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 500, 500);
    ledc_set_duty_and_update(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 500, 500);

}

