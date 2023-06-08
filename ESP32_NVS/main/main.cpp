#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

typedef struct AP
{
    char ssid[50];
    char password[50];
} ap_t;

void printMemory(unsigned char *address, int size)
{
    for (int i = 0; i < size; i++)
    {
        printf("%.2x", address[i]);
    }
    printf("\n");
}

extern "C" void app_main(void)
{
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    uint32_t RebootCounter = 0;
    char *name_space = "NvsTest";
    char *CounterNameKey = "Counter";
    nvs_handle_t nvs_handle;

    uint32_t max_ap = 2;
    ap_t aps_set[max_ap];
    ap_t aps_get[max_ap];

    nvs_flash_init();
    nvs_open(name_space, NVS_READWRITE, &nvs_handle);
    // read
    nvs_get_u32(nvs_handle, CounterNameKey, &RebootCounter);
    ESP_LOGI("NVS 信息", "Key  : %s", CounterNameKey);
    ESP_LOGI("NVS 信息", "Value: %d", RebootCounter);
    RebootCounter++;
    // write
    // 初始化的作用展示
    printMemory((unsigned char*)aps_set, sizeof(aps_set));
    memset(aps_set, 0, sizeof(aps_set));
    printMemory((unsigned char*)aps_set, sizeof(aps_set));

    for (int i = 0; i < max_ap; i++)
    {
        strcpy(aps_set[i].ssid, "bilibili");
        strcpy(aps_set[i].password, "12345678");
    }

    nvs_set_blob(nvs_handle, "aps", aps_set, sizeof(aps_set));
    nvs_set_u32(nvs_handle, CounterNameKey, RebootCounter);
    nvs_commit(nvs_handle);
ESP_LOGW
    size_t length = sizeof(aps_get);
    nvs_get_blob(nvs_handle, "aps", aps_get, &length);
    for (int i = 0; i < max_ap; i++)
    {
        ESP_LOGI("NVS 信息", "SSID: %s PASSWORD: %s", aps_get[i].ssid, aps_get[i].password);
    }
    nvs_close(nvs_handle);
    nvs_flash_deinit();
    ESP_LOGI("NVS 信息", "重启次数：%d", RebootCounter);
    uint32_t num = 255;
    printMemory((unsigned char *)&num, 4);
    num = 32767;
    printMemory((unsigned char *)&num, 4);
    num = 0xAABBCCDD;
    printMemory((unsigned char *)&num, 4);
    printMemory((unsigned char*)aps_set, sizeof(aps_set));
}
