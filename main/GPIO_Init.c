#include "driver/gpio.h"
#include "gpio_define.h"

void GPIO_Init(){
    gpio_config_t Gpio_Config_output = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ull << OUTPUT_AIN1) | 
                        (1ull << OUTPUT_AIN2) |
                        (1ull << OUTPUT_BIN1) |
                        (1ull << OUTPUT_BIN2),
    };
    gpio_config(&Gpio_Config_output);

    // gpio_config_t Gpio_Config_input = {
    //     .mode = GPIO_MODE_OUTPUT,
    //     .pin_bit_mask = (1ull << INPUT_HALL_LA) | 
    //                     (1ull << INPUT_HALL_LB) |
    //                     (1ull << INPUT_HALL_RA) |
    //                     (1ull << INPUT_HALL_RA),
    // };
    // gpio_config(&Gpio_Config_input);
    
}