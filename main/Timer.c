// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "freertos/queue.h"
// #include "driver/ledc.h"
// #include "driver/pcnt.h"
// #include "esp_attr.h"
// #include "esp_log.h"
// #include "sdkconfig.h"
// #include "driver/gpio.h"
// #include "esp_timer.h"

// static const char *TAG = "example";

// #define PCNT_H_LIM_VAL 11
// #define PCNT_L_LIM_VAL -40
// #define PCNT_THRESH1_VAL 10
// #define PCNT_THRESH0_VAL -10
// #define PCNT_INPUT_SIG_IO 4  // 脉冲输入GPIO
// #define PCNT_INPUT_CTRL_IO 5 // 控制GPIO HIGH=计数向上，LOW=计数向下
// #define LED_R 2

// xQueueHandle pcnt_evt_queue; // 处理脉冲计数器事件的队列

// //从PCNT传递事件的示例结构,主程序的中断处理程序。
// typedef struct
// {
//     int unit;        //触发中断的PCNT装置
//     uint32_t status; // 关于导致中断的事件类型的信息
// } pcnt_evt_t;

// //解码PCNT的部门发出的中断信号并将此消息与时间类型一起传递给队列的主程序
// static void IRAM_ATTR pcnt_example_intr_handler(void *arg)
// {
//     int pcnt_unit = (int)arg;
//     pcnt_evt_t evt;
//     evt.unit = pcnt_unit;
//     //保存导致中断的PCNT事件类型将它传递给主程序
//     pcnt_get_event_status(pcnt_unit, &evt.status);
//     xQueueSendFromISR(pcnt_evt_queue, &evt, NULL);
// }

// void LED_Init(void)
// {
//     gpio_pad_select_gpio(LED_R);                 // 选择GPIO口
//     gpio_set_direction(LED_R, GPIO_MODE_OUTPUT); // GPIO作为输出
//     gpio_set_level(LED_R, 0);                    // 默认低电平
// }

// /* 初始化PCNT函数:
//  *  配置和初始化PCNT
//  *  设置输入过滤器
//  *  设置计数器时间观看
//  */
// static void pcnt_example_init(int unit)
// {
//     //准备PCNT单元
//     pcnt_config_t pcnt_config = {
//         //设定pct输入信号，控制gpio
//         .pulse_gpio_num = PCNT_INPUT_SIG_IO,
//         .ctrl_gpio_num = PCNT_INPUT_CTRL_IO,
//         .channel = PCNT_CHANNEL_0,
//         .unit = unit,
//         //在脉冲输入的正/负边缘做什么?
//         .pos_mode = PCNT_COUNT_INC, // 计算正面的边缘
//         .neg_mode = PCNT_COUNT_DIS, // 保持计数器值在负边
//         //控制输入低或高时该怎么办?
//         .lctrl_mode = PCNT_MODE_REVERSE, // 如果计数方向低，则反向计数
//         .hctrl_mode = PCNT_MODE_KEEP,    // 如果高，保持主计数器模式
//         // 最大最小值的限制
//         .counter_h_lim = PCNT_H_LIM_VAL,
//         .counter_l_lim = PCNT_L_LIM_VAL,
//     };
//     //初始化PCNT单元
//     pcnt_unit_config(&pcnt_config);

//     //配置并启用输入过滤器
//     pcnt_set_filter_value(unit, 100);
//     pcnt_filter_enable(unit);

//     //设置阈值0和1，并启用事件监视
//     pcnt_set_event_value(unit, PCNT_EVT_THRES_1, PCNT_THRESH1_VAL);
//     pcnt_event_enable(unit, PCNT_EVT_THRES_1);

//     pcnt_set_event_value(unit, PCNT_EVT_THRES_0, PCNT_THRESH0_VAL);
//     pcnt_event_enable(unit, PCNT_EVT_THRES_0);

//     //初始化PCNT的计数器
//     pcnt_counter_pause(unit);
//     pcnt_counter_clear(unit);

//     //安装中断服务并添加isr回调处理器
//     pcnt_isr_service_install(0);
//     pcnt_isr_handler_add(unit, pcnt_example_intr_handler, (void *)unit);

//     //一切都设置好了，现在去计数
//     pcnt_counter_resume(unit);
// }
// static void periodic_timer_callback(void *arg);
// #define DELAY_BASE_MS 1000
// void app_main(void)
// {

//     //创建一个一次性定时器
//     const esp_timer_create_args_t periodic_timer_args = {
//         .callback = &periodic_timer_callback, //只回调一次回调
//         //定时器名称
//         .name = "periodic"};
//     esp_timer_handle_t periodic_timer;
//     /* 计时器已经创建，但还没有运行 */
//     ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
//     //创建定时器句柄，设置定时器周期为2秒，启动定时器
//     ESP_ERROR_CHECK(esp_timer_start_once(periodic_timer, 10 * 1000 * 1000));

//     LED_Init();
//     int pcnt_unit = PCNT_UNIT_0;
//     //初始化LEDC产生采样脉冲信号
//     int i = 0;
//     double f;
//     //初始化PCNT事件队列和PCNT函数
//     pcnt_evt_queue = xQueueCreate(10, sizeof(pcnt_evt_t));
//     pcnt_example_init(pcnt_unit);

//     int16_t count = 0;
//     pcnt_evt_t evt;
//     portBASE_TYPE res;
//     while (1)
//     {
//         //等待事件信息从PCNT的中断处理程序传递，一旦收到，解码事件类型并打印在串行监视器上
//         res = xQueueReceive(pcnt_evt_queue, &evt, DELAY_BASE_MS / portTICK_PERIOD_MS);
//         if (res == pdTRUE)
//         {
//             pcnt_get_counter_value(pcnt_unit, &count);
//             // ESP_LOGI(TAG, "Event PCNT unit[%d]; cnt: %d", evt.unit, count);
//             if (evt.status & PCNT_EVT_THRES_1)
//             {
//                 static bool state = true;
//                 gpio_set_level(LED_R, state);
//                 state = !state;
//                 i++;
//                 // f=i/10.0;
//                 f = i / 46.0;
//                 printf("cylinder number : %f\n", f);
//             }

//             if (evt.status & PCNT_EVT_THRES_0)
//             {
//                 ESP_LOGI(TAG, "THRES0 EVT");
//             }
//         }
//     }
// }
// static int j = 0;
// static void periodic_timer_callback(void *arg)
// {
//     int64_t time_since_boot = esp_timer_get_time();
//     j++;
//     printf("j = %d\n", j);
//     if (j >= 1)
//     {
//         printf("vQueueDelete pcnt_evt_queue\n");
//         vQueueDelete(pcnt_evt_queue);
//         pcnt_isr_handler_remove(PCNT_UNIT_0);
//     }
// }

