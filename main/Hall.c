#include "driver/pulse_cnt.h"
#include "gpio_define.h"
#include "driver/pcnt_types_legacy.h"
#include "driver/gpio.h"


pcnt_unit_handle_t counter_Init(){
    pcnt_unit_config_t PCNT_Config = {
        .intr_priority = 0,
        .low_limit = -390,
        .high_limit = 390,
    };
    pcnt_unit_handle_t PCNT_Handle;

    pcnt_new_unit(&PCNT_Config, &PCNT_Handle);

    pcnt_chan_config_t CHAN_Config_A = {
        .edge_gpio_num = INPUT_HALL_LA,
        .level_gpio_num = INPUT_HALL_LB,
    };
    pcnt_channel_handle_t CHAN_Handle;
    pcnt_new_channel(PCNT_Handle, &CHAN_Config_A, &CHAN_Handle);

    // decrease the counter on rising edge, increase the counter on falling edge

    pcnt_channel_set_edge_action(CHAN_Handle, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD);
    // keep the counting mode when the control signal is high level, and reverse the counting mode when the control signal is low level
    pcnt_channel_set_level_action(CHAN_Handle, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE);

    // pcnt_event_callbacks_t event_Callback = {
    //     .on_reach = 0,
    // };
    
    // pcnt_watch_event_data_t Watch_Data = {
    //     .watch_point_value = 0,
    //     .zero_cross_mode = PCNT_UNIT_ZERO_CROSS_NEG_POS,
    // };

    // static bool example_pcnt_on_reach(pcnt_unit_handle_t unit, const pcnt_watch_event_data_t *edata, void *user_ctx)
    // {
    //     BaseType_t high_task_wakeup;
    //     QueueHandle_t queue = (QueueHandle_t)user_ctx;
    //     // send watch point to queue, from this interrupt callback
    //     xQueueSendFromISR(queue, &(edata->watch_point_value), &high_task_wakeup);
    //     // return whether a high priority task has been waken up by this function
    //     return (high_task_wakeup == pdTRUE);
    // }

    // pcnt_event_callbacks_t cbs = {
    //     .on_reach = example_pcnt_on_reach,
    // };

    // QueueHandle_t queue = xQueueCreate(10, sizeof(int));
    // ESP_ERROR_CHECK(pcnt_unit_register_event_callbacks(pcnt_unit, &cbs, queue));


    // void *user_data;
    // pcnt_unit_register_event_callbacks(PCNT_Handle, &event_Callback, user_data);

    pcnt_glitch_filter_config_t filter_config = {
    .max_glitch_ns = 1000,
    };
    pcnt_unit_set_glitch_filter(PCNT_Handle, &filter_config);


    pcnt_unit_enable(PCNT_Handle);
    pcnt_unit_start(PCNT_Handle);
    return PCNT_Handle;

}

