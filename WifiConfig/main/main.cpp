#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

const char *LogWifi = "WiFi";
void task_list()
{
    char prtTaskList[250];
    vTaskList(prtTaskList);
    printf("****************************************************\n");
    printf("Task            State   Prio    Stack   Num     Core\n");
    printf("****************************************************\n");
    printf(prtTaskList);
    printf("****************************************************\n");
}
void wifi_scan(void *pt)
{
    // Phase 4
    ESP_LOGI(LogWifi, "4: Wifi 扫描阶段");
    wifi_country_t country_config = {
        .cc = "CN",
        .schan = 1,
        .nchan = 13,
        .policy = WIFI_COUNTRY_POLICY_AUTO,
    };
    esp_wifi_set_country(&country_config);
    wifi_scan_config_t scan_config{
        .show_hidden = true,
    };
    esp_wifi_scan_start(&scan_config, true);

    vTaskDelete(NULL);
}
void wifi_result_show(void *pt)
{
    uint16_t ap_num = 0;
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_num));
    ESP_LOGI(LogWifi, "AP Count : %d", ap_num);

    uint16_t max_aps = 20;
    wifi_ap_record_t ap_records[max_aps];
    memset(ap_records, 0, sizeof(ap_records));
    uint16_t aps_count = max_aps;
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&aps_count, ap_records));
    ESP_LOGI(LogWifi, "AP Count %d", aps_count);
    printf("%30s %s %s %s\n", "SSID", "频道", "强度", "MAC地址");
    for (int i = 0; i < aps_count; i++)
    {
        printf("%30s  %3d  %3d  %02X-%02X-%02X-%02X-%02X-%02X\n",
               ap_records[i].ssid,
               ap_records[i].primary,
               ap_records[i].rssi,
               ap_records[i].bssid[0],
               ap_records[i].bssid[1],
               ap_records[i].bssid[2],
               ap_records[i].bssid[3],
               ap_records[i].bssid[4],
               ap_records[i].bssid[5]);
    }
    vTaskDelete(NULL);
}
void run_on_event(void *handler_arg, esp_event_base_t base, int32_t id, void *event_data)
{
    // Event handler logic
    ESP_LOGW("EVENT HANDLE", "BASE:%s,ID:%d", base, id);
    switch (id)
    {
    case WIFI_EVENT_STA_START:
        ESP_LOGW("EVENT HANDLE", "WIFI_EVENT_STA_START");
        xTaskCreate(wifi_scan, "Wifi Scan Task", 1024 * 12, NULL, 1, NULL);
        break;
    case WIFI_EVENT_SCAN_DONE:
        ESP_LOGW("EVENT HANDLE","WIFI_EVENT_SCAN_DONE");
        xTaskCreate(wifi_result_show,"Wifi Show Task",1024*12,NULL,1,NULL);
        break;
    default:
        break;
    }
}
void app_task(void *pt)
{
    ESP_LOGI("APP TASK", "APP Task 创建完成");
    esp_event_handler_register(ESP_EVENT_ANY_BASE,
                               ESP_EVENT_ANY_ID,
                               run_on_event,
                               NULL);
    while (1)
    {
        // ESP_LOGI("APP TASK", "无聊的运行中");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

extern "C" void app_main(void)
{
    // Phase 0
    ESP_LOGI(LogWifi, "0: 初始化NVS存储");
    nvs_flash_init();
    // Phase 1
    ESP_LOGI(LogWifi, "1: WiFi 初始化阶段");
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_config);

    xTaskCreate(app_task, "App Task", 1024 * 12, NULL, 1, NULL);
    // Phase 2
    ESP_LOGI(LogWifi, "2: Wifi 初始化阶段");
    esp_wifi_set_mode(WIFI_MODE_STA);
    // Phase 3
    ESP_LOGI(LogWifi, "3: Wifi 启动阶段");
    esp_wifi_start();

    // task_list();
    while (1)
    {
        // ESP_LOGI("Main Task", "无聊的运行中");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    // vTaskDelete(NULL);
}
