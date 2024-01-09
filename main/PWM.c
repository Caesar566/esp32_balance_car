#include "driver/ledc.h"
#include "gpio_define.h"
#include "PWM.h"

void PWM_Init(){
    ledc_channel_config_t ledc_channel_l = { 
	.channel =	LEDC_CHANNEL_0,	// LED控制器通道号, 
	.duty = 0, 
	.gpio_num = OUTPUT_PWM_L,				// LED控制器通道对应GPIO, 
	.speed_mode = LEDC_HIGH_SPEED_MODE, // 模式, 
	.timer_sel = LEDC_TIMER_0,   		// 使用哪个定时器0-3
    };

    ledc_channel_config_t ledc_channel_r = { 
	.channel =	LEDC_CHANNEL_1,	// LED控制器通道号, 
	.duty = 0, 
	.gpio_num = OUTPUT_PWM_R,				// LED控制器通道对应GPIO, 
	.speed_mode = LEDC_HIGH_SPEED_MODE, // 模式, 
	.timer_sel = LEDC_TIMER_0,   		// 使用哪个定时器0-3
    };

    // 配置LED控制器
    ledc_channel_config(&ledc_channel_l);
    ledc_channel_config(&ledc_channel_r);
}