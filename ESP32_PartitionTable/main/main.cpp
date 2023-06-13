#include <stdio.h>
#include "nvs_flash.h"
#include "esp_log.h"
#include <cstring>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"

typedef struct ap
{
    char SSID[50];
    char Password[50];
} ap_t;
const char *Debug = "诊断信息";
extern "C" void app_main(void)
{
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    ap_t ap_set;
    ap_t ap_get;
    size_t length = sizeof(ap_get);
    // 初始化分区
    const char *part_ota0 = "nvs";
    const char *TestNamespace = "TestOta0";
    const char *KeyName = "WifiData";
    nvs_handle_t nvs_handle;
    ESP_LOGI(Debug, "Flash Init Partion: %s", esp_err_to_name(nvs_flash_init_partition(part_ota0)));
    // 打开分区，创建分区句柄
    ESP_LOGI(Debug, "Open From Partition: %s", esp_err_to_name(nvs_open_from_partition(part_ota0, TestNamespace, NVS_READWRITE, &nvs_handle)));
    // 获取OTA0 分区数据
    ESP_LOGI(Debug, "Get Blob: %s", esp_err_to_name(nvs_get_blob(nvs_handle, KeyName, &ap_get, &length)));
    // ap_set.SSID ="Vico_123";
    // ap_set.Password = "HelloWorld";
    if (0 == memcmp(&ap_get.SSID, &ap_set.SSID, sizeof(ap_set.SSID)))
    {
        strcpy(ap_set.SSID, "VICO_123");
        strcpy(ap_set.Password, "HelloWorld");
    }
    else
    {
        strcpy(ap_set.SSID, "VICO_456");
        strcpy(ap_set.Password, "HaHaHaHa");
    }
    // ESP_LOGI("诊断信息", "已经配置号密码，准备写入");

    ESP_LOGI(Debug, "Set Blob: %s", esp_err_to_name(nvs_set_blob(nvs_handle, KeyName, &ap_set, sizeof(ap_set))));
    ESP_LOGI(Debug, "Nvs Commit: %s", esp_err_to_name(nvs_commit(nvs_handle)));
    // 关闭分区
    nvs_close(nvs_handle);
    // 取消挂在分区
    ESP_LOGI(Debug, "Deinit Partition: %s", esp_err_to_name(nvs_flash_deinit_partition(part_ota0)));
    ESP_LOGI("Wifi Info", "SSID : %s Password : %s", ap_get.SSID, ap_get.Password);
    // ESP_LOGI(Debug,"SSID: %s",ap_get.SSID);
}
