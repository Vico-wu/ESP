#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_task_wdt.h"

static EventGroupHandle_t s_wifi_event_group;

#define STA_START BIT0
#define SCAN_DONE BIT1

void run_on_event(void *handler_arg, esp_event_base_t base, int32_t id, void *event_data)
{
    ESP_LOGW("EVENT_HANDLE", "BASE:%s,ID:%d", base, id);

    switch (id)
    {
    case WIFI_EVENT_STA_START:
        ESP_LOGW("EVENT_HANDLE", "WIFI_EVENT_STA_START");
        xEventGroupSetBits(s_wifi_event_group, STA_START);
        break;
    case WIFI_EVENT_SCAN_DONE:
        ESP_LOGW("EVENT_HANDLE", "WIFI_EVENT_SCAN_DONE");
        xEventGroupSetBits(s_wifi_event_group, SCAN_DONE);
        break;
    default:
        break;
    }
}
void app_task(void *pt)
{
    ESP_LOGW("APP TASK", "APP TASK Created Sucessful");
    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, run_on_event, NULL);
    while (true)
    {
        xEventGroupWaitBits(s_wifi_event_group,
                            STA_START,
                            pdFALSE,
                            pdFALSE,
                            portMAX_DELAY);
        ESP_LOGW("APP TASK", "Received STA START Event, you can run APP SCAN now.");
        wifi_country_t wifi_country_config = {
            .cc = "CN",
            .schan = 1,
            .nchan = 13,
        };
        esp_wifi_set_country(&wifi_country_config);
        esp_wifi_scan_start(NULL, false);
        xEventGroupWaitBits(s_wifi_event_group,
                            SCAN_DONE,
                            pdFALSE,
                            pdFALSE,
                            portMAX_DELAY);
        ESP_LOGW("APP TASK", "Received SCAN DONE Event, you can print the result now.");
        uint16_t ap_num = 0;
        esp_wifi_scan_get_ap_num(&ap_num);
        ESP_LOGI("WIFI", "AP Count : %d", ap_num);
        uint16_t max_aps = 40;
        wifi_ap_record_t ap_records[max_aps];
        memset(ap_records, 0, sizeof(ap_records));
        uint16_t aps_count = max_aps;
        esp_wifi_scan_get_ap_records(&aps_count, ap_records);
        ESP_LOGI("WIFI", "AP Count : %d", aps_count);
        printf("%30s %s %s %s\n", "SSID", "Channel", "Strength", "MAC Addr");
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
}
void app_main(void)
{
    ESP_LOGI("WIFI", "0. Initialize NVS memory phase");
    nvs_flash_init();
    ESP_LOGI("WIFI","1. WIFI Initializing phase");
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&wifi_config);
    ESP_LOGW("WIFI", "Creating APP TASK and FreeRTOS Event Group");
    s_wifi_event_group = xEventGroupCreate();
    xTaskCreate(app_task, "APP TASK", 1024 * 12, NULL, 1, NULL);
    ESP_LOGI("WIFI", "2. WIFI Initializing phase.");
    esp_wifi_set_mode(WIFI_MODE_STA);
    ESP_LOGI("WIFI", "3. WIFI Starting phase.");
    esp_wifi_start();
    while (true)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
