#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/Task.h"
const char* TagTraffic = "交通灯";
bool LogLvl =false;
int count = 100;
char ptrTaskList[250];
// void app_main(void)
// {
//     ESP_LOGI("Counter","Tick(ms): %d",portTICK_PERIOD_MS);
//     if (true == LogLvl)
//         esp_log_level_set(TagTraffic,ESP_LOG_VERBOSE);
//     ESP_LOGE(TagTraffic,"不亮了");
//     ESP_LOGW(TagTraffic,"灯泡寿命还有5%%");
//     ESP_LOGI(TagTraffic,"交通灯寿命还有75%%");
//     ESP_LOGD(TagTraffic,"灯泡亮度1000,已使用200小时");
//     ESP_LOGV(TagTraffic,"灯丝还有5丝厚度,氧化程度0.23,寿命正常");
//     ESP_LOGE(TagTraffic,"有人闯红灯");
//     ESP_LOGW(TagTraffic,"现在是红灯,禁止通行");
//     ESP_LOGI(TagTraffic,"20秒后,绿灯");
    
//     while(1)
//     {
//         ESP_LOGI("Counter","Counter value is %d",count);
//         count ++;
//         vTaskDelay(1000/portTICK_PERIOD_MS);
//     }
// }
void app_main(void)
{
    vTaskList(ptrTaskList);
    printf("*****************************************\n");
    printf("Task     State     Prio     Stack     Num\n");
    printf("*****************************************\n");
    printf(ptrTaskList);
    printf("*****************************************\n");

}