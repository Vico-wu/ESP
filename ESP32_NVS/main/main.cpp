#include <stdio.h>
#include "nvs_flash.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" void app_main(void)
{
    vTaskDelay(1000/portTICK_PERIOD_MS);
    uint32_t RebootCounter = 0;
    char * name_space = "NvsTest";
    char * CounterNameKey = "Counter";
    nvs_handle_t nvs_handle;
    nvs_flash_init();
    nvs_open(name_space,NVS_READWRITE,&nvs_handle);
    //read
    nvs_get_u32(nvs_handle,CounterNameKey,&RebootCounter);
    ESP_LOGI("NVS 信息","Key  : %s",CounterNameKey);
    ESP_LOGI("NVS 信息","Value: %d",RebootCounter);
    RebootCounter ++;
    //write
    nvs_set_u32(nvs_handle,CounterNameKey,RebootCounter);
    nvs_close(nvs_handle);
    nvs_flash_deinit();
    ESP_LOGI("NVS 信息","重启次数：%d",RebootCounter);
}
